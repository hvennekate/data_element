#ifndef SPECFITCURVE_H
#define SPECFITCURVE_H
#include "specstreamable.h"

class specFitCurve : specStreamable
{
	QList<QPair<QString, double> > variables ;
	QStringList fitParameters ;
	QString expression ;

public:
	specFitCurve();
	QStringList descriptorKeys() ;
	QString descriptor(const QString& key, bool full=false) ;
	QString changeDescriptor(QString key, QString value) ;
};

#endif // SPECFITCURVE_H
