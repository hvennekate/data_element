#include "tst_shortcutactivation.h"
#include "../specplotwidget.h"
#include <QAction>
#include "specview.h"

Q_DECLARE_METATYPE(QAction*)

tst_shortCutActivation::tst_shortCutActivation(QObject *parent) :
	QObject(parent),
	mainWidget(new specPlotWidget),
	triggered(false)
{
	setObjectName("shortCutActivation");
}

void tst_shortCutActivation::initTestCase()
{
	mainWidget = new specPlotWidget ;
	mainWidget->show();
//	QTest::qWaitForWindowShown(mainWidget) ; // TODO!
}

void tst_shortCutActivation::cleanupTestCase()
{
	delete mainWidget ;
}

void tst_shortCutActivation::testTrigger_data()
{
	if (!mainWidget) return ;
	QTest::addColumn<QAction*>("action") ;

	foreach(QAction* action, mainWidget->findChildren<QAction*>())
		QTest::newRow(action->iconText().toLatin1().data()) << action ;
}

void tst_shortCutActivation::testTrigger()
{
	QFETCH(QAction*, action) ;
	disconnect(action, 0,0,0) ;
	connect(action, SIGNAL(triggered()), this, SLOT(trigger())) ;
	action->setEnabled(true) ;
	action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_9)) ;
	triggered = false ;
	QTest::keyClick(action->parentWidget(), Qt::Key_9, Qt::ControlModifier) ;
//	action->trigger();
	action->setShortcuts(QList<QKeySequence>());
	QVERIFY2(triggered, action->iconText().toLatin1().data()) ;
}

void tst_shortCutActivation::trigger()
{
	triggered = true ;
}
