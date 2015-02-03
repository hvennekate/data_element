#ifndef TST_SPECMETAPARSER_H
#define TST_SPECMETAPARSER_H

#include <QString>
#include <QtTest>
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
	tst_specMetaParser() ;

private Q_SLOTS:
	void init() ;
	void cleanup() ;

	void ok() ;
	void ok_data() ;

	void warnings() ;
	void warnings_data() ;

	void evaluate() ;
	void evaluate_data() ;
};

#endif // TST_SPECMETAPARSER_H
