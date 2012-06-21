#include "tst_specmetaparser.h"

Q_DECLARE_METATYPE(QVector<double>)

void tst_specMetaParser::init()
{
	// ought to be init test case
	QHash<QString, specDescriptor> description ;
	description["Zeit"] = specDescriptor(42) ;
	description["Name"] = specDescriptor("testitem") ;

	QList<specDataPoint> data ;
	for (int i = 1500 ; i < 2000 ; i += 30)
		data << specDataPoint(42, i, sqrt(i), 1000) ;

	testItems << new specDataItem(data, description) << new specDataItem(data, description);

	// preparing the parser

}

void tst_specMetaParser::cleanup()
{
	foreach(specModelItem* item, testItems)
		delete item;
	testItems.clear();
}

void tst_specMetaParser::warnings() {}
void tst_specMetaParser::warnings_data() {}

void tst_specMetaParser::ok()
{
	QFETCH(QString,variableDefinitions) ;
	QFETCH(QString,xExpression) ;
	QFETCH(QString,yExpression) ;
	QFETCH(bool, result) ;
	specMetaParser parser(variableDefinitions,xExpression,yExpression) ;
	QCOMPARE(parser.ok(), result) ;
}
void tst_specMetaParser::ok_data()
{
	QTest::addColumn<QString>("variableDefinitions") ;
	QTest::addColumn<QString>("xExpression") ;
	QTest::addColumn<QString>("yExpression") ;
	QTest::addColumn<bool>("result") ;

	QTest::newRow("simple") << "x = x1600:1700\ny = i1600:1700" << "x" << "y" << true ;
	QTest::newRow("awrong") << "x = x1600\ny = i1600:1700" << "x" << "y" << false ;
	QTest::newRow("bwrong") << "x = a\ny = i1600:1700" << "x" << "y" << false ;
}

void tst_specMetaParser::evaluate() {}
void tst_specMetaParser::evaluate_data() {}


