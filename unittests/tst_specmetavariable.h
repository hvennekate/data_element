#ifndef TST_SPECMETAVARIABLE_H
#define TST_SPECMETAVARIABLE_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../kinetic/specmetavariable.h"
#include "../spectral/specdataitem.h"

class tst_specMetaVariable : public QObject
{
	Q_OBJECT
private:
	specDataItem *testItem ;
public:
	tst_specMetaVariable() ;

private Q_SLOTS:
	void init() ;
	void cleanup() ;

	void setRange();
	void setRange_data();

	void xValues() ;
	void xValues_data() ;

	void values() ;
	void values_data() ;
};

#endif // TST_SPECMETAVARIABLE_H
