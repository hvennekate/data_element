#include "specrange.h"
#include <qwt_symbol.h>
#include <QTextStream>
#include "specplot.h"

specRange::specRange(double min, double max)
	: QwtDoubleInterval(min,max)
{ // TODO set curve properties
	QwtArray<double> x, y ;
	x << min << max ;
	y << 0 << 0 ;
	QwtPlotCurve::setData(x,y) ;
	
	QPen pen ( QColor ( 255,139,15,100 ) ) ;
	pen.setWidth ( 5 ) ;
	pen.setCapStyle(Qt::RoundCap);
	setPen ( pen ) ;
	pen.setColor ( QColor ( 255,139,15 ) ) ;
	setSymbol ( QwtSymbol ( QwtSymbol::Ellipse,pen.brush(), ( QPen ) pen.color(),QSize ( 5,5 ) ) ) ;
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

void specRange::refreshPlotData()
{
	QwtArray<double> xarr, yarr ;
	xarr << minValue() << maxValue() ;
	yarr << yVal << yVal ;
	setData(xarr,yarr) ;
	((specPlot*) plot())->refreshRanges() ;
}

specRange::~specRange()
{
}
