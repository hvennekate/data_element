#ifndef SPECDATAITEM_H
#define SPECDATAITEM_H

#include "specmodelitem.h"
#include <QHash>
#include <QVector>
#include "specdatapoint.h"
#include "specdescriptor.h"
#include "specstreamable.h"

/*! List item which holds data.*/
class specDataItem : public specModelItem
{
	friend class dataItemProperties ;
private:
	double offset, slope, factor, xshift ;
	int zeroMultiplications ;
	QHash<QString,specDescriptor> description ;
	QVector<specDataPoint> data ;
	QVector<double> wnums() const ;
	QVector<double> ints() const ;
	QVector<double> times() const ;
	void applyCorrection(specDataPoint&) const ;  /*!< Korrektur anwenden. */
	void reverseCorrection(specDataPoint&) const; /*!< Korrektur rueckhaengig machen. */
	void readFromStream(QDataStream &) ;
	void writeToStream(QDataStream &) const ;
	type typeId() const { return specStreamable::dataItem ; }
public:
	/*! Standard constructor.*/
	specDataItem(const QVector<specDataPoint> &data, // TODO change to reference/pointer
		     const QHash<QString,specDescriptor> &description, // TODO change to reference/pointer
		     specFolderItem* par=0, QString tag="");
	specDataItem(const specDataItem&) ;
	specDataItem() ;
	/*! Copy constructor necessary because it is disabled in QwtPlotItem */
//	specDataItem(const specDataItem&) ;

	/* For undo commands managing raw data */
	const QVector<specDataPoint>& allData() const { return data ; }
	void setData(const QVector<specDataPoint>&) ;
	QVector<specDataPoint> getDataExcept(const QList<specRange*>& ranges) ;
	void applyCorrection(QVector<specDataPoint>&) const ;
	void reverseCorrection(QVector<specDataPoint>&) const ;

	/* Description related */
	QString descriptor(const QString &key, bool full =true) const ;
	bool changeDescriptor(QString key, QString value) ;
	bool isEditable(QString key) const ;
	bool setActiveLine(const QString &, int) ;
	int activeLine(const QString &key) const ;
	QStringList descriptorKeys() const ;
	spec::descriptorFlags descriptorProperties(const QString& key) const ;

	QIcon decoration() const ;

	/* Data operations */
	specDataItem& operator+=(const specDataItem& toAdd) ;
	void flatten(bool oneTime=true) ;
	void scaleBy(const double&) ;
	void addToSlope(const double&) ;
	void moveYBy(const double&) ;
	void moveXBy(const double&) ;
	QVector<double> intensityData() const ;
	void refreshPlotData() ;

	/* not reviewed */
	void exportData(const QList<QPair<bool,QString> >&, const QList<QPair<spec::value,QString> >&, QTextStream&) const ;

	/* Functions to be deleted soon: */
	void subMap(const QMap<double, double>&) ;
	int removeData(QList<specRange*>*) ; // TODO remove
	specUndoCommand *itemPropertiesAction(QObject *parentObject) ;
};

#endif
