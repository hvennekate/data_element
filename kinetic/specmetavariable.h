#ifndef SPECMETAVARIABLE_H
#define SPECMETAVARIABLE_H
#include <QString>
#include "model/specmodelitem.h"
#include <qwt_interval.h>

class specPlot ;
class specMetaParser ;
class specMetaRange ;
class CanvasPicker ;

class specMetaVariable : public QwtInterval // TODO change to specInterval
{
	int begin, end, inc ;
	bool refreshingRanges ;
	static QString extract(QString&, const QRegExp&) ;
	specMetaParser* parent ; // TODO extract templated version of parent, child to inherit from
				// template <class parent, class child> class parent
				// template <class parent, class child> class child
				// class specMetaVariable : public child<specMetaParser, specMetaVariable>
				// also: template<class container, class T> clearPointers(container<T*>&)
	QVector<specRange*> ranges ;
	QString code ;
	void clearRanges() ;
protected:
	virtual double processPoints(QVector<QPointF>& points) const { Q_UNUSED(points) return 0 ;}
	QString descriptor ;
	bool extractXs(specModelItem *item, QVector<double> & xvals) const ;
public:
	specMetaVariable();
	virtual bool xValues(specModelItem*, QVector<double>&) const;
	virtual QVector<double> values(specModelItem*, const QVector<double>&) const ;
	bool setIndexRange(int& begin, int& end, int& increment, int max) const;
	static specMetaVariable* factory(QString, specMetaParser* parent = 0) ;
	void rangeChanged(specMetaRange*) ;
	void produceRanges(QSet<specPlot*>) ;
	void detachRanges();
	QString codeValue() ;
};

#endif // SPECMETAVARIABLE_H
