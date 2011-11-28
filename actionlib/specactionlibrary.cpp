#include "specactionlibrary.h"
#include "specdeleteaction.h"
#include "specaddfolderaction.h"
#include <QDebug>
#include "specaddfoldercommand.h"
#include "specdeletecommand.h"
#include "specmovecommand.h"
#include "speccopyaction.h"
#include "specpasteaction.h"
#include "speccutaction.h"
#include "specplotmovecommand.h"
#include "changeplotstyleaction.h"
#include "spectreeaction.h"
#include "specmulticommand.h"
#include "specmergeaction.h"
#include "specremovedataaction.h"
#include "specaveragedataaction.h"
#include "specaddsvgitem.h"
#include "specnewmetaitemaction.h"
#include "kinetic/specmetaview.h"
#include "specaddconnectionsaction.h"

specActionLibrary::specActionLibrary(QObject *parent) :
    QObject(parent)
{
	undoStack = new QUndoStack ;
	connect(undoStack,SIGNAL(indexChanged(int)),this,SIGNAL(stackChanged())) ;
	qDebug("***** action library initialized") ;
	qDebug() << undoStack ;
}

void specActionLibrary::push(specUndoCommand * cmd)
{
	qDebug("*** pushing onto stack") ;
	qDebug() << "undostack responses:  " ;
	qDebug() << undoStack ;
	qDebug() << undoStack->index() ;
	qDebug() << undoStack->canUndo() ;
	qDebug() << undoStack->canRedo() ;
	undoStack->push(cmd) ;
	qDebug("*** pushed onto stack") ;
}

QToolBar* specActionLibrary::toolBar(QWidget *target)
{
	addParent(target) ;
	if (typeid(*target)== typeid(specDataView))
	{
		QToolBar *bar = new QToolBar(target) ;
		qDebug("adding delete action") ;
		specDeleteAction *action = new specDeleteAction(target) ;
		action->setLibrary(this);
		bar->addAction(action) ;
		qDebug("added delete action") ;
		qDebug("Adding new folder action") ;
		specAddFolderAction *addAction = new specAddFolderAction(target) ;
		addAction->setLibrary(this) ;
		bar->addAction(addAction) ;
		qDebug("added addfolder action") ;

		specCopyAction *copyAction = new specCopyAction(target) ;
		copyAction->setLibrary(this) ;
		bar->addAction(copyAction) ;

		specPasteAction *pasteAction = new specPasteAction(target) ;
		pasteAction->setLibrary(this) ;
		bar->addAction(pasteAction) ;

		specCutAction *cutAction = new specCutAction(target) ;
		cutAction->setLibrary(this) ;
		bar->addAction(cutAction) ;

		specTreeAction *treeAction = new specTreeAction(target) ;
		treeAction->setLibrary(this) ;
		bar->addAction(treeAction) ;

		specMergeAction *mergeAction = new specMergeAction(target) ;
		mergeAction->setLibrary(this) ;
		bar->addAction(mergeAction) ;

		specRemoveDataAction *removeDataAction = new specRemoveDataAction(target) ;
		removeDataAction->setLibrary(this) ;
		bar->addAction(removeDataAction) ;

		specAverageDataAction *averageAction = new specAverageDataAction(target) ;
		averageAction->setLibrary(this) ;
		bar->addAction(averageAction) ;

		specAddSVGItemAction *SVGAction = new specAddSVGItemAction(target) ;
		SVGAction->setLibrary(this) ;
		bar->addAction(SVGAction) ;

		QAction *undoAction = undoStack->createUndoAction(target) ;
		undoAction->setIcon(QIcon::fromTheme("edit-undo"));
		bar->addAction(undoAction) ;

		qDebug("adding redo action") ;
		QAction *redoAction = undoStack->createRedoAction(target) ;
		redoAction->setIcon(QIcon::fromTheme("edit-redo")) ;
		bar->addAction(redoAction) ;

		changePlotStyleAction *styleAction = new changePlotStyleAction(target) ;
		styleAction->setLibrary(this) ;
		bar->addAction(styleAction) ;

		qDebug("returning toolbar") ;
		return bar ;
	}
	else if (dynamic_cast<specPlot*>(target))
	{
		qDebug("0000000 adding plot toolbar");
		QToolBar *bar = new QToolBar(target) ;
		return bar ;
	}
	else if (dynamic_cast<specMetaView*>(target))
	{
		qDebug("0000000 adding meta view toolbar") ;
		QToolBar *bar = new QToolBar(target) ;

		specDeleteAction *deleteAction = new specDeleteAction(target) ;
		deleteAction->setLibrary(this) ;
		bar->addAction(deleteAction) ;

		specAddFolderAction *addFolderAction = new specAddFolderAction(target) ;
		addFolderAction->setLibrary(this) ;
		bar->addAction(addFolderAction) ;

		specNewMetaItemAction *newMetaAction = new specNewMetaItemAction(target) ;
		newMetaAction->setLibrary(this) ;
		bar->addAction(newMetaAction) ;

		specAddConnectionsAction *addConnectionsAction = new specAddConnectionsAction(target) ;
		addConnectionsAction->setLibrary(this) ;
		bar->addAction(addConnectionsAction) ;

		return bar ;
	}

	return new QToolBar(target) ;
}

void specActionLibrary::addDragDropPartner(specModel* model)
{
	if (!partners.contains(model))
		partners << model ;
	model->setDropBuddy(this) ;
}

void specActionLibrary::addParent(QWidget *pointer)
{
	if (!parents.contains(pointer))
		parents << pointer ;
}

QWidget* specActionLibrary::parentId(int num)
{
	return parents[num] ;
}

QDataStream& specActionLibrary::write(QDataStream &out)
{
	out << qint32(undoStack->count()) ;
	out << qint32(undoStack->index()) ;

	qDebug() << "count:" << qint32(undoStack->count()) << "index:" << qint32(undoStack->index()) ;

	for (int i = 0 ; i < undoStack->count() ; ++i)
	{
		specUndoCommand* command = (specUndoCommand*) undoStack->command(i) ;
		out << qint32(command->id()) << quint32(parents.indexOf(command->parentWidget())) ;
	}
	for (int i = 0 ; i < undoStack->count() ; ++i)
		((specUndoCommand*) undoStack->command(i))->write(out) ;
	return out ;
}

specUndoCommand *specActionLibrary::commandById(int id, specUndoCommand* parent)
{
	switch(id)
	{
	case spec::deleteId :
		return new specDeleteCommand(parent) ;
	case spec::newFolderId :
		return new specAddFolderCommand(parent) ;
	case spec::moveItemsId :
		return new specMoveCommand(parent) ;
	case spec::movePlotId :
		return new specPlotMoveCommand(parent) ;
	case spec::multiCommandId :
		return new specMultiCommand(parent) ;
	default:
		return 0 ;
	}
}

QDataStream& specActionLibrary::read(QDataStream &in)
{
	qint32 num, position ;
	in >> num >> position ;
	qDebug() << "count: " << num << "index: " << position ;
	for (int i = 0 ; i < num ; ++i)
	{
		qint32 id, parentId ;
		in >> id >> parentId ;
		qDebug() << "Id: " << id << "Parent: " << parentId << parents[parentId] ;
		specUndoCommand *command = commandById(id) ;
		if (!command) continue ;
		command->setParentWidget(parents[parentId]) ;
		qDebug() << "pushing read in command onto stack" << command ;
		undoStack->push(command) ;
		qDebug() << "pushed command.  Stack size: " << undoStack->count() ;
	}
	undoStack->setIndex(position) ;
	qDebug() << "Set position.  Stack size: " << undoStack->count() << "  Pos: " << undoStack->index() ;
	for (int i = 0 ; i < num ; ++i)
	{
		qDebug() << "reading command"<<i ;
		((specUndoCommand*) undoStack->command(i))->read(in) ;
	}
	qDebug() << "reading undo commands done." ;
	return in ;
}

void specActionLibrary::setLastRequested(const QModelIndexList &list)
{
	lastRequested = list ;
}

void specActionLibrary::moveInternally(const QModelIndex &parent, int row, specView* target)
{
	specMoveCommand *command = new specMoveCommand(lastRequested,parent,row) ;
	command->setParentWidget(target);
	push(command) ;
}
