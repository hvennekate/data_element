#include "specundoaction.h"

specUndoAction::specUndoAction(QObject *parent) :
    QAction(parent)
{
}

void specUndoAction::gotTrigger()
{
	qDebug() << "got trigger:" << parent() ;
	if (!parent()) return ;
	execute() ;
}

void specUndoAction::setLibrary(specActionLibrary* lib)
{
	if (lib == 0)
		disconnect(this,SIGNAL(triggered()),this,SLOT(gotTrigger())) ;
	else
		connect(this,SIGNAL(triggered()),this,SLOT(gotTrigger())) ;
	library = lib ;
}
