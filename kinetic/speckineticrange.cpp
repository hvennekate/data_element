#include "speckineticrange.h"
#include <qwt_plot.h>
#include <QTextStream>
#include "speckrcontextmenu.h"

specKineticRange::specKineticRange(double min,double max,specKinetic *par)
		: specRange(min,max), parent(par)
{
}


specKineticRange::~specKineticRange()
{
	if(parent) parent->removeRange(this) ;
}


void specKineticRange::refreshPlotData()
{// TODO maybe create different function which only contains call to parent
	QVector<double> xarr, yarr ;
	xarr << minValue() << maxValue() ;
	yarr << yVal << yVal ;
	setSamples(xarr,yarr) ;
	if(plot()) plot()->replot() ;
	if(parent) parent->refreshPlotData() ;
}

QDataStream& operator>>(QDataStream& stream, specKineticRange& range)
{
	double min, max ;
	stream >> min >> max >> range.yVal ;
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "   reading kinetic range:  " << min << "  " << max << "  " << range.yVal << endl ;
	range.pointMoved(1,max,range.yVal) ;
	range.pointMoved(0,min,range.yVal) ;
	return stream ;
}

QDataStream& operator<<(QDataStream& stream, const specKineticRange& range)
{
	return stream << range.minValue() << range.maxValue() << range.yVal ;
}

QMenu* specKineticRange::contextMenu()
{
	return new specKRContextMenu(this) ;
}
