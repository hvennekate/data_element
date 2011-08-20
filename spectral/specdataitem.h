#ifndef SPECDATAITEM_H
#define SPECDATAITEM_H

#include "specmodelitem.h"
#include <QHash>
#include "specdatapoint.h"
#include "specdescriptor.h"

struct specDataFilter{ // TODO Eventuell direkt in specDataItem integrieren oder als friend Klasse.
	int x,y ;
	double xmin, xmax, ymin, ymax, offset, slope, factor ;
	specDataFilter() : x(1), y(2), xmin(-INFINITY), xmax(INFINITY), ymin(-INFINITY), ymax(INFINITY), offset(0), slope(0), factor(1) {}
	QVariant plotData(const QList<specDataPoint>&) const;
	QVector<double> wnums(const QList<specDataPoint>&) const ;
	QVector<double> ints(const QList<specDataPoint>&) const ;
	QVector<double> times(const QList<specDataPoint>&) const ;
	QVector<double> mints(const QList<specDataPoint>&) const ;
	
	bool addData(QList<specDataPoint>&, const QVector<double>&, const QVector<double>&, const QVector<double>&, const QVector<double>&) ;
	void subMap(QList<specDataPoint>& data, const QMap<double,double>& toSub) ;
	void addX(QList<specDataPoint>& data, const double& value) ;
	private:
		void applyCorrection(double &t, double &wn, double &in) const;
		void reverseCorrection(double &t, double &wn, double &in) const;
};

/*! List item which holds data.*/
class specDataItem : public specModelItem
{
private:
	QHash<QString,specDescriptor> description ;
	QList<specDataPoint> data ;
	specDataFilter filter ;
	int addedItems ;
	QVector<double> wnums() const ;
	QVector<double> ints() const ;
	QVector<double> mints() const ;
	QVector<double> times() const ;
protected:
	QDataStream& readFromStream(QDataStream&) ;
	QDataStream& writeToStream(QDataStream&) const ;
public:
	/*! Standard constructor.*/
	specDataItem(QList<specDataPoint> data, // TODO change to reference/pointer
		     QHash<QString,specDescriptor> description, // TODO change to reference/pointer
		     specFolderItem* par=0, QString tag="");
	/*! Standard destructor.*/
	~specDataItem();

	int removeData(QList<specRange*>*) ;
	QMultiMap<double,QPair<double,double> >* kinetics(QList<specKineticRange*>) const;
	/*! Return value of descriptor \a key. */
	QString descriptor(const QString &key) const ;
	/*! Is the descriptor \a key editable?*/
	bool isEditable(QString key) const ;
	/*! Set descriptor \a key 's value to  \a value. */
	bool changeDescriptor(QString key, QString value) ;
	/*! Regenerate data used for plotting. @todo maybe not use spectrum function??? */
	void refreshPlotData() ;
	/*! Try to set data whose \f$\nu\f$ value is in \a ranges to zero by applying some correction (preferrably by subtracting a linear function previously fit to the data concerned).*/
	QIcon decoration() const ;
	QStringList descriptorKeys() const ;
	specDataItem& operator+=(const specDataItem& toAdd) ;
	void subMap(const QMap<double, double>&) ;
	spec::descriptorFlags descriptorProperties(const QString& key) const ;
	void flatten(bool timeAverage=true, bool oneTime=true) ;
	/*! Reimplementation from specCanvasItem -- scales y-factor, x-slope and y-offset by given factor */
	void scaleBy(const double&) ;
	/*! Reimplementation from specCanvasItem -- adds given value to x-slope */
	void addToSlope(const double&) ;
	/*! Reimplementation from specCanvasItem -- adds given value to y-offset */
	void moveYBy(const double&) ;
	void moveXBy(const double&) ;
	void exportData(const QList<QPair<bool,QString> >&, const QList<QPair<spec::value,QString> >&, QTextStream&) ;
	QVector<double> intensityData() ;
	void movingAverage(int) ;
	void average(int) ;
};

#endif
