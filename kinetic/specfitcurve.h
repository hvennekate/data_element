#ifndef SPECFITCURVE_H
#define SPECFITCURVE_H
#include "specstreamable.h"
#include <muParser.h>
#include "speccanvasitem.h"
#include <QRectF>
#include "specdescriptor.h"

class specFitCurve : public specCanvasItem
{
private:
	class fitData : public QwtSeriesData<QPointF>
	{
	private:
		static const int fixedSize = 1000 ;
		mu::Parser *parser ;
		double *x ;
		QVector<QPointF> samples ;
	public:
		fitData(mu::Parser* p = 0) ;
		~fitData() ;
		QRectF boundingRect() const ;
		QPointF sample(size_t i) const ;
		void setRectOfInterest(const QRectF &) ;
		size_t size() const ;
		void reevaluate() ;
	};

	typedef QPair<QString,double> variablePair;
	QList<variablePair> variables ;
	qint16 activeVar ;
	QStringList fitParameters ;
	specDescriptor expression ;
	mu::Parser *parser ;
	QString errorString ;
	double coerce(double val, double min, double max) ;
	static bool acceptableVariable(const QString&) ;
	inline static bool inacceptableVariable(const QString& s) { return !acceptableVariable(s) ; }
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::fitCurve ; }
	void clearParser() ;
	void generateParser() ;
	void setParserConstants();
public:
	specFitCurve();
	~specFitCurve() ;
	QStringList descriptorKeys() ;
	QString descriptor(const QString& key, bool full=false) ;
	bool changeDescriptor(QString key, QString value) ;
	bool setActiveLine(const QString& key, int n) ;
	int activeLine(const QString& key) const ;
	void refreshPlotData();
	void refit(QwtSeriesData<QPointF>* data) ;
	int rtti() const { return specStreamable::fitCurve ; }
	void attach(QwtPlot *plot) ;
};

// from lmcurve.c:
typedef struct {
    double *x;
    const double *y;
    mu::Parser *parser ;
    QList<std::string>* variableNames ;
} lmcurve_data_struct;

void evaluateParser(const double *parameters, int count, const void *data, double *fitResults, int *info) ;

#endif // SPECFITCURVE_H
