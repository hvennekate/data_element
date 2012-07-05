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
#include "log/speclogview.h"
#include "specimportspecaction.h"

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
	qDebug() << "*** pushing onto stack" ;
	undoStack->push(cmd) ;
	qDebug("*** pushed onto stack") ;
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
}

QAction* specActionLibrary::undoAction(QObject* target)
{
	QAction *undoAction = undoStack->createUndoAction(target) ;
	undoAction->setIcon(QIcon::fromTheme("edit-undo"));
}

QToolBar* specActionLibrary::toolBar(QWidget *target)
{
	addParent(target) ;
	specView* view ;
	if ((view = dynamic_cast<specView*>(target)) && view->model())
		addParent(view->model()) ;

	if (typeid(*target)== typeid(specDataView))
	{
		QToolBar *bar = new QToolBar(target) ;

		addNewAction(bar, new specAddFolderAction(target)) ;
		addNewAction(bar, new specAddSVGItemAction(target)) ;
		addNewAction(bar, new specImportSpecAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specCopyAction(target)) ;
		addNewAction(bar, new specCutAction(target)) ;
		addNewAction(bar, new specPasteAction(target)) ;
		addNewAction(bar, new specDeleteAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specTreeAction(target)) ;
		addNewAction(bar, new specMergeAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specRemoveDataAction(target)) ;
		addNewAction(bar, new specAverageDataAction(target)) ;
		addNewAction(bar, new changePlotStyleAction(target)) ;

		return bar ;
	}
	else if (dynamic_cast<specMetaView*>(target))
	{
		QToolBar *bar = new QToolBar(target) ;

		addNewAction(bar, new specAddFolderAction(target)) ;
		addNewAction(bar, new specNewMetaItemAction(target));
		addNewAction(bar, new specAddSVGItemAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specCopyAction(target)) ;
		addNewAction(bar, new specCutAction(target)) ;
		addNewAction(bar, new specPasteAction(target)) ;
		addNewAction(bar, new specDeleteAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specAddConnectionsAction(target)) ;

		return bar ;
	}
	else if (dynamic_cast<specLogView*>(target))
	{
		QToolBar *bar = new QToolBar(target) ;

		addNewAction(bar, new specAddFolderAction(target)) ;
		addNewAction(bar, new specImportSpecAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specCopyAction(target)) ;
		addNewAction(bar, new specCutAction(target)) ;
		addNewAction(bar, new specPasteAction(target)) ;
		addNewAction(bar, new specDeleteAction(target)) ;

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

void specActionLibrary::addParent(QObject *pointer)
{
	if (!parents.contains(pointer))
		parents << pointer ;
}

QObject* specActionLibrary::parentId(int num)
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
		out << qint32(command->id()) << quint32(parents.indexOf(command->parentObject())) ;
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
		command->setParentObject(parents[parentId]) ;
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
	qDebug() << "setting last req" << list ;
	lastRequested = list ;
}

void specActionLibrary::moveInternally(const QModelIndex &parent, int row, specView* target)
{
	specMoveCommand *command = new specMoveCommand(lastRequested,parent,row) ;
	command->setParentObject(target);
	push(command) ;
}

void specActionLibrary::addPlot(specPlot *plot)
{
	connect(this,SIGNAL(stackChanged()),plot,SLOT(replot())) ;
}
