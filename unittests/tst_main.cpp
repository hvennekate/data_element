#include <QTest>
#include "tst_specmetavariable.h"
#include "tst_specmetaparser.h"

//class tst_specMetaVariable ;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	qDebug() << "RUNNING specMetaVariable tests" ;
	tst_specMetaVariable specMetaVariableTest ;
	QTest::qExec(&specMetaVariableTest, argc, argv) ;

	qDebug() << "RUNNING specMetaParser tests" ;
	tst_specMetaParser specMetaParserTest ;
	QTest::qExec(&specMetaParserTest, argc, argv) ;
	qDebug() << "All tests done." ;
	return 0 ;
}

//#include "tst_main.moc"
