#include "specactionlibrary.h"
#include "specdeleteaction.h"
#include "specaddfolderaction.h"
#include <QDebug>
#include "specaddfoldercommand.h"
#include "specdeletecommand.h"

specActionLibrary::specActionLibrary(QObject *parent) :
    QObject(parent)
{
	undoStack = new QUndoStack ;
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
		bar->addAction(undoStack->createUndoAction(target)) ;
		qDebug("adding redo action") ;
		bar->addAction(undoStack->createRedoAction(target)) ;
		qDebug("returning toolbar") ;
		return bar ;
	}
	return new QToolBar(target) ;
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
		qDebug() << "Id: " << id << "Parent: " << parentId ;
		specUndoCommand* command ;
		switch(id)
		{
		case spec::deleteId :
			command = new specDeleteCommand ;
			break ;
		case spec::newFolderId :
			command = new specAddFolderCommand ;
			break ;
		}
		if (!command) continue ;
			command->setParentWidget(parents[parentId]) ;
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
}
