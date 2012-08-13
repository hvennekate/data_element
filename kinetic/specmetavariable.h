#ifndef SPECMETAVARIABLE_H
#define SPECMETAVARIABLE_H
#include <QString>
#include "model/specmodelitem.h"
#include <qwt_interval.h>
#include <specmetarange.h>

class specPlot ;
class specMetaParser ;
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
	QVector<specMetaRange*> ranges ;
	QString code ;
	void clearRanges() ;
protected:
	virtual double processPoints(QVector<QPointF>& points) const { Q_UNUSED(points) return 0 ;}
	QString descriptor ;
	bool extractXs(specModelItem *item, QVector<double> & xvals) const ;
public:
	explicit specMetaVariable(specMetaParser *parent = 0);
	~specMetaVariable() ;
	virtual bool xValues(specModelItem*, QVector<double>&) const;
	virtual QVector<double> values(specModelItem*, const QVector<double>&) const ;
	bool setIndexRange(int& begin, int& end, int& increment, int max) const;
	static specMetaVariable* factory(QString, specMetaParser* parent = 0) ;
	void produceRanges(QSet<specPlot*>) ;
	void detachRanges();
	QString codeValue() const ;
	specMetaRange::addressObject address(specMetaRange*) ;
	void getRangePoint(int range, int point, double& x, double& y) const ;
	void setRange(int rangeNo, int pointNo, double newX, double newY) ;
};

#endif // SPECMETAVARIABLE_H
