#include <QTest>
#include "tst_specmetavariable.h"
#include "tst_specmetaparser.h"
#include "tst_moveplotcommand.h"
#include "tst_shortcutactivation.h"

typedef QPair<QString, int> stringIntPair ;

template <class testclass>
stringIntPair runTest()
{
	testclass test ;
	qDebug() << "RUNNING" << test.objectName() << "TEST" ;
	return qMakePair(test.objectName(), QTest::qExec(&test, QStringList())) ;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QVector<stringIntPair> results ;
	results << runTest<tst_specMetaParser>()
		<< runTest<tst_specMetaVariable>()
		<< runTest<tst_movePlotCommand>()
		<< runTest<tst_shortCutActivation>() ;
	qDebug() << "=========== All tests done. ===========" ;
	foreach(const stringIntPair& result, results)
		qDebug() << (result.second ? "!!" : "  ")
			 << result.second
			 << result.first ;

	return 0 ;
}

//#include "tst_main.moc"
