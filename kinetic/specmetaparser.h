#ifndef SPECMETAPARSER_H
#define SPECMETAPARSER_H

#include <QString>
#include <QMap>
#include <ginac/ginac.h>
#include "specmodelitem.h"
#include <QVector>
#include "specmetavariable.h"

class specMetaParser
{
private:
	bool valid ;
	QStringList errors ;
	GiNaC::lst symbols ;
	QList<specMetaVariable*> evaluators ;
	GiNaC::ex prepare(const QString&) ;
	GiNaC::ex x, y ;
	QString xExp, yExp ;
	specMetaItem *parent ;
	void clear() ;
	bool containsNan(const QVector<double>&) ;
	bool changingRange ;
public:
	specMetaParser(const QString &expressionList, const QString& xExpression, const QString& yExpression, specMetaItem* parent);
	void evaluatorIntervalChanged() ;
	QString warnings() const ;
	~specMetaParser() { clear() ; }
	bool ok() const ;
	void setAssignments(const QString &expressionList, const QString& xExpression, const QString& yExpression) ;
	QwtSeriesData<QPointF>* evaluate(const QVector<specModelItem*>&);
	void attachRanges(QSet<specPlot*>) ;
	void detachRanges() ;
};

#endif // SPECMETAPARSER_H
