#include "tst_specmetaparser.h"

Q_DECLARE_METATYPE(QVector<double>)
Q_DECLARE_METATYPE(QVector<QPointF>)

void tst_specMetaParser::init()
{
	// ought to be init test case
	QHash<QString, specDescriptor> description ;
	description["Zeit"] = specDescriptor(42) ;
	description["Name"] = specDescriptor("testitem") ;

	QVector<specDataPoint> data ;
	for (int i = 1500 ; i < 2000 ; i += 30)
		data << specDataPoint(i, i/10, 1000) ;

	testItems << new specDataItem(data, description) << new specDataItem(data, description);

	// preparing the parser

}

void tst_specMetaParser::cleanup()
{
	foreach(specModelItem* item, testItems)
		delete item;
	testItems.clear();
}

void tst_specMetaParser::evaluate()
{
	QFETCH(QString,variableDefinitions) ;
	QFETCH(QString,xExpression) ;
	QFETCH(QString,yExpression) ;
	QFETCH(QVector<QPointF>, result) ;
	specMetaParser parser(variableDefinitions,xExpression,yExpression,0) ;
//	qDebug() << parser.warnings() ;
	QwtSeriesData<QPointF> *data = parser.evaluate(testItems) ;
	QwtPointSeriesData reference(result) ;
	QCOMPARE(data->size(),reference.size()) ;
	for (size_t i = 0 ; i < data->size() && i < reference.size() ; ++i)
		QCOMPARE(data->sample(i),reference.sample(i)) ;
	delete data ;
}
void tst_specMetaParser::evaluate_data()
{
	QTest::addColumn<QString>("variableDefinitions") ;
	QTest::addColumn<QString>("xExpression") ;
	QTest::addColumn<QString>("yExpression") ;
	QTest::addColumn<QVector<QPointF> >("result") ;

	double integral = 15.*(162+2.*165+168) ;
	QTest::newRow("simple") << "x = x1600:1700\ny = i1600:1700" << "x" << "y" <<
				   (QVector<QPointF>() << QPointF(1620,integral)
				    << QPointF(1650,integral)
				    << QPointF(1680,integral)
				    << QPointF(1620,integral)
				    << QPointF(1650,integral)
				    << QPointF(1680,integral));
	QTest::newRow("Descriptors") << "x = \"Zeit\"\ny = \"Zeit\"" << "x" << "y" <<
					QVector<QPointF>(2,QPointF(42,42));
}

void tst_specMetaParser::ok()
{
	QFETCH(QString,variableDefinitions) ;
	QFETCH(QString,xExpression) ;
	QFETCH(QString,yExpression) ;
	QFETCH(bool, result) ;
	specMetaParser parser(variableDefinitions,xExpression,yExpression,0) ;
//	qDebug() << parser.warnings() ;
	QCOMPARE(parser.ok(), result) ;
}
void tst_specMetaParser::ok_data()
{
	QTest::addColumn<QString>("variableDefinitions") ;
	QTest::addColumn<QString>("xExpression") ;
	QTest::addColumn<QString>("yExpression") ;
	QTest::addColumn<bool>("result") ;

	QTest::newRow("simple") << "x = x1600:1700\ny = i1600:1700" << "x" << "y" << true ;
	QTest::newRow("awrong") << "x = x1600\ny = i1600:1700" << "x" << "y" << true ;
	QTest::newRow("bwrong") << "x = a\ny = i1600:1700" << "x" << "y" << false ;
}

void tst_specMetaParser::warnings()
{
	QFETCH(QString,variableDefinitions) ;
	QFETCH(QString,xExpression) ;
	QFETCH(QString,yExpression) ;
	QFETCH(QString, result) ;
	specMetaParser parser(variableDefinitions,xExpression,yExpression,0) ;
//	qDebug() << parser.warnings() ;
	QCOMPARE(parser.warnings(), result) ;
}
void tst_specMetaParser::warnings_data()
{
	QTest::addColumn<QString>("variableDefinitions") ;
	QTest::addColumn<QString>("xExpression") ;
	QTest::addColumn<QString>("yExpression") ;
	QTest::addColumn<QString>("result") ;

	QTest::newRow("simple") << "x = x1600:1700\ny = i1600:1700" << "x" << "y" << "" ;
	QTest::newRow("awrong") << "x = x1600\ny = i1600:1700" << "x" << "y" << "" ;
	QTest::newRow("bwrong") << "x = a\ny = i1600:1700" << "x" << "y" << "Not an acceptable value:  a" ;
}


tst_specMetaParser::tst_specMetaParser()
{
	setObjectName("specMetaParser") ;
}
