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
	void clear() ;
public:
	specMetaParser(const QString &expressionList, const QString& xExpression, const QString& yExpression);
	QString warnings() const ;
	~specMetaParser() { clear() ; }
	bool ok() const ;
	void setAssignments(const QString &expressionList, const QString& xExpression, const QString& yExpression) ;
	QwtSeriesData<QPointF>* evaluate(const QVector<specModelItem*>&);
};

#endif // SPECMETAPARSER_H
