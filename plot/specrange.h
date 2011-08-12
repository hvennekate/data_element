#ifndef SPECRANGE_H
#define SPECRANGE_H

#include "speccanvasitem.h"
#include <qwt_double_interval.h>
#include <names.h>

class specRange : public specCanvasItem, public QwtDoubleInterval // TODO make private and channel/wrap required functionality
{
protected:
	double yVal ;
public:
	specRange ( double,double );
	void pointMoved ( const int&, const double&, const double& ) ;
	virtual void refreshPlotData() ;
	~specRange();

};

class specSelectRange : public specRange
{
public:
	specSelectRange(double a, double b) : specRange(a,b) {}
} ;

#endif
