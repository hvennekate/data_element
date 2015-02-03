#include "specmergeaction.h"
#include "specdeleteaction.h"
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

bool mergeActionThread::cleanUp() // TODO parentClass
{
	if(!toTerminate) return false ;
	items.clear();
	toBeDeleted.clear();
	foreach (specModelItem* item, toInsert)
		delete item ;
	toInsert.clear();
	return true ;
}

void mergeActionThread::mergeItems(specDataItem* newItem, const specDataItem* other) const
{
	if (!newItem || !other) return ;
	specDataItem* correctedItem = 0 ;
	if(spectralAdaptation != spec::noCorrection)
	{
		QMap<double, double> reference = newItem->dataMap() ;
		if (!reference.isEmpty())
		{
			specFilterGenerator filterGenerator ;
			filterGenerator.calcOffset();
			if (spectralAdaptation == spec::offsetAndSlope)
				filterGenerator.calcSlope();
			filterGenerator.setReference(reference) ;
			correctedItem = new specDataItem(*other) ;
			specDataPointFilter filter = filterGenerator.generateFilter(correctedItem) ;
			if (filter.valid())
			{
				correctedItem->addDataFilter(filter) ;
				other = correctedItem ; // alternatively:  const_cast
			}
		}
	}

	*newItem += *other ;
	delete correctedItem ;
}

mergeActionThread::mergeActionThread(const QList<specModelItem*>& itms,
				     const specDescriptorComparisonCriterion::container& crit,
				     spec::correctionMode sadap)
	: specWorkerThread(itms.size()),
	  criteria(crit),
	  spectralAdaptation(sadap),
	  items(itms)
{
}

mergeActionThread::~mergeActionThread() { cleanUp() ; }

#define PASSLISTFUNCTIONMACRO(FUNCTIONNAME,LISTNAME) \
	QList<specModelItem*> mergeActionThread::FUNCTIONNAME() { \
	QList<specModelItem*> result = LISTNAME; \
	LISTNAME.clear(); \
	return result ; }
PASSLISTFUNCTIONMACRO(getItemsToDelete, toBeDeleted)
PASSLISTFUNCTIONMACRO(getItemsToInsert, toInsert)

void mergeActionThread::run()
{
//	emit progressValue(0);
	specProfiler profiler("run merge command thread:") ;
	// sort the pointers

	// Create and insert new merged items
	int total = items.size() ;
	while(!items.isEmpty())
	{
//		emit progressValue(total - items.size());
		if(cleanUp()) return ;

		specModelItem *comparisonItem = items.first() ;

		QList<specModelItem*> toMergeWith ;
		QList<specModelItem*>::iterator i = items.begin() ;
		while (i != items.end())
		{
			if (specDescriptorComparisonCriterion::itemsEqual(comparisonItem, *i, criteria))
			{
				toMergeWith << *i ;
				i = items.erase(i) ;
			}
			else
				++i ;
		}
		if (toMergeWith.size() == 1) continue ;

		specDataItem* newItem = new specDataItem() ;
		foreach(specModelItem* other, toMergeWith)
			mergeItems(newItem, dynamic_cast<specDataItem*>(other));
		newItem->flatten();

		toBeDeleted << toMergeWith ;
		toInsert << newItem ;
	}
//	emit progressValue(total) ;

	finish() ; // enable cleanUp()
}

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

	// let user define similarities
	specDescriptorComparisonCriterion::container criteria ;
	spec::correctionMode spectralAdaptation;
	dialog->setDescriptors(model->descriptors()) ;
	if(dialog->exec() != QDialog::Accepted) return 0 ;
	dialog->getMergeCriteria(criteria, spectralAdaptation);

	mergeActionThread mat(pointers, criteria, spectralAdaptation) ;
	mat.start();
	mat.wait() ;
	while(!mat.isFinished()) ; // just to be sure

	QList<specModelItem*> toInsert = mat.getItemsToInsert() ;
	if (!model->insertItems(toInsert, insertionIndex, insertionRow))
	{
		foreach(specModelItem* item, toInsert)
			delete item ;
		return 0 ;
	}
	QList<specModelItem*> toDelete = mat.getItemsToDelete() ;

	// Prepare umbrella command
	specMultiCommand* Command = new specMultiCommand ;
	Command->setMergeable(false) ;

	// compile description
	QString description = tr("Merge ")
			      + QString::number(toDelete.size())
			      + tr(" items to ")
			      + QString::number(toInsert.size())
			      + tr(" items.") ;
	if(spectralAdaptation != spec::noCorrection)
		description += tr(" Items aligned prior to merging (offset")
				+ (spectralAdaptation == spec::offsetAndSlope ?
					tr(" and slope") : QString())
				+ tr(").") ;

	QStringList criteriaDescription ;
	foreach(const specDescriptorComparisonCriterion& comparison, criteria)
		criteriaDescription << comparison.descriptor()
				       + (comparison.tolerance() ?
						  " (" + QString::number(comparison.tolerance()) + ")"
						: QString());
	if(!criteriaDescription.isEmpty())
		(description += tr(" Criteria (Tolerance): ")) += criteriaDescription.join(", ") ;

	Command->setText(description) ;

	// Create actual worker commands
	specAddFolderCommand* insertionCommand = new specAddFolderCommand(Command) ;
	Command->setParentObject(model) ;
	insertionCommand->setItems(toInsert) ;
	specDeleteAction::command(model, toDelete, Command) ;

	return Command ;
}
