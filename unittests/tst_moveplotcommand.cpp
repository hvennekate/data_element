#include "tst_moveplotcommand.h"
#include "../model/specdescriptor.h"
#include "../spectral/specdatapoint.h"
#include "../actionlib/commands/specplotmovecommand.h"
#include <cfloat>
#include <cmath>
#include <QTime>
#include <QDate>
#include "../model/specmodel.h"
#include "../spectral/specdataitem.h"
#include <string>
#include "../utility-functions.h"

tst_movePlotCommand::tst_movePlotCommand()
	: model(0),
	  itemA(0),
	  itemB(0)
{
	setObjectName("movePlotCommand") ;
	// random seed
	QTime t(QTime::currentTime()) ;
	QDate d(QDate::currentDate()) ;
	rseed = (((((unsigned int) d.month() * 30 + d.day())*24
			+ t.hour()) * 60
			+ t.minute()) * 60
			+ t.second()) * 1000
			+ t.msec() ;
	qDebug() << "Random seed:" << rseed ;
}

void tst_movePlotCommand::init()
{
	QHash<QString, specDescriptor> description ;
	QVector<specDataPoint> dataA, dataB ;
	for (int i = -10 ; i < 10 ; ++i)
	{
		dataA << specDataPoint(i, i/10., 1000) ;
		dataB << specDataPoint(i, exp(-i*i), 500) ;
	}
	model = new specModel(this) ;
	itemA = new specDataItem(dataA, description) ;
	itemB = new specDataItem(dataB, description) ;
	model->insertItems(QList<specModelItem*>() << itemA << itemB,
			   QModelIndex()) ;
	itemA->refreshPlotData();
	itemB->refreshPlotData();
	QDataStream out(&savedModel, QIODevice::WriteOnly) ;
	out << (specStreamable&) *model ;
}

void tst_movePlotCommand::cleanup()
{
	delete model ;
}

bool xIsEqual(const QPointF&a, const QPointF& b)
{
	return a.x() == b.x() ;
}

void tst_movePlotCommand::check()
{
	QFETCH(double, Offset) ;
	QFETCH(double, Slope) ;
	QFETCH(double, Scale) ;
	QFETCH(double, Shift) ;

	specPlotMoveCommand *command = new specPlotMoveCommand(0) ;
	QVector<QPointF> data, newData ;
	for (size_t i = 0 ; i < itemA->dataSize() ; ++i)
		data << itemA->sample(i) ;
	for (int i = 0 ; i < data.size() ; ++i)
	{
		data[i].setX(data[i].x() + (std::isnormal(Shift) ? Shift : 0) ) ;
		data[i].setY(data[i].y() * (std::isnormal(Scale) ? Scale : 1)
			     + (std::isnormal(Offset) ? Offset : 0)
			     + data[i].x() * (std::isnormal(Slope) ? Slope :0)
			     ) ;
	}
	// Average data:
	QVector<QPointF> ave ;
	averageToNew(data.begin(), data.end(), xIsEqual, std::back_inserter(ave));
	data.swap(ave) ;

	command->setParentObject(model) ;
	command->setItem(itemA) ;
	command->setCorrections(Shift, Offset, Slope, Scale) ;
	command->redo();
	itemA->revalidate();
	for (size_t i = 0 ; i < itemA->dataSize() ; ++i)
		newData << itemA->sample(i) ;
	command->undo();

	QByteArray newModel ;
	QDataStream out(&newModel, QIODevice::WriteOnly) ;
	out << (specStreamable&) *model ;

	bool dataEqual = (data.size() == newData.size()) ;
	QVector<QPointF> diffData ;
	for (int i = 0 ; i < qMin(data.size(), newData.size()) ; ++i)
	{
		if (!dataEqual) break ;
		const QPointF &sample = data[i],
				&newSample = newData[i] ;
		dataEqual = doubleComparison(sample.x(), newSample.x())
				&& doubleComparison(sample.y(), newSample.y()) ;
		diffData << newSample - sample ;
	}

	if (!dataEqual)
	{
		qDebug() << Offset << Slope << Scale << Shift ;
		qDebug() << data ;
		qDebug() << newData ;
		qDebug() << diffData ;
		QCOMPARE(data, newData) ;
	}
//	QCOMPARE(savedModel, newModel) ;
}

void doubleToHex(const double& d, char*a)
{
	const int16_t* p = (const int16_t*) &d ;
	for (size_t i = 0 ; i < sizeof(double)/sizeof(int16_t) ; ++i)
	{
		a[2*i]   = '0' + p[i] / 256 ;
		a[2*i+1] = '0' + p[i] % 256 ;
	}
}

void tst_movePlotCommand::check_data()
{
	QTest::addColumn<double>("Offset") ;
	QTest::addColumn<double>("Slope") ;
	QTest::addColumn<double>("Scale") ;
	QTest::addColumn<double>("Shift") ;

	QVector<double> possibleValues ;
	possibleValues << NAN
		       << INFINITY
		       << - INFINITY
		       << 0.
		       << 1
		       << -1
		       << 5.
		       << -5.5
		       << 4.7477e89
		       << 4.7477e-89 ;

	// now, for the fun of it, some random values:
	srand(rseed);
//	const int len = sizeof(double)/sizeof(int16_t) ;
//	for (int j = 0 ; j < 1 ; ++j)
//	{
//		double r = 0 ;
//		int16_t *p = (int16_t*) &r ;
//		for (int i = 0 ; i < len ; ++i)
//			p[i] = rand() ;
//		possibleValues << r ;
//	}
	qDebug() << possibleValues ;
//	char n[len*2*4+1] ;
//	n[len*2*4] = '\0' ;
	foreach (double of, possibleValues)
	{
//		doubleToHex(of, n) ;
		foreach(double sl, possibleValues)
		{
//			doubleToHex(sl, n+len*2);
			foreach(double sc, possibleValues)
			{
//				doubleToHex(sc, n+len*2*2);
				foreach(double sh, possibleValues)
				{
//					doubleToHex(sh, n+len*2*3) ;
					QTest::newRow("") << of << sl << sc << sh ;
				}
			}
		}
	}
}
