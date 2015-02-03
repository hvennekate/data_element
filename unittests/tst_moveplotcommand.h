#ifndef TST_MOVEPLOTCOMMAND_H
#define TST_MOVEPLOTCOMMAND_H

#include <QObject>
#include <QtTest>

class specModel ;
class specModelItem ;

class tst_movePlotCommand : public QObject
{
	Q_OBJECT
private:
	specModel* model ;
	specModelItem *itemA, *itemB ;
	QByteArray savedModel ;
	unsigned int rseed ;
public:
	explicit tst_movePlotCommand(QObject* parent = 0);
private Q_SLOTS:
	void init() ;
	void cleanup() ;

	void check() ;
	void check_data() ;

};

#endif // TST_MOVEPLOTCOMMAND_H
