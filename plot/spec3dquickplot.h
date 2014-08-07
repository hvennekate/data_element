#ifndef SPEC3DQUICKPLOT_H
#define SPEC3DQUICKPLOT_H

#include <QVector3D>
#include "qwt3d_surfaceplot.h"

class specPlot ;
class QwtPlotItem ;

class spec3dQuickPlot : public Qwt3D::SurfacePlot
{
	Q_OBJECT
private:
	const specPlot* plot ;
	QString Descriptor ;
	Qwt3D::TripleField dataPoints ;
	Qwt3D::CellField cells ;
	void refreshDataPoints() ;
	double xrange, yrange, zrange ;
public:
	spec3dQuickPlot(QWidget* parent = 0);
	void setPlot(const specPlot*) ;
public slots:
	void setDescriptor(const QString&) ;
private slots:
	void Plot() ;
};

#endif // SPEC3DQUICKPLOT_H
