#include "specundoaction.h"
#include <QEvent>

specUndoAction::specUndoAction(QObject* parent) :
	QAction(parent)
{
	setShortcutContext(Qt::WidgetShortcut);
	if(QWidget* w = qobject_cast<QWidget*> (parent))
		w->addAction(this);
}

void specUndoAction::gotTrigger()
{
	if(!parent()) return ;
	execute() ;
}

void specUndoAction::setLibrary(specActionLibrary* lib)
{
	if(lib == 0)
		disconnect(this, SIGNAL(triggered()), this, SLOT(gotTrigger())) ;
	else
		connect(this, SIGNAL(triggered()), this, SLOT(gotTrigger())) ;
	library = lib ;
}

bool specUndoAction::event(QEvent *e)
{
	if (e->type() == QEvent::ActionChanged)
	{
		QString description = toolTip().section("\t",0,0) ;
		if (!shortcut().isEmpty())
			description += QString("\t\t<br><small>") + shortcut().toString() + "</small>";
		setToolTip(description) ;
	}
	return QAction::event(e) ;
}
