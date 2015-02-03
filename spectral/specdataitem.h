#ifndef SPECDATAITEM_H
#define SPECDATAITEM_H

#include "specmodelitem.h"
#include <QHash>
#include <QVector>
#include "specdatapoint.h"
#include "specdescriptor.h"
#include "specstreamable.h"
#include "speclogentryitem.h"
#include "specdatapointfilter.h"

/*! List item which holds data.*/
class specDataItem : public specLogEntryItem
{
	friend class dataItemProperties ;
	friend class specLegacyDataItem ;
public:
	typedef QVector<specDataPoint> dataContainer ;
private:
	specDataPointFilter filter ;
	dataContainer data ;
	dataContainer correctedData() const ;
	void readFromStream(QDataStream&) ;
	void writeToStream(QDataStream&) const ;
	type typeId() const { return specStreamable::dataItem ; }
	QString exportCoreData(const QList<QPair<int, QString> > &dataFormat, const QStringList &numericDescriptors) const ;
public:
	/*! Standard constructor.*/
	specDataItem(const dataContainer& data,  // TODO change to reference/pointer
		     const QHash<QString, specDescriptor>& description, // TODO change to reference/pointer
		     specFolderItem* par = 0, QString tag = "");
	specDataItem(const specDataItem&) ;
	specDataItem() ;
	/*! Copy constructor necessary because it is disabled in QwtPlotItem */
	//	specDataItem(const specDataItem&) ;

	/* For undo commands managing raw data */
	const dataContainer& allData() const { return data ; }
	void setData(const dataContainer&) ;
	void swapData(dataContainer&) ;
	dataContainer getDataExcept(const QList<specRange*>& ranges) ;
	void applyCorrection(dataContainer&) const ;
	void reverseCorrection(dataContainer&) const ;

	/* Description related */
	QIcon decoration() const ;

	/* Data operations */
	specDataItem& operator+= (const specDataItem& toAdd) ;
	void flatten() ;
	QVector<double> intensityData() const ;
	void refreshPlotData() ;
	specDataPointFilter dataFilter() const;
	void setDataFilter(const specDataPointFilter&) ;
	void addDataFilter(const specDataPointFilter&) ;
	int size() const ;

	/* for plot picker*/
	void attach(QwtPlot* plot) ;
	void detach() ;

	specUndoCommand* itemPropertiesAction(QObject* parentObject) ;
};

class specLegacyDataItem : public specDataItem // TODO durch readAlternative ersetzen
{
private:
	type myType ;
	type typeId() const { return myType; }
	void readFromStream(QDataStream& in) ;
public:
	specLegacyDataItem() ;
};

#endif
