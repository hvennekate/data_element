#include "spec3dquickplot.h"
#include "specplot.h"
#include "specdataitem.h"
#include <cmath>
#include <QTextDocument>

spec3dQuickPlot::spec3dQuickPlot(QWidget *parent)
	: Qwt3D::SurfacePlot(parent),
	  plot(0),
	  xrange(1),
	  yrange(1),
	  zrange(1)
{
}

bool compareQPointF(const QPointF& a, const QPointF& b)
{
	if (a.x() == b.x())
		return a.y() < b.y() ;
	return a.x() < b.x() ;
}

void spec3dQuickPlot::refreshDataPoints()
{
	dataPoints.clear();
	cells.clear();
	if (!plot) return ;

	// Pass 1: collect yValues, (minX, maxX)
	double minx = INFINITY, maxx = -INFINITY ;
	QVector<specDataItem*> dataItems ;
	foreach (QwtPlotItem* plotItem, plot->itemList(spec::spectrum))
	{
		specDataItem* item = dynamic_cast<specDataItem*>(plotItem) ;
		if (!item) continue ;
		minx = qMin(minx, item->minXValue()) ;
		maxx = qMax(maxx, item->maxXValue()) ;
		dataItems << item ;
	}

	// Pass 2: collect spectra (points only if x in [minx,maxx]), grouped by yValue
	// Pass 2a: collect x values
	QMultiMap<double, QList<QPointF> > dataByY ;
	QSet<double> xValueSet ;
	foreach(specDataItem* item, dataItems)
	{
		QList<QPointF> points ;
		foreach(const QPointF& p, item->dataVector())
		{
			double x = p.x() ;
			if (x >= minx && x <= maxx)
			{
				points << p ;
				xValueSet << x ;
			}
		}
		qSort(points.begin(), points.end(), compareQPointF) ; // TODO: average multiple x Values
		dataByY.insertMulti(item->descriptorValue(Descriptor), points) ;
	}
	QList<double> xValues = xValueSet.toList() ;
	qSort(xValues) ;

	// Pass 3: average spectra
	// Pass 3a: interpolate spectra
	double minz = INFINITY, maxz = -INFINITY ;
	foreach(const double & y, dataByY.keys())
	{
		QVector<QList<QPointF>::const_iterator> positions, ends ;
		foreach(const QList<QPointF>& dataSet, dataByY.values(y))
		{
			positions << dataSet.constBegin() ;
			ends << dataSet.constEnd() ;
		}
		for (QList<double>::const_iterator xit = xValues.begin() ; xit != xValues.end() ; ++xit)
		{
			int divisor = 0 ;
			double zSum = 0 ;
			for (int i = 0 ; i < positions.size() ; ++i)
			{
				QList<QPointF>::const_iterator initial = positions[i] ;
				QList<QPointF>::const_iterator final = initial ;
				double sum = 0;
				while (final != ends[i] && final->x() == *xit)
				{
					sum += final->y() ;
					++ final ;
				}
				if (final != initial) // ok, wir haben min. einen gueltigen Datenpunkt gesehen
				{
					zSum += sum / (final - initial) ;
					positions[i] = final ;
					++ divisor ;
					continue ;
				}
				if (final == ends[i]) continue ; // No interpolation possible
				if (final->x() > *xit) continue ; // Beyond interpolation
				if (final +1 == ends[i]) continue ; // Should not happen, but:  also no interpolation possible
				// interpolate:
				double xfactor = (*xit - final->x()) / ((final+1)->x() - final->x()) ;
				zSum += (final+1)->y() * xfactor + (1-xfactor) * final->y() ;
				++ divisor ;
				if ((xit+1) != xValues.end() && (final+1)->x() == *(xit+1))
					positions[i] = final + 1;
			}
			double z = zSum / divisor ;
			dataPoints.push_back(Qwt3D::Triple(*xit, y, z)) ;
			if (std::isfinite(z))
			{
				maxz = qMax(z,maxz) ;
				minz = qMin(z,minz) ;
			}
		}
	}

	// generate cells
	for (int j = 0 ; j < dataByY.keys().size()-1 ; ++j)
	{
		for (int i = 0 ; i < xValues.size()-1 ; ++i)
		{
			Qwt3D::Cell c1 ;
			c1.push_back(i+xValues.size()*j);
			c1.push_back(i+1+xValues.size()*j);
			c1.push_back(i+1+xValues.size()*(j+1));
			c1.push_back(i+xValues.size()*(j+1));
			cells.push_back(c1) ;
		}
	}

	// scaling parameters
	xrange = qAbs(maxx - minx) ;
	yrange = dataByY.isEmpty() ? NAN : qAbs(dataByY.keys().last() - dataByY.keys().first()) ;
	zrange = qAbs(maxz - minz) ;
	if (!std::isfinite(xrange)) xrange = 1 ;
	if (!std::isfinite(yrange)) yrange = 1 ;
	if (!std::isfinite(zrange)) zrange = 1 ;
}

void spec3dQuickPlot::Plot()
{
	refreshDataPoints();

	loadFromData(dataPoints, cells) ;
	setCoordinateStyle(Qwt3D::BOX);

	double maxrange = qMax(qMax(xrange, yrange), zrange) ;
	setScale(maxrange/xrange,maxrange/yrange,maxrange/zrange);
//	setPlotStyle(Qwt3D::POINTS);
//	setOrtho(false) ;

	for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
	{
		coordinates()->axes[i].setMajors(7);
		coordinates()->axes[i].setMinors(4);
	}

	QVector<Qwt3D::AXIS> xaxes, yaxes, zaxes ;
	xaxes << Qwt3D::X1 << Qwt3D::X2 << Qwt3D::X3 << Qwt3D::X4 ;
	yaxes << Qwt3D::Y1 << Qwt3D::Y2 << Qwt3D::Y3 << Qwt3D::Y4 ;
	zaxes << Qwt3D::Z1 << Qwt3D::Z2 << Qwt3D::Z3 << Qwt3D::Z4 ;

#define LABELSETTINGMACRO(LABELSTRING,AXISSET) \
foreach (Qwt3D::AXIS axis, AXISSET) \
{ \
	coordinates()->axes[axis].setLabelString(LABELSTRING); \
	coordinates()->axes[axis].setLabel(true) ; \
	coordinates()->axes[axis].setLabelFont(font()) ; \
	coordinates()->axes[axis].setNumberFont(font()) ; \
}


	if (plot)
	{
		QTextDocument text ;
		text.setHtml(plot->axisTitle(specPlot::xBottom).text()) ;
		QString xtext = text.toPlainText() ;
		text.setHtml(plot->axisTitle(specPlot::yLeft).text()) ;
		QString ztext = text.toPlainText() ;
		LABELSETTINGMACRO(xtext, xaxes) ;
		LABELSETTINGMACRO(ztext, zaxes) ;
	}

	LABELSETTINGMACRO(Descriptor, yaxes) ;

	setFloorStyle(Qwt3D::FLOORISO);

	updateData() ;
	updateGL() ;
}

void spec3dQuickPlot::setDescriptor(const QString & d)
{
	Descriptor = d ;
	Plot();
}

void spec3dQuickPlot::setPlot(const specPlot *p)
{
	plot = p ;
	if (plot)
		disconnect(plot, 0, this, 0) ;
	connect(p, SIGNAL(replotted()), this, SLOT(Plot())) ;
	Plot();
}
