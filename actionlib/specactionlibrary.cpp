#include "specactionlibrary.h"
#include "specdeleteaction.h"
#include "specaddfolderaction.h"
#include "specmovecommand.h"
#include "speccopyaction.h"
#include "specpasteaction.h"
#include "speccutaction.h"
#include "changeplotstyleaction.h"
#include "spectreeaction.h"
#include "specflattentreeaction.h"
#include "specmergeaction.h"
#include "specremovedataaction.h"
#include "specaveragedataaction.h"
#include "specaddsvgitem.h"
#include "specnewmetaitemaction.h"
#include "kinetic/specmetaview.h"
#include "specaddconnectionsaction.h"
#include "log/speclogview.h"
#include "specimportspecaction.h"
#include "names.h"
#include "speclabelaction.h"
#include <QUndoView>
#include "utility-functions.h"
#include "genericexportaction.h"
#include "specitempropertiesaction.h"
#include <QClipboard>
#include <QApplication>
#include "specaddfitaction.h"
#include "specconductfitaction.h"
#include "specremovefitaction.h"
#include "spectogglefitstyleaction.h"
#include "specmetaitem.h"
#include "specselectconnectedaction.h"
#include "specsetmultilineaction.h"
#include "specsvgitem.h"
#include "specdescriptoreditaction.h"
#include <QProgressDialog>
#include "spectiltmatrixaction.h"
#include "specspectrumcalculatoraction.h"
#include "specplotwidget.h"
#include <QMessageBox>
#include <specdockwidget.h>
#include "specnormalizeaction.h"

specDockWidget* specActionLibrary::undoWidget()
{
	return dockWidget ;
}

QList<QWidget*> specHistoryWidget::mainWidgets() const
{
	return QList<QWidget*>() << undoView ;
}

specHistoryWidget::specHistoryWidget(QUndoStack* stack, QWidget *parent)
	: specDockWidget(tr("History"), parent),
	  undoView(new QUndoView(stack, this))
{
	setWhatsThis("Undo history.  Click on any command to forward/rewind to that particular state.");
	toggleViewAction()->setIcon(QIcon(":/undoView.png")) ;
	toggleViewAction()->setWhatsThis(tr("Shows and hides the undo history."));
	setupWindow(0);
}

specActionLibrary::specActionLibrary(QObject *parent) :
	QObject(parent),
	progress(0),
	purgeUndoAction(new QAction(QIcon::fromTheme("user-trash"),tr("Clear history"),this)),
	dockWidget(0)
{
	undoStack = new QUndoStack(this) ; // TODO consider making this a plain variable
	connect(undoStack,SIGNAL(cleanChanged(bool)),this,SLOT(stackClean(bool))) ;
	connect(purgeUndoAction,SIGNAL(triggered()),this,SLOT(purgeUndo())) ;
	purgeUndoAction->setWhatsThis(tr("Deletes the complete undo history -- use with care and don't point this at humans."));

	dockWidget = new specHistoryWidget(undoStack,qobject_cast<QWidget*>(parent ? parent->parent() :0)) ;
}

void specActionLibrary::stackClean(const bool &b) // This is due to the stack being clean
{ // when the window is UNmodified ... (chiams are bad!)
	emit stackModified(!b) ;
}

void specActionLibrary::push(specUndoCommand * cmd)
{
	if (!cmd) return ;
	undoStack->push(cmd) ;
}

void specActionLibrary::addNewAction(QToolBar *bar, specUndoAction *action)
{
	action->setLibrary(this) ;
	bar->addAction(action) ;
}

QAction* specActionLibrary::redoAction(QObject* target)
{
	QAction *redoAction = undoStack->createRedoAction(target) ;
	redoAction->setIcon(QIcon::fromTheme("edit-redo")) ;
	redoAction->setToolTip(tr("Redo")) ;
	redoAction->setWhatsThis(tr("Redo - Redoes what has been undone by clicking the undo button.\nNote that all of your undo history (including possible redos) will be saved along with your work and will be available again upon loading your file again."));
	return redoAction ;
}

QAction* specActionLibrary::undoAction(QObject* target)
{
	QAction *undoAction = undoStack->createUndoAction(target) ;
	undoAction->setIcon(QIcon::fromTheme("edit-undo"));
	undoAction->setToolTip(tr("Undo")) ;
	undoAction->setWhatsThis(tr("Undo - By clicking this button you can revert changes.  To \"undo the undo\" click the redo button right next door.\nNote that all of your undo history will be saved along with your work and will be available again upon loading your file again."));
	return undoAction ;
}

QToolBar* specActionLibrary::toolBar(QWidget *target)
{
	QToolBar *bar = new QToolBar(target) ;
	bar->setContentsMargins(0,0,0,0) ;
	bar->setIconSize(QSize(20,20)) ;

	specView* view = dynamic_cast<specView*>(target) ;
	specDataView*   dataView   = dynamic_cast<specDataView*>  (target) ;
	specMetaView*   metaView   = dynamic_cast<specMetaView*>  (target) ;
	specLogView*    logView    = dynamic_cast<specLogView*>   (target) ;
	specPlot*       plot       = dynamic_cast<specPlot*>      (target) ;
	specPlotWidget* plotWidget = dynamic_cast<specPlotWidget*>(target) ;

	if (view && view->model())
	{
		addParent(view) ;
		addParent(view->model()) ;
		addNewAction(bar, new specAddFolderAction(target)) ;
		if (metaView)
			addNewAction(bar, new specNewMetaItemAction(target));
		else
		{
			addNewAction(bar, new specImportSpecAction(target)) ;
			addNewAction(bar, new specTreeAction(target)) ;
			addNewAction(bar, new specFlattenTreeAction(target)) ;
		}
		if (dataView || metaView)
			addNewAction(bar, new specAddSVGItemAction(target)) ;
		addNewAction(bar,new genericExportAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specCopyAction(target)) ;
		addNewAction(bar, new specCutAction(target)) ;
		addNewAction(bar, new specPasteAction(target)) ;
		addNewAction(bar, new specDeleteAction(target)) ;
		bar->addSeparator() ;
		if (metaView || logView)
		{
			bar->addAction(undoAction(view)) ;
			bar->addAction(redoAction(view)) ;
			bar->addSeparator() ;
		}
		if (dataView)
		{
			addNewAction(bar, new specMergeAction(target)) ;
			addNewAction(bar, new specTiltMatrixAction(target)) ;
			addNewAction(bar, new specDescriptorEditAction(target)) ;
			bar->addSeparator() ;
			addNewAction(bar, new specRemoveDataAction(target)) ;
			addNewAction(bar, new specAverageDataAction(target)) ;
			addNewAction(bar, new specSpectrumCalculatorAction(target)) ;
			addNewAction(bar, new specNormalizeAction(target)) ;
		}
		addNewAction(bar, new specItemPropertiesAction(target)) ;
		if (metaView)
		{
			addNewAction(bar, new specAddConnectionsAction(target)) ;
			addNewAction(bar, new specSelectConnectedAction(target)) ;
			addNewAction(bar, new specAddFitAction(target)) ;
			addNewAction(bar, new specRemoveFitAction(target)) ;
			addNewAction(bar, new specToggleFitStyleAction(target));
			addNewAction(bar, new specConductFitAction(target)) ;
		}
		if (logView)
			addNewAction(bar, new specDescriptorEditAction(target)) ;
		bar->addSeparator() ;
		if (dataView || metaView)
			addNewAction(bar, new changePlotStyleAction(target)) ;
		bar->setWindowTitle(tr("Items"));
	}

	if (plot)
	{
		addParent(plot);
		addNewAction(bar, new specTitleAction(target)) ;
		addNewAction(bar, new specXLabelAction(target)) ;
		addNewAction(bar, new specYLabelAction(target)) ;
		bar->addActions(plot->actions());
		bar->setWindowTitle(tr("Plot"));
	}

	if (plotWidget)
	{
		delete bar ;
		bar = plotWidget->createToolbar() ;
		bar-> addSeparator() ;
		bar-> addAction(purgeUndoAction) ;
		bar-> addSeparator() ;
		bar-> addAction(undoAction(this)) ;
		bar-> addAction(redoAction(this)) ;
		bar->setWindowTitle(tr("Main"));
	}

	return bar ;
}

void specActionLibrary::addDragDropPartner(specModel* model)
{
	if (!partners.contains(model))
		partners << model ;
	model->setDropBuddy(this) ;
}

void specActionLibrary::addParent(QObject *pointer)
{
	if (!parents.contains(pointer))
		parents << pointer ;
}

QObject* specActionLibrary::parentId(int num)
{
	return parents[num] ;
}

void specActionLibrary::writeToStream(QDataStream &out) const
{
	out << qint32(undoStack->count()) ;
	out << qint32(undoStack->index()) ;
	int progressToGo = 0, progressStart = 0 ;
	if (progress)
	{
		progressStart = progress->value() ;
		progressToGo = progress->maximum() - progressStart ;

	}
	for (int i = 0 ; i < undoStack->count() ; ++i)
	{
		if (progress) progress->setValue(progressStart + (i*progressToGo)/undoStack->count());
		specUndoCommand* command = (specUndoCommand*) undoStack->command(i) ;
		out << type(command->id()) << qint32(parents.indexOf(command->parentObject()))
			<< *command ;
	}
	undoStack->setClean();
}

specStreamable* specActionLibrary::factory(const type &t) const
{
	return commandGenerator.commandById(t) ;
}

void specActionLibrary::readFromStream(QDataStream &in)
{
	qint32 num, position ;
	in >> num >> position ; // TODO really rely on num?

	foreach(QObject* parent, parents)
		qDebug() << "Parent: " << parent->objectName() ;

	type t ;
	QVector<qint32> parentIndex(num,0) ;
	int progressToGo = 0, progressStart = 0 ;
	if (progress)
	{
		progressStart = progress->value() ;
		progressToGo = progress->maximum() - progressStart ;

	}

	for (int i = 0 ; i < num ; ++i)
	{
		if (progress) progress->setValue(progressStart + (i*progressToGo)/num);
		in >> t >> parentIndex[i] ;
		specStreamable *streamable = produceItem(in) ;
		specUndoCommand *command = dynamic_cast<specUndoCommand*>(streamable) ;
		if (!command)
		{
			qDebug() << "Error reading command no." << i << "of type" << t ;
			undoStack->clear();
			parentIndex.clear();
			delete streamable ;
			continue ;
		}
		qDebug() << "Reading item:" << i << "total count:" << undoStack->count() << "/" << num ;
		////// For manually discarding corrupted undo commands:
		//		if (QMessageBox::question(0,tr("Really read?"),
		//					  tr("Really read command no. ")
		//					  + QString::number(i)
		//					  + tr("?  Description is:\n")
		//					  + command->text(),
		//					  QMessageBox::Yes | QMessageBox::No,
		//					  QMessageBox::Yes)
		//				== QMessageBox::Yes)
		undoStack->push(command) ;
		//		else
		//			delete command ;
	}

	undoStack->setIndex(position) ;
	for (int i = 0 ; i < undoStack->count() ; ++i)
		((specUndoCommand*) undoStack->command(i))->setParentObject(parents[parentIndex[i]]) ;
	qDebug() << "to be read:" << num << "actually on stack:" << undoStack->count() ;
	undoStack->setClean();
}

void specActionLibrary::setLastRequested(const QModelIndexList &list)
{
	lastRequested = list ;
}

int specActionLibrary::moveInternally(const QModelIndex &parent, int row, specView* target)
{
	int count = lastRequested.size() ;
	specMoveCommand *command = new specMoveCommand ;
	command->setParentObject(target->model());
	command->setItems(lastRequested,parent,row) ;
	command->setText(tr("Move items"));
	push(command) ;
	return count ;
}

int specActionLibrary::deleteInternally(specModel* model)
{
	int count = lastRequested.size() ;
	QList<specModelItem*> pointers = model->pointerList(lastRequested) ;
	specUndoCommand *command = specDeleteAction::command(model, pointers) ;
	command->setText(tr("Move items"));
	push(command) ;
	return count ;
}

void specActionLibrary::addPlot(specPlot *plot)
{
	connect(undoStack,SIGNAL(indexChanged(int)),plot,SLOT(replot())) ;
	plot->setUndoPartner(this) ;
}

void specActionLibrary::purgeUndo()
{
	if (QMessageBox::Yes ==
			QMessageBox::question(0,
						  tr("Really Clear History?"),
						  tr("Do you really want to delete all undo and redo actions?  WARNING:  This cannot be undone."),
						  QMessageBox::Yes | QMessageBox::No,
						  QMessageBox::No))
		undoStack->clear();
}

QMenu *specActionLibrary::contextMenu(QWidget *w)
{
//	specMetaView* metaView = dynamic_cast<specMetaView*>(w) ;
	specView *view = dynamic_cast<specView*>(w) ;
//	specDataView* dataView = dynamic_cast<specDataView*>(w) ;
	QList<QAction*> actions ;
	actions << view->findChild<specAddConnectionsAction*>()
			<< view->findChild<specAddFitAction*>()
			<< view->findChild<specRemoveFitAction*>()
			<< view->findChild<specToggleFitStyleAction*>()
			<< view->findChild<specConductFitAction*>()

			<< view->findChild<specTiltMatrixAction*>()
			<< view->findChild<specSpectrumCalculatorAction*>()

			<< view->findChild<changePlotStyleAction*>()

			<< view->findChild<specAddFolderAction*>()
			<< view->findChild<specSetMultilineAction*>() // TODO:  set Checked behavior
			<< view->findChild<specItemPropertiesAction*>()
			<< view->findChild<specPasteAction*>()
			<< view->findChild<specCopyAction*>()
			<< view->findChild<specCutAction*>()
			<< view->findChild<specDeleteAction*>() ;
	actions.removeAll(0) ;
	if (actions.isEmpty()) return 0 ;

	QMenu *cMenu = new QMenu(w) ;
	cMenu->addActions(actions) ;
//	if (metaView && metaView->model())
//	{
//		specModelItem *currentItem = view->model()->itemPointer(metaView->currentIndex()) ;
//		if (specMetaItem* mi = dynamic_cast<specMetaItem*>(currentItem))
//		{
//			addNewAction(cMenu, new specAddConnectionsAction(w)) ;
//			if (mi->getFitCurve()) // TODO should be handled by the actions requirements
//			{
//				addNewAction(cMenu, new specRemoveFitAction(w)) ;
//				addNewAction(cMenu, new specConductFitAction(w)) ;
//				addNewAction(cMenu, new specToggleFitStyleAction(w)) ;
//			}
//			else
//				addNewAction(cMenu, new specAddFitAction(w)) ;
//		}
//	}

//	if (dataView && dataView->model())
//	{
//		addNewAction(cMenu, new specTiltMatrixAction(w)) ;
//		addNewAction(cMenu, new specSpectrumCalculatorAction(w)) ;
//	}

//	if ((dataView && dataView->model()) || (metaView && metaView->model()))
//		addNewAction(cMenu, new changePlotStyleAction(w)) ;

//	if (view && view->model())
//	{
//		addNewAction(cMenu, new specAddFolderAction(w)) ;
//		specModelItem *currentItem = view->model()->itemPointer(view->currentIndex()) ;
//		specSetMultilineAction *mlAction = new specSetMultilineAction(w) ;
//		addNewAction(cMenu, mlAction) ;
//		if (currentItem && view->currentIndex().isValid())
//			mlAction->setChecked(
//						currentItem->descriptorProperties(
//							view->model()->descriptors()[
//							view->currentIndex().column()]) & spec::multiline) ; // TODO move to action
//		if (dynamic_cast<specDataItem*>(currentItem)
//				|| dynamic_cast<specMetaItem*>(currentItem)
//				|| dynamic_cast<specSVGItem*>(currentItem))
//			addNewAction(cMenu, new specItemPropertiesAction(w)) ;
//		if (QApplication::clipboard()->mimeData())
//			addNewAction(cMenu, new specPasteAction(w)) ;
//		if (!view->getSelection().isEmpty())
//		{
//			addNewAction(cMenu, new specCopyAction(w)) ;
//			addNewAction(cMenu, new specCutAction(w)) ;
//			addNewAction(cMenu, new specDeleteAction(w)) ;
//		}
//	}
	return cMenu ;
}

specActionLibrary::~specActionLibrary()
{
	delete undoStack ;
}

void specActionLibrary::setProgressDialog(QProgressDialog *p)
{
	progress = p ;
}
