#include "specdockwidget.h"
#include <QEvent>
#include <QFileInfo>

#define APPLYTOSUBDOCKS foreach(specDockWidget* sub, subDocks)

specDockWidget::specDockWidget(QString type, QWidget *parent) :
	QDockWidget(parent),
	widgetTypeName(type),
	changingTitle(false)
{
}

void specDockWidget::changeEvent(QEvent *event)
{
	if (changingTitle) return ;
	if (event->type() == QEvent::WindowTitleChange)
	{
		changingTitle = true ;
		setWindowTitle(QFileInfo(windowFilePath()).fileName()
			       + QLatin1String("[*]")
			       + QLatin1Char(' ') + QChar(0x2014) + QLatin1Char(' ')
			       + widgetTypeName) ;
		changingTitle = false ;
		event->accept();
	}
	QDockWidget::changeEvent(event) ;
}
