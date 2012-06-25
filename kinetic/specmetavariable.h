#ifndef SPECMETAVARIABLE_H
#define SPECMETAVARIABLE_H
#include <QString>
#include "model/specmodelitem.h"
#include <qwt_interval.h>

class specMetaVariable
{
	int begin, end, inc ;
	QwtInterval interval ;
	QString extract(QString&, const QRegExp&) const ;
protected:
	virtual double processPoints(QVector<QPointF>& points) const { return 0 ;}
	QString descriptor ;
public:
	specMetaVariable();
	bool xValues(specModelItem*, QVector<double>&) const;
	virtual QVector<double> values(specModelItem*, const QVector<double>&) const ;
	bool setIndexRange(int& begin, int& end, int& increment, int max) const;
	static specMetaVariable* factory(QString) ;
};

#endif // SPECMETAVARIABLE_H
