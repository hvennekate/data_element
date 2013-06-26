#include "specrange.h"
#include <qwt_symbol.h>
#include <QTextStream>
#include "specplot.h"

specRange::specRange(double min, double max, double yinit)
	: QwtInterval(min,max,QwtInterval::IncludeBorders) // TODO check if ok
{ // TODO set curve properties
	QVector<double> x, y ;
	x << min << max ;
	y << yinit << yinit ;
	QwtPlotCurve::setSamples(x,y) ;

	QPen pen ( QColor ( 255,139,15,100 ) ) ;
	pen.setWidth ( 5 ) ;
	pen.setCapStyle(Qt::RoundCap);
	setPen ( pen ) ;
	pen.setColor ( QColor ( 255,139,15 ) ) ;
	setSymbol ( new QwtSymbol ( QwtSymbol::Ellipse,pen.brush(), ( QPen ) pen.color(),QSize ( 5,5 ) ) ) ;
}

void specRange::pointMoved(const int& point, const double& x, const double& y)
{
	if (point == 0)
		setMinValue(x) ;
	else
		setMaxValue(x) ;

	if (!isValid())
		setInterval(maxValue(),minValue()) ;
	yVal = y ;
	refreshPlotData() ;
}

void specRange::writeToStream(QDataStream &out) const
{
	specCanvasItem::writeToStream(out) ;
	out << qreal(yVal) << qreal(minValue()) << qreal(maxValue()) ;
}

void specRange::readFromStream(QDataStream &in)
{
	qreal y, min, max ;
	in >> y >> min >> max ;
	yVal = y ;
	setInterval(min, max, QwtInterval::IncludeBorders) ;
	refreshPlotData();
}

void specRange::refreshPlotData()
{
	QVector<double> xarr, yarr ;
	xarr << minValue() << maxValue() ;
	yarr << yVal << yVal ;
	setSamples(xarr,yarr) ;
	//	((specPlot*) plot())->refreshRanges() ; // TODO check if this works for kinetics also
}

specRange::~specRange()
{
	detach();
}
