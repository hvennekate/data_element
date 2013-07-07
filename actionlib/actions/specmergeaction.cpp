#include "specmergeaction.h"
#include "specdeletecommand.h"
#include "specaddfoldercommand.h"
#include "specmulticommand.h"
#include "specspectrumplot.h"

#include "names.h"
#include "specprofiler.h"
#include <QProgressDialog>
#include "specdataitem.h"
#include "specmergedialog.h"

#include "specworkerthread.h"

class sortItemsByDescriptionFunctor
{
private:
	const QStringList *comparisons ;
public:
	sortItemsByDescriptionFunctor(const QStringList *c) : comparisons(c) {}
	bool operator() (specModelItem const* a, specModelItem const* b)
	{
		foreach(QString value, *comparisons)
		{
			if (a->descriptor(value, true) == b->descriptor(value, true)) continue ;
			if (a->descriptorProperties(value) & spec::numeric)
				return a->descriptor(value).toDouble() <b->descriptor(value).toDouble() ;
			return a->descriptor(value, true) < b->descriptor(value, true) ;
		}
		if (!b->dataSize()) return false ;
		if (!a->dataSize()) return true ;
		return a->sample(0).x() < b->sample(0).x() ;
	}
};

class mergeActionThread : public specWorkerThread
{
private:
	specMultiCommand *Command ;
	specModel* model ;
	QList<stringDoublePair > criteria ;
	bool spectralAdaptation ;
	QList<specDataItem*> items ;
	QVector<specDataItem*> toBeDeleted ;
	QVector<specModelItem*> newlyInserted ;

	bool cleanUp() // TODO parentClass
	{
		if (!toTerminate) return false ;
		foreach(specModelItem* item, newlyInserted)
			delete item ;
		newlyInserted.clear();
		items.clear();
		toBeDeleted.clear();
		delete Command ;
		return true ;
	}

	bool itemsAreEqual(specModelItem* first, specModelItem* second, const QList<stringDoublePair>& criteria) ;

	static const int offset = 10 ;

public:
	mergeActionThread(specModel* Model, QList<specDataItem*> itms, QList<stringDoublePair> crit, bool sadap)
		: specWorkerThread(itms.size()+offset),
		  Command(0),
		  model(Model),
		  criteria(crit),
		  spectralAdaptation(sadap),
		  items(itms)
	{
	}

	specUndoCommand *command()
	{
		specUndoCommand *oc = Command ;
		Command = 0 ;
		return oc ;
	}

	void run()
	{
		emit progressValue(0);
		specProfiler profiler("run merge command thread:") ;
		QVector<specDataItem*> toBeDeleted ;
		QVector<specModelItem*> newlyInserted ;

		// Create and insert new merged items
		int total = items.size() ;
		QStringList comparisons ;
		foreach(stringDoublePair pair, criteria)
			comparisons << pair.first ;
		sortItemsByDescriptionFunctor sorter(&comparisons) ;
		while (!items.isEmpty())
		{
			int progress = total-items.size() ;
			emit progressValue(progress);
			// form chunk -> only merge if same parent! (new logic)
			QList<specDataItem*> chunk ;
			specFolderItem * const parent = items.first()->parent() ;
			const int row = parent->childNo(items.first()) ;
			while (!items.isEmpty() && items.first()->parent() == parent && parent->childNo(items.first()) == row + chunk.size())
				chunk << items.takeFirst() ;

			QVector<specModelItem*> toInsert ;
			int chunkSize =chunk.size() ;
			qSort(chunk.begin(), chunk.end(), sorter) ;
			while (!chunk.isEmpty())
			{
				emit progressValue(progress+chunkSize-chunk.size()) ;
				if (cleanUp()) return ;
				specDataItem *newItem = new specDataItem(QVector<specDataPoint>(),QHash<QString,specDescriptor>()) ;
				QList<specDataItem*> toMergeWith ;
				// look for items to merge with
				do toMergeWith << chunk.takeFirst() ;
				while (!chunk.isEmpty() && itemsAreEqual(toMergeWith.first(), chunk.first(), criteria)) ;

				toBeDeleted << toMergeWith.toVector() ;
				// if there are others, do the merge
				if (!toMergeWith.isEmpty())
				{
					if (spectralAdaptation)
					{
						foreach(specModelItem* other,toMergeWith)
						{
							// generate reference spectrum
							QMap<double,double> reference ;
							newItem->revalidate();
							for (size_t i = 0 ; i < newItem->dataSize() ; ++i)
							{
								const QPointF point = newItem->sample(i) ;
								reference[point.x()] = point.y() ;
							}

							// define spectral ranges
							specRange *range = new specRange(reference.begin().key(), (reference.end() -1).key()) ;
							QwtPlotItemList ranges ;
							ranges << range ; // DANGER
							// protection against nan values (make sure spectra do indeed overlap)
							int overlappingCount = 0 ;
							for(size_t i = 0 ; i < other->dataSize() ; ++i)
							{
								overlappingCount += range->contains(other->sample(i).x()) ;
								if (overlappingCount == 2) break ;
							}
							if (overlappingCount == 2)
							{
								QwtPlotItemList spectra ;
								for (QList<specDataItem*>::iterator i = toMergeWith.begin() ; i != toMergeWith.end() ; ++i)
									spectra << (QwtPlotItem*) *i ;

								// perform spectral adaptation
								// TODO NAN protection
								specMultiCommand *correctionCommand= specSpectrumPlot::generateCorrectionCommand(ranges, QwtPlotItemList() << (QwtPlotItem*) other, reference, model, true, true) ;
								correctionCommand->redo();
								*newItem += *((specDataItem*) other) ;
								correctionCommand->undo();
								delete correctionCommand ;
							}
							else
								*newItem += *((specDataItem*) other) ;
							delete range ;
						}
					}
					else
						foreach(specModelItem* other, toMergeWith)
							*newItem += *((specDataItem*) other) ; // TODO check this cast
					newItem->flatten();
				}
				toInsert << newItem ; // this creates overhead if  there are many items not to be merged...
			}
			if (model->insertItems(toInsert.toList(), model->index(parent), row))
				newlyInserted += toInsert ; // TODO else...
		}
		emit progressValue(total) ;

		Command = new specMultiCommand ;
		Command->setParentObject(model) ;
		Command->setMergeable(false) ;

		// preparing insertion command
		specAddFolderCommand *insertionCommand = new specAddFolderCommand(Command) ;
		QModelIndexList insertList = model->indexList(newlyInserted.toList()) ;
		insertionCommand->setItems(insertList) ;
		insertionCommand->setParentObject(model) ;


		// prepare to delete the old items
		specDeleteCommand *deletionCommand = new specDeleteCommand(Command) ;
		QModelIndexList deleteList = model->indexList(toBeDeleted.toList()) ;
		deletionCommand->setItems(deleteList) ;
		deletionCommand->setParentObject(model) ;

		// compile description
		QStringList criteriaDescription ;
		foreach(const stringDoublePair& comparison, criteria)
			criteriaDescription << comparison.first + " (" + QString::number(comparison.second) + ")" ;
		QString description = tr("Merge ")
				+ QString::number(toBeDeleted.size())
				+ tr(" items to ")
				+ QString::number(newlyInserted.size())
				+ tr(" items.") ;
		if (spectralAdaptation)
			description += tr(" Items aligned prior to merging.") ;
		if (!criteriaDescription.isEmpty())
			(description += tr(" Criteria (Tolerance): ")) += criteriaDescription.join(", ") ;
		Command->setText(description) ;
		cleanUp() ;
		emit progressValue(total+offset);
	}
};

specMergeAction::specMergeAction(QObject *parent)
	: specItemAction(parent),
	  dialog(new specMergeDialog(0))
{
	setIcon(QIcon(":/merge.png")) ;
	setToolTip(tr("Merge items")) ;
	setWhatsThis(tr("Merge selected data items.  You may define criteria and processing options for merging."));
	setText(tr("Merge items...")) ;
	setShortcut(tr("Ctrl+M"));
}

specMergeAction::~specMergeAction()
{
	delete dialog ;
}

specUndoCommand* specMergeAction::generateUndoCommand()
{
	specProfiler profiler("Prepare merge commands:") ;
	if (selection.size() < 2) return 0 ;
	QList<specDataItem*> items ;
	foreach(QModelIndex index, selection)
		items << dynamic_cast<specDataItem*>(model->itemPointer(index)) ;
	items.removeAll(0) ;
	if (items.isEmpty()) return 0 ;
	qSort(items.begin(), items.end(), specModel::pointerIsLessComparison) ;

	// let user define similarities
	QList<stringDoublePair > criteria ;
	bool spectralAdaptation ;
	dialog->setDescriptors(model->descriptors(), model->descriptorProperties()) ;
	if (dialog->exec() != QDialog::Accepted) return 0 ;
	dialog->getMergeCriteria(criteria, spectralAdaptation);
	if (criteria.isEmpty()) return 0 ;
	mergeActionThread mat(model,items,criteria,spectralAdaptation) ;
	mat.run();
	return mat.command() ;
}

bool mergeActionThread::itemsAreEqual(specModelItem* first, specModelItem* second, const QList<stringDoublePair>& criteria) // TODO insert actual strings into merge criteria
{
	if (!first || !second)
		return false ;
	// TODO may need to be revised if descriptor contains actual numeric value, not QString
	foreach(stringDoublePair criterion, criteria)
	{
		double tolerance = criterion.second ;
		if (tolerance != -1)
		{
			double a = first->descriptor(criterion.first).toDouble(),
					b = second->descriptor(criterion.first).toDouble() ;
			if (!(b-tolerance <= a && a <= b+tolerance)) return false ;
		}
		else if (first->descriptor(criterion.first,true) != second->descriptor(criterion.first,true))
			return false ;
	}
	return true ;
}
