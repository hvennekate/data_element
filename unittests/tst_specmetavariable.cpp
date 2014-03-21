#include "tst_specmetavariable.h"

Q_DECLARE_METATYPE(QVector<double>)

void tst_specMetaVariable::init()
{
	// ought to be init test case
	QHash<QString, specDescriptor> description ;
	description["Zeit"] = specDescriptor(42) ;
	description["Name"] = specDescriptor("testitem") ;

	QVector<specDataPoint> data ;
	for (int i = 1500 ; i < 2000 ; i += 30)
		data << specDataPoint(i, i/10, 1000) ;

	testItem = new specDataItem(data, description) ;
}

void tst_specMetaVariable::cleanup()
{
	delete testItem ;
}

void tst_specMetaVariable::setRange()
{
	QFETCH(QString, variableString) ;
	QFETCH(int, min) ;
	QFETCH(int, max) ;
	QFETCH(int, inc) ;
	specMetaVariable *var = specMetaVariable::factory(variableString) ;
	specMetaVariable::indexLimit limit = var->indexRange(10) ;
	QCOMPARE(limit.begin, min) ;
	QCOMPARE(limit.end, max) ;
	QCOMPARE(limit.increment, inc) ;
	delete var ;
}

void tst_specMetaVariable::setRange_data()
{
	QTest::addColumn<QString>("variableString");
	QTest::addColumn<int>("min");
	QTest::addColumn<int>("max");
	QTest::addColumn<int>("inc");
	QTest::newRow("every_other") << QString("[::2]i1600:1700") << 0 << 10 << 2;
	QTest::newRow("odd_other") << QString("[1:5:2]i1600:1700") << 1 << 5 << 2;
	QTest::newRow("all") << QString("i1600:1700") << 0 << 10 << 1;
	QTest::newRow("single") << QString("[3]i1600:1700") << 3 << 4 << 1;
}


void tst_specMetaVariable::xValues()
{
	QFETCH(QString, variableString) ;
	specMetaVariable* var(specMetaVariable::factory(variableString)) ;
	QFETCH(QVector<double>, input) ;
	QFETCH(QVector<double>, output) ;
	QFETCH(bool, returns) ;

	QCOMPARE(returns, var->xValues(testItem,input)) ;
//	qDebug() << input ;
	QCOMPARE(output, input) ;
	delete var ;
}

void tst_specMetaVariable::xValues_data()
{
	QTest::addColumn<QString>("variableString") ;
	QTest::addColumn<QVector<double> > ("input") ;
	QTest::addColumn<QVector<double> > ("output") ;
	QTest::addColumn<bool>("returns") ;

	QTest::newRow("initiate") << "[::2]x1600:1700"
				  << QVector<double>(1,NAN)
				  << (QVector<double>() << 1620 << 1650 << 1680)
				  << false ;
	QTest::newRow("single") << "[::2]x1600:1700"
				<< (QVector<double>() << 1500 << 1650 << 1700)
				<< (QVector<double>() << 1650)
				<< true ;
	QTest::newRow("all") << "[::2]i1600:1700"
			     << (QVector<double>() << 1500 << 1650 << 1700)
			     << (QVector<double>() << 1500 << 1650 << 1700)
			     << true ;
}

void tst_specMetaVariable::values()
{
	QFETCH(QString, variableString) ;
	QFETCH(QVector<double>, xValues) ;
	QFETCH(QVector<double>, output) ;

	testItem->refreshPlotData();

//	qDebug() << "XVALUES (ITEM)" << testItem->dataSize() ;

	specMetaVariable *var(specMetaVariable::factory(variableString)) ;
//	qDebug() << "Soll:" << output << "Ist:" << var->values(testItem,xValues) << "Gleich:" ;
//	for (int i = 0 ; i < output.size() ; ++i)
//		qDebug() << QString().setNum(output[i],'g',18)
//			 << QString().setNum(var->values(testItem,xValues)[i],'g',18)
//			 << (output[i] == var->values(testItem,xValues)[i]) ;
	QCOMPARE(output,var->values(testItem,xValues)) ;
	delete var ;
}

void tst_specMetaVariable::values_data()
{
	QTest::addColumn<QString>("variableString") ;
	QTest::addColumn<QVector<double> >("xValues") ;
	QTest::addColumn<QVector<double> >("output") ;

	double integral = 15.*(162+2.*165+168) ;
	QTest::newRow("integral") << "[::2]i1600:1700"
				  << (QVector<double>() << 1620 << 1650 << 1680)
				  << (QVector<double>() << integral << integral << integral) ;
	QTest::newRow("xValues") << "[::2]x1600:1700"
				 << (QVector<double>() << 1650 << 1800)
				 << (QVector<double>() << 1650 << 1800) ;
	QTest::newRow("yValues") << "[::2]y1600:1700"
				 << (QVector<double>() << 1650 << 1680)
				 << (QVector<double>() << 165 << 168) ;
	QTest::newRow("MaxSimple") << "[::2]u1600:1700"
				   << (QVector<double>() << 1650 << 1680)
				   << (QVector<double>(2,168)) ;
	QTest::newRow("MinSimple") << "[::2]l1600:1700"
				   << (QVector<double>() << 1650 << 1680)
				   << (QVector<double>(2,162)) ; // Disregards the given xValues
}

tst_specMetaVariable::tst_specMetaVariable()
{
	setObjectName("specMetaVariable") ;
}
