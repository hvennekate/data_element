#ifndef SPECMETAPARSER_H
#define SPECMETAPARSER_H

#include <QString>
#include <QMap>
#include <muParser.h>
#include "specmodelitem.h"
#include <QVector>
#include "specmetavariable.h"

class specMetaParser
{
private:
	bool valid ;
	QStringList errors ;
	QStringList symbols ;
	QList<specMetaVariable*> evaluators ;
	mu::Parser prepare(const QString&) ;
	mu::Parser x, y ;
	QString xExp, yExp ;
	specMetaItem *parent ;
	void clear() ;
	bool containsNan(const QVector<double>&) ;
	bool changingRange ;
public:
	specMetaParser(const QString &expressionList, const QString& xExpression, const QString& yExpression, specMetaItem* parent);
	QString warnings() const ;
	~specMetaParser() { clear() ; }
	bool ok() const ;
	void setAssignments(const QString &expressionList, const QString& xExpression, const QString& yExpression) ;
	QwtSeriesData<QPointF>* evaluate(const QVector<specModelItem*>&);
	void attachRanges(QSet<specPlot*>) ;
	void detachRanges() ;
	specMetaRange::addressObject addressOf(specMetaVariable*) const ;
	void getRangePoint(int variable, int range, int point, double &x, double &y) const ;
	void setRange(int variableNo, int rangeNo, int pointNo, double newX, double newY) ;
};

#endif // SPECMETAPARSER_H
