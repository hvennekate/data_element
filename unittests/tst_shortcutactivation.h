#ifndef TST_SHORTCUTACTIVATION_H
#define TST_SHORTCUTACTIVATION_H

#include <QtTest>
#include <QObject>

class specPlotWidget ;

class tst_shortCutActivation : public QObject
{
	Q_OBJECT
private:
	specPlotWidget *mainWidget ;
	bool triggered ;
public:
	explicit tst_shortCutActivation(QObject *parent = 0);	
private Q_SLOTS:
	void initTestCase() ;
	void cleanupTestCase() ;
	void testTrigger() ;
	void testTrigger_data() ;
private slots:
	void trigger() ;
};

#endif // TST_SHORTCUTACTIVATION_H
