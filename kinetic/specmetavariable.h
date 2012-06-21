#ifndef SPECMETAVARIABLE_H
#define SPECMETAVARIABLE_H
#include <QString>
#include "model/specmodelitem.h"
#include <qwt_interval.h>

class specMetaVariable
{
	int begin, end, inc ;
	bool valid, all, single, array ;
	QChar mode ;
	QString descriptor ;
	QwtInterval interval ;
	QString extract(QString&, const QRegExp&) const ;
public:
	specMetaVariable(QString);
	bool xValues(specModelItem*, QVector<double>&) const;
	QVector<double> values(specModelItem*, const QVector<double>&) const ;
	bool setRange(int& begin, int& end, int& increment, int max) const;
	bool completeRange() const ;
};

#endif // SPECMETAVARIABLE_H
