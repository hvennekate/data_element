#include "specactionlibrary.h"
#include "specdeleteaction.h"
#include "specaddfolderaction.h"
#include <QDebug>

specActionLibrary::specActionLibrary(QObject *parent) :
    QObject(parent)
{
	undoStack = new QUndoStack ;
	qDebug("***** action library initialized") ;
	qDebug() << undoStack ;
}

QDataStream& specActionLibrary::write(QDataStream& out)
{
	return out ;
}

QDataStream& specActionLibrary::read(QDataStream& in)
{
	return in ;
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
