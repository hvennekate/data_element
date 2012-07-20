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
#include "names.h"

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
	qDebug() << "!!!!!!!!! Pushing command onto undo stack" ;
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
	return redoAction ;
}

QAction* specActionLibrary::undoAction(QObject* target)
{
	QAction *undoAction = undoStack->createUndoAction(target) ;
	undoAction->setIcon(QIcon::fromTheme("edit-undo"));
	return undoAction ;
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

void specActionLibrary::writeToStream(QDataStream &out) const
{
	out << qint32(undoStack->count()) ;
	out << qint32(undoStack->index()) ;
	for (int i = 0 ; i < undoStack->count() ; ++i)
	{
		specUndoCommand* command = (specUndoCommand*) undoStack->command(i) ;
		out << type(command->id()) << qint32(parents.indexOf(command->parentObject()))
		    << *command ;
	}
}

specUndoCommand *specActionLibrary::commandById(int id, specUndoCommand* parent)
{
	switch(id)
	{
	case specStreamable::deleteCommandId :
		return new specDeleteCommand(parent) ;
	case specStreamable::newFolderCommandId :
		return new specAddFolderCommand(parent) ;
	case specStreamable::moveItemsCommandId :
		return new specMoveCommand(parent) ;
	case specStreamable::movePlotCommandId :
		return new specPlotMoveCommand(parent) ;
	case specStreamable::multiCommandId :
		return new specMultiCommand(parent) ;
	default:
		return 0 ;
	}
}

void specActionLibrary::readFromStream(QDataStream &in)
{
	qint32 num, position ;
	in >> num >> position ; // TODO really rely on num?

	type t ;
	QVector<qint32> parentIndex(num,0) ;

	for (int i = 0 ; i < num ; ++i)
	{
		in >> t >> parentIndex[i] ;
		specUndoCommand *command = commandById(t) ;
		in >> *command ;
		undoStack->push(command) ;
	}

	undoStack->setIndex(position) ;
	for (int i = 0 ; i < num ; ++i)
		((specUndoCommand*) undoStack->command(i))->setParentObject(parents[parentIndex[i]]) ;
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
