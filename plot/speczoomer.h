#ifndef SPECZOOMER_H
#define SPECZOOMER_H

#include <qwt_plot_zoomer.h>

class specZoomer : public QwtPlotZoomer
{
public:
	specZoomer(QwtPlotCanvas *);
	virtual QwtText trackerText(const QwtDoublePoint &pos) const ;
	void changeZoomBase(const QwtDoubleRect&) ;
	~specZoomer();

};

#endif
