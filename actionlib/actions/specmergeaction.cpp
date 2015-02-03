#include <QVector>
#include <QThreadPool>
#include "specmergeaction.h"
#include "specdeleteaction.h"
#include "specaddfoldercommand.h"
#include "specmulticommand.h"
#include "specmergerunnable.h"

#include "names.h"
#include "specprofiler.h"
#include <QProgressDialog>
#include "specdataitem.h"
#include "specmergedialog.h"

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

	// setup
	assembleSelection();
	specMergeRunnable::dataContainer *originalData = new specMergeRunnable::dataContainer ;
	QVector<specModelItem*> input = pointers.toVector(), output(pointers.size(),0) ;
	QThreadPool pool ;
	originalData->input = &input;
	originalData->output= &output;
	originalData->criteria = &criteria ;
	originalData->threadPool = &pool ;
	originalData->cmode = spectralAdaptation ;
	originalData->itemIndexes = QVector<int>(pointers.size()) ;
	for (int i = 0 ; i < pointers.size() ; ++i)
		originalData->itemIndexes[i] = i ;

	// run
	pool.start(new specMergeRunnable(originalData));
	pool.waitForDone() ;

	// sort out the items
	QList<specModelItem*> toInsert, toDelete ;
	for (int i = 0 ; i < pointers.size() ; ++i)
	{
		specModelItem *newItem = output[i],
				*oldItem = input[i] ;
		if (oldItem) toDelete << oldItem ;
		if (!newItem) continue ; // no item to insert
		QModelIndex insertionIndex(model->index(oldItem)) ;
		if (!model->insertItems(QList<specModelItem*>() << newItem,
					insertionIndex.parent(),
					insertionIndex.row()))
			delete newItem ;
		else
			toInsert << newItem ;
	}

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
