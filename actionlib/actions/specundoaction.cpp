#include "specundoaction.h"

specUndoAction::specUndoAction(QObject *parent) :
    QAction(parent)
{
	setShortcutContext(Qt::WidgetShortcut);
	if (QWidget* w = qobject_cast<QWidget*>(parent))
		w->addAction(this);
}

void specUndoAction::gotTrigger()
{
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
