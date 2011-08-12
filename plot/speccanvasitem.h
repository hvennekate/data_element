#ifndef SPECCANVASITEM_H
#define SPECCANVASITEM_H

#include <qwt_plot_curve.h>
#include <QList>
#include <QPair>
#include <qwt_double_interval.h>
#include "names.h"
#include <QMenu>

class specCanvasItem : public QwtPlotCurve
{
	private:
		QList<int> selectedPoints ;
	public:
		specCanvasItem(QString description="");
		virtual void pointMoved(const int&, const double&, const double&) ;
		virtual void applyRanges(QList<QwtDoubleInterval*>&) ;
		virtual void refreshPlotData() =0;
		virtual void scaleBy(const double&) {}
		virtual void addToSlope(const double&) {}
		virtual void moveYBy(const double&) {}
		virtual void moveXBy(const double&) {}
		virtual QMenu* contextMenu() ;
		void setLineWidth() ;
		~specCanvasItem();

};

#endif
