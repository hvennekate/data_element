#ifndef SPECRANGE_H
#define SPECRANGE_H

#include "speccanvasitem.h"
#include <qwt_interval.h>
#include <names.h>

class specRange : public specCanvasItem, public QwtInterval // TODO make private and channel/wrap required functionality
{
protected:
	double yVal ;
public:
	specRange ( double,double );
	void pointMoved ( const int&, const double&, const double& ) ;
	virtual void refreshPlotData() ;
	int rtti() const { return spec::zeroRange ; }
	~specRange();
};

class specSelectRange : public specRange
{
public:
	specSelectRange(double a, double b) : specRange(a,b) {}
} ;

#endif
