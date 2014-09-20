#include "specdataitem.h"
#include <QVariant>
#include <utility-functions.h>
#include <algorithm>
#include "dataitemproperties.h"
#include "specplot.h"

QVector<double> specDataItem::intensityData() const
{
	QVector<double> retval ;
	foreach(const specDataPoint & dataPoint, data)
	retval << dataPoint.mint ;
	return retval ;
}

specDataItem::specDataItem(const QVector<specDataPoint>& dat, const QHash<QString, specDescriptor>& desc, specFolderItem* par, QString description)
	: specLogEntryItem(desc, par, description),
	  data(dat)
{}

specDataItem::specDataItem()
	: specLogEntryItem()
{}

specDataItem::dataContainer specDataItem::correctedData() const
{
	dataContainer result(data) ;
	for(int i = 0 ; i < result.size() ; ++i)
		filter.applyCorrection(result[i]) ;
	return result ;
}

void specDataItem::refreshPlotData()
{
	QVector<QPointF> newData ;
	foreach(const specDataPoint & dataPoint, correctedData())
	newData << dataPoint ;
	setSamples(newData) ;  // TODO QPointF
}

QIcon specDataItem::decoration() const { return QIcon(":/data.png") ; }

void specDataItem::readFromStream(QDataStream& in)
{
	specLogEntryItem::readFromStream(in) ;
	in >> data
	   >> filter ;
}

void specDataItem::writeToStream(QDataStream& out) const
{
	specLogEntryItem::writeToStream(out) ;
	out << data
	    << filter ;
}

specDataItem& specDataItem::operator+= (const specDataItem& toAdd)
{
	// merging descriptors
	foreach(QString key, toAdd.description.keys())
	{
		if(description.keys().contains(key))
			description[key] = (description[key] * data.size()
					    + toAdd.description[key] * toAdd.data.size())
					* (1./(data.size() + toAdd.data.size()));
		else
			description[key] = toAdd.description[key] ;
	}
	if(!descriptor("").contains(toAdd.descriptor("")))
		changeDescriptor("", toAdd.descriptor("", true).prepend(descriptor("", true).isEmpty() ? "" : descriptor("", true).append(", "))) ;
	// merging actual data
	foreach(specDataPoint point, toAdd.correctedData())
	{
		filter.reverseCorrection(point) ;
		data << point ;
	}
	invalidate() ;
	return (*this) ;
}

bool compareDataPoints(const specDataPoint& a, const specDataPoint& b)
{
	return a == b ;
}

void specDataItem::flatten()
{
	qSort(data) ;
	QVector<specDataPoint> newData ;
	averageToNew(data.begin(), data.end(), compareDataPoints, std::back_inserter(newData)) ;
	data.swap(newData) ;
	invalidate() ;
}

QString specDataItem::exportCoreData(const QList<QPair<int, QString> > &dataFormat, const QStringList &numericDescriptors) const
{
	QString result ;
	typedef QPair<int, QString>  formatPair ;
	foreach(const specDataPoint & point, correctedData())
	{
		foreach(const formatPair & format, dataFormat)
		{
			switch(format.first)
			{
				case spec::wavenumber: result += QString::number(point.nu) ; break ;
				case spec::signal: result += QString::number(point.sig) ; break ;
				case spec::maxInt: result += QString::number(point.mint) ; break ;
			default:
				result += descriptor(numericDescriptors[format.first - spec::numericDescriptor]) ;
			}
			result += format.second ;
		}
	}
	return result ;
}

QVector<specDataPoint> specDataItem::getDataExcept(const QList<specRange*>& ranges)
{
	QVector<specDataPoint> newData ;
	for(int i = 0 ; i < data.size() ; ++i)
	{
		specDataPoint point = data[i] ;
		filter.applyCorrection(point) ;
		bool include = true ; // TODO std::copy_remove
		foreach(specRange * range, ranges)
		{
			if(range->contains(point.nu))
			{
				include = false ;
				break ;
			}
		}
		if(include)
			newData << data[i] ;
	}
	return newData ;
}

void specDataItem::applyCorrection(QVector<specDataPoint>& newData) const
{
	for(int i = 0 ; i < newData.size() ; ++i)
		filter.applyCorrection(newData[i]) ;
}

void specDataItem::reverseCorrection(QVector<specDataPoint>& newData) const
{
	for(int i = 0 ; i < newData.size() ; ++i)
		filter.reverseCorrection(newData[i]) ;
}

void specDataItem::swapData(QVector<specDataPoint>& newData)
{
	data.swap(newData) ;
	qSort(data) ;
	invalidate() ;
}

specDataItem::specDataItem(const specDataItem& other)
	: specLogEntryItem(other),
	  filter(other.filter),
	  data(other.data)
{
}

specUndoCommand* specDataItem::itemPropertiesAction(QObject* parentObject)
{
	dataItemProperties propertiesDialog(this) ;
	propertiesDialog.exec() ;
	if(propertiesDialog.result() != QDialog::Accepted) return 0 ;
	return propertiesDialog.changeCommands(parentObject) ;
}

void specDataItem::attach(QwtPlot* pl)
{
	specModelItem::attach(pl) ;
	specPlot* p = qobject_cast<specPlot*> (plot()) ;
	if(p)
		p->attachToPicker(this) ;
}

void specDataItem::detach()
{
	specPlot* p = qobject_cast<specPlot*> (plot()) ;
	if(p)
		p->detachFromPicker(this) ;
	specModelItem::detach() ;
}

specLegacyDataItem::specLegacyDataItem()
	: specDataItem(),
	  myType(legacyDataItem)
{}

void specLegacyDataItem::readFromStream(QDataStream& in)
{
	myType = dataItem ;
	specLogEntryItem::readFromStream(in) ;
	QVector<legacyDatapoint>  legacyData ;
	in >> legacyData
	   >> filter ;
	foreach(legacyDatapoint point, legacyData)
	data << point ;
}


void specDataItem::setDataFilter(const specDataPointFilter& f)
{
	filter = f ;
	invalidate();
}

specDataPointFilter specDataItem::dataFilter() const
{
	return filter ;
}

void specDataItem::addDataFilter(const specDataPointFilter& other)
{
	filter += other ;
	invalidate();
}

