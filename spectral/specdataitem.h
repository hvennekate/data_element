#ifndef SPECDATAITEM_H
#define SPECDATAITEM_H

#include "specmodelitem.h"
#include <QHash>
#include "specdatapoint.h"
#include "specdescriptor.h"

/*! \brief Filter-Klasse fuer Plotdaten

  Filtert Daten nach
	\f$ \tilde{y} = a\cdot y + b\cdot x + c \f$
 */
class specDataFilter
{ // TODO Eventuell direkt in specDataItem integrieren oder als friend Klasse.
private:
	double offset, slope, factor ;
	void applyCorrection(double &t, double &wn, double &in) const; /*!< Korrektur anwenden. */
	void reverseCorrection(double &t, double &wn, double &in) const; /*!< Korrektur rueckhaengig machen. */
public:
	specDataFilter() : offset(0), slope(0), factor(1) {}
	QVector<double> wnums(const QList<specDataPoint>&) const ; /*!< Wellenzahlen */
	QVector<double> ints(const QList<specDataPoint>&) const ;  /*!< Intensitaeten */
	QVector<double> times(const QList<specDataPoint>&) const ; /*!< Zeiten */
	QVector<double> mints(const QList<specDataPoint>&) const ; /*!< Maximalintensitaeten */
	
	bool addData(QList<specDataPoint>& existingList,
		     const QVector<double>& newTimes,
		     const QVector<double>& newWnums,
		     const QVector<double>& newInts,
		     const QVector<double>& newMInts) const ;
	void subMap(QList<specDataPoint>& data, const QMap<double,double>& toSub) ;
	void addX(QList<specDataPoint>& data, const double& value) ;
};

/*! List item which holds data.*/
class specDataItem : public specModelItem
{
private:
	QHash<QString,specDescriptor> description ;
	QList<specDataPoint> spectralData ;
	specDataFilter filter ;
	QVector<double> wnums() const ;
	QVector<double> ints() const ;
	QVector<double> mints() const ;
	QVector<double> times() const ;
protected:
	QDataStream& readFromStream(QDataStream&) ;
	QDataStream& writeToStream(QDataStream&) const ;
public:
	/*! Standard constructor.*/
	specDataItem(QList<specDataPoint> spectralData, // TODO change to reference/pointer
		     QHash<QString,specDescriptor> description, // TODO change to reference/pointer
		     specFolderItem* par=0, QString tag="");
	specDataItem(const specDataItem&) ;
	/*! Standard destructor.*/
	~specDataItem();

	int removeData(QList<specRange*>*) ;
	void removeData(const QVector<int>&) ;
	QList<specDataPoint> getData(const QVector<int>& indexes) ;
	const QList<specDataPoint>& allData() ;
	void insertData(const QList<specDataPoint>&) ;
	void clearData() ;
	QMultiMap<double,QPair<double,double> >* kinetics(QList<specKineticRange*>) const;
	/*! Return value of descriptor \a key. */
	QString descriptor(const QString &key, bool full =true) const ;
	/*! Is the descriptor \a key editable?*/
	bool isEditable(QString key) const ;
	/*! Set descriptor \a key 's value to  \a value. */
	bool changeDescriptor(QString key, QString value) ;
	bool setActiveLine(const QString &, int) ;
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
