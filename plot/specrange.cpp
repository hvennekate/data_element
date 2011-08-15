#include "specrange.h"
#include <qwt_symbol.h>
#include <QTextStream>
#include "specplot.h"

specRange::specRange(double min, double max)
	: QwtInterval(min,max)
{ // TODO set curve properties
	QVector<double> x, y ;
	x << min << max ;
	y << 0 << 0 ;
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

void specRange::refreshPlotData()
{
	QVector<double> xarr, yarr ;
	xarr << minValue() << maxValue() ;
	yarr << yVal << yVal ;
	setSamples(xarr,yarr) ;
	((specPlot*) plot())->refreshRanges() ;
}

specRange::~specRange()
{
}
