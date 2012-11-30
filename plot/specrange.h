#ifndef SPECRANGE_H
#define SPECRANGE_H

#include "speccanvasitem.h"
#include <qwt_interval.h>
#include <names.h>

class specRange : public specCanvasItem, public QwtInterval // TODO make private and channel/wrap required functionality
{
private:
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::range ; }
protected:
	double yVal ;
public:
    specRange ( double,double, double y = 0 );
	void pointMoved ( const int&, const double&, const double& ) ;
	virtual void refreshPlotData() ;
	int rtti() const { return spec::zeroRange ; }
	~specRange();
};

#endif
