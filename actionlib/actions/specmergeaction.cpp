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
#include "specfiltergenerator.h"

class sortItemsByDescriptionFunctor
{
private:
	const QStringList* comparisons ;
public:
	sortItemsByDescriptionFunctor(const QStringList* c) : comparisons(c) {}
	bool operator()(specModelItem const* a, specModelItem const* b)
	{
		foreach(QString value, *comparisons)
		{
			if(a->descriptor(value, true) == b->descriptor(value, true)) continue ;
			if(a->descriptorProperties(value) & spec::numeric)
				return a->descriptor(value).toDouble() < b->descriptor(value).toDouble() ;
			return a->descriptor(value, true) < b->descriptor(value, true) ;
		}
		if(!b->dataSize()) return false ;
		if(!a->dataSize()) return true ;
		return a->sample(0).x() < b->sample(0).x() ;
	}
};

class mergeActionThread : public specWorkerThread
{
private:
	specMultiCommand* Command ;
	specModel* model ;
	QList<stringDoublePair > criteria ;
	bool spectralAdaptation ;
	QList<specModelItem*> items ;
	QList<specModelItem*> toBeDeleted ;
	QList<specModelItem*> newlyInserted ;

	bool cleanUp() // TODO parentClass
	{
		if(!toTerminate) return false ;
		items.clear();
		delete Command ;
		return true ;
	}

	void mergeItems(specDataItem* newItem, const specDataItem* other) const
	{
		if (!newItem || !other) return ;
		if(spectralAdaptation)
		{
			specDataItem* correctedItem = new specDataItem(*other) ;
			specFilterGenerator filterGenerator ;
			filterGenerator.calcOffset();
			filterGenerator.calcSlope();
			filterGenerator.setReference(newItem->dataMap()) ;
			specDataPointFilter filter = filterGenerator.generateFilter(correctedItem) ;
			if (filter.valid())
				correctedItem->addDataFilter(filter) ;
			other = correctedItem ;
		}

		*newItem += *other ;
		if (spectralAdaptation) delete other ;
	}

	bool itemsAreEqual(specModelItem* first, specModelItem* second, const QList<stringDoublePair>& criteria) ;

	static const int offset = 10 ;

public:
	mergeActionThread(specModel* Model, QList<specModelItem*> itms, QList<stringDoublePair> crit, bool sadap)
		: specWorkerThread(itms.size() + offset),
		  Command(0),
		  model(Model),
		  criteria(crit),
		  spectralAdaptation(sadap),
		  items(itms)
	{
	}

	specUndoCommand* command()
	{
		specUndoCommand* oc = Command ;
		Command = 0 ;
		return oc ;
	}

	void run()
	{
		emit progressValue(0);
		specProfiler profiler("run merge command thread:") ;
		QList<specModelItem*> toBeDeleted ;
		QList<specModelItem*> newlyInserted ;

		// Create and insert new merged items
		int total = items.size() ;
		QStringList comparisons ;
		foreach(stringDoublePair pair, criteria)
		comparisons << pair.first ;
		sortItemsByDescriptionFunctor sorter(&comparisons) ;
		while(!items.isEmpty())
		{
			int progress = total - items.size() ;
			emit progressValue(progress);
			// form chunk -> only merge if same parent! (new logic)
			QList<specModelItem*> chunk ;
			specFolderItem* const parent = items.first()->parent() ;
			const int row = parent->childNo(items.first()) ;
			while(!items.isEmpty() && items.first()->parent() == parent && parent->childNo(items.first()) == row + chunk.size())
				chunk << items.takeFirst() ;

			QList<specModelItem*> toInsert ;
			int chunkSize = chunk.size() ;
			qSort(chunk.begin(), chunk.end(), sorter) ;
			QList<specModelItem*> chunkDelete ;
			while(!chunk.isEmpty())
			{
				emit progressValue(progress + chunkSize - chunk.size()) ;
				if(cleanUp()) return ;
				specDataItem* newItem = new specDataItem(QVector<specDataPoint>(), QHash<QString, specDescriptor>()) ;
				QList<specModelItem*> toMergeWith ;
				// look for items to merge with
				// consider iterator-erase
				do toMergeWith << chunk.takeFirst() ;
				while(!chunk.isEmpty() && itemsAreEqual(toMergeWith.first(), chunk.first(), criteria)) ;

				if (toMergeWith.size() == 1) continue ;
				chunkDelete << toMergeWith ;

				foreach(specModelItem* other, toMergeWith)
					mergeItems(newItem, dynamic_cast<specDataItem*>(other)) ;
				newItem->flatten();
				toInsert << newItem ;
			}
			if(model->insertItems(toInsert, model->index(parent), row))
			{
				newlyInserted << toInsert ;
				toBeDeleted << chunkDelete ;
			}
			else
				foreach(specModelItem* item, toInsert)
					delete item ;
		}
		emit progressValue(total) ;

		Command = new specMultiCommand ;
		Command->setMergeable(false) ;

		// compile description
		QStringList criteriaDescription ;
		foreach(const stringDoublePair & comparison, criteria)
		criteriaDescription << comparison.first + " (" + QString::number(comparison.second) + ")" ;
		QString description = tr("Merge ")
				      + QString::number(toBeDeleted.size())
				      + tr(" items to ")
				      + QString::number(newlyInserted.size())
				      + tr(" items.") ;
		if(spectralAdaptation)
			description += tr(" Items aligned prior to merging.") ;
		if(!criteriaDescription.isEmpty())
			(description += tr(" Criteria (Tolerance): ")) += criteriaDescription.join(", ") ;
		Command->setText(description) ;

		specAddFolderCommand* insertionCommand = new specAddFolderCommand(Command) ;
		specDeleteCommand* deletionCommand = new specDeleteCommand(Command) ;
		Command->setParentObject(model) ;
		insertionCommand->setItems(newlyInserted) ;
		deletionCommand->setItems(toBeDeleted) ;

		cleanUp() ;
		emit progressValue(total + offset);
	}
};

specMergeAction::specMergeAction(QObject* parent)
	: specRequiresDataItemAction(parent),
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
	if(selection.size() < 2) return 0 ;
	qSort(pointers.begin(), pointers.end(), specModel::lessThanItemPointer) ;

	// let user define similarities
	QList<stringDoublePair > criteria ;
	bool spectralAdaptation ;
	dialog->setDescriptors(model->descriptors(), model->descriptorProperties()) ;
	if(dialog->exec() != QDialog::Accepted) return 0 ;
	dialog->getMergeCriteria(criteria, spectralAdaptation);
	mergeActionThread mat(model, pointers, criteria, spectralAdaptation) ;
	mat.run();
	return mat.command() ;
}

bool mergeActionThread::itemsAreEqual(specModelItem* first, specModelItem* second, const QList<stringDoublePair>& criteria)  // TODO insert actual strings into merge criteria
{
	if(!first || !second)
		return false ;
	// TODO may need to be revised if descriptor contains actual numeric value, not QString
	foreach(stringDoublePair criterion, criteria)
	{
		double tolerance = criterion.second ;
		if(tolerance != -1)
		{
			double a = first->descriptor(criterion.first).toDouble(),
			       b = second->descriptor(criterion.first).toDouble() ;
			if(!(b - tolerance <= a && a <= b + tolerance)) return false ;
		}
		else if(first->descriptor(criterion.first, true) != second->descriptor(criterion.first, true))
			return false ;
	}
	return true ;
}
