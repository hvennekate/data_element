#ifndef TST_SPECMETAPARSER_H
#define TST_SPECMETAPARSER_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../kinetic/specmetaparser.h"
#include "../spectral/specdataitem.h"
#include <QVector>

class tst_specMetaParser : public QObject
{
	Q_OBJECT
private:
	QVector<specModelItem *> testItems ;
	specMetaParser *parser ;
public:

private Q_SLOTS:
	void init() ;
	void cleanup() ;

	void warnings() ;
	void warnings_data() ;

	void ok() ;
	void ok_data() ;

	void evaluate() ;
	void evaluate_data() ;
};

#endif // TST_SPECMETAPARSER_H
