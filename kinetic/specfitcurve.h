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
		mu::Parser* parser ;
		double* x ;
		QVector<QPointF> samples ;
	public:
		QString errorString ;
		fitData(mu::Parser* p = 0) ;
		~fitData() ;
		QRectF boundingRect() const ;
		QPointF sample(size_t i) const ;
		void setRectOfInterest(const QRectF&) ;
		size_t size() const ;
		void reevaluate() ;
	};

	typedef QPair<QString, double> variablePair;
	QList<variablePair> variables ;
	double confidenceInterval(const QString& v) const ;
	QString plainVariableName(const QString& v) const ;
	QVector<double> numericalErrors ;
	qint16 activeVar ;
	QStringList fitParameters ;
	specDescriptor expression ;
	mu::Parser* parser ;
	QString errorString ;
	static bool acceptableVariable(const QString&) ;
	inline static bool inacceptableVariable(const QString& s) { return !acceptableVariable(s) ; }
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::fitCurve ; }
	void clearParser() ;
	void generateParser() ;
	void setParserConstants();
	bool variablesMulti, expressionMulti, messagesMulti ;
public:
	specFitCurve();
	~specFitCurve() ;
	QStringList descriptorKeys() ;
	static QStringList genericDescriptorKeys() ;
	QString descriptor(const QString& key, bool full = false) ;
	QString editDescriptor(const QString& key) ;
	bool changeDescriptor(QString key, QString value) ;
	bool setActiveLine(const QString& key, int n) ;
	int activeLine(const QString& key) const ;
	void refreshPlotData();
	void refit(QwtSeriesData<QPointF>* data) ;
	QVector<double> getFitData(QwtSeriesData<QPointF>* data) ;
	int rtti() const { return specStreamable::fitCurve ; }
	void attach(QwtPlot* plot) ;
	void setDescriptorProperties(QString key, spec::descriptorFlags f) ;
	spec::descriptorFlags descriptorProperties(const QString& key) const ;
};

// from lmcurve.c:
typedef struct
{
	double* x;
	const double* y;
	mu::Parser* parser ;
	QVector<std::string>* variableNames ;
} lmcurve_data_struct;

void evaluateParser(const double* parameters, int count, const void* data, double* fitResults, int* info) ;

#endif // SPECFITCURVE_H
