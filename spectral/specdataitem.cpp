#include "specdataitem.h"
#include <QVariant>
#include <utility-functions.h>
#include <algorithm>
#include "dataitemproperties.h"
#include "specplot.h"


void specDataItem::applyCorrection(specDataPoint& point) const
{
	point.nu += xshift ;
    point.sig = zeroMultiplications ? 0 : point.sig*factor+offset+slope*point.nu ;
}

void specDataItem::reverseCorrection(specDataPoint& point) const
{
	point.sig = (point.sig-offset-slope*point.nu)/factor ;
	point.nu = point.nu - xshift ;
}

void specDataItem::subMap(const QMap<double, double> &toSub)
{
	for (int i = 0 ; i < data.size() ; i++)
	{
		specDataPoint point = data[i] ;
		applyCorrection(point) ;
		if (toSub.contains(point.nu))
			point.sig -= toSub[point.nu] ;
		reverseCorrection(point) ;
		data[i] = point ;
	}
	invalidate() ;
}

QVector<double> specDataItem::wnums() const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		specDataPoint point = data[i] ;
		applyCorrection(point) ;
		retval << point.nu ;
	}
	return retval ;
}

QVector<double> specDataItem::intensityData() const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
		retval << data[i].mint ;
	return retval ;
}

QVector<double> specDataItem::ints() const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		specDataPoint point = data[i] ;
		applyCorrection(point) ;
		retval << point.sig ;
	}
	return retval ;
}

QVector<double> specDataItem::times() const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
		retval << data[i].t ;
	return retval ;
}



specDataItem::specDataItem(const QVector<specDataPoint>& dat, const QHash<QString,specDescriptor>& desc,
			   specFolderItem* par, QString description)
	: specModelItem(par,description),
	  offset(0),
	  slope(0),
	  factor(1),
	  xshift(0),
	  zeroMultiplications(0),
	  description(desc),
	  data(dat)
{
}

specDataItem::specDataItem()
	: specModelItem(0,""),
	  offset(0),
	  slope(0),
	  factor(1),
	  xshift(0),
	  zeroMultiplications(0)
{
}

bool specDataItem::isEditable(QString key) const
{
	if (key== QString(""))
		return true ;
	if (description.contains(key))
	{
		return description[key].isEditable() ;
	}
	
	return true ;
}

bool specDataItem::changeDescriptor(QString key, QString value)
{
	if (key=="")
		return specModelItem::changeDescriptor(key,value) ;
	if (description.contains(key))
		return description[key].setContent(value) ;
	
	description[key] = specDescriptor(value,spec::editable) ;
	return true ;
}

bool specDataItem::setActiveLine(const QString& key, int line)
{
	if (description.contains(key))
		return description[key].setActiveLine(line) ;
	return specModelItem::setActiveLine(key,line) ;
}

int specDataItem::activeLine(const QString& key) const
{
	if (description.contains(key))
		return description[key].activeLine() ;
	return specModelItem::activeLine(key) ;
}

void specDataItem::refreshPlotData()
{
	QVector<double> x=wnums(), y=ints();
	processData(x,y) ;
	setSamples(x,y) ; // TODO QPointF
}

QString specDataItem::descriptor(const QString &key, bool full) const
{
	if (description.contains(key))
		return description[key].content(full) ;
	return specModelItem::descriptor(key, full) ;
}

QIcon specDataItem::decoration() const { return QIcon(":/data.png") ; }

QStringList specDataItem::descriptorKeys() const
{
	return (specModelItem::descriptorKeys() << description.keys()) ;
}

void specDataItem::readFromStream(QDataStream &in)
{
	specModelItem::readFromStream(in) ;
	in >> description
	   >> data
	   >> offset
	   >> slope
	   >> factor
	   >> xshift
	   >> zeroMultiplications ;
}

void specDataItem::writeToStream(QDataStream & out) const
{
	specModelItem::writeToStream(out) ;
	out << description
	    << data
	    << offset
	    << slope
	    << factor
	    << xshift
	    << zeroMultiplications ;
}

specDataItem& specDataItem::operator+=(const specDataItem& toAdd)
{
	// merging descriptors
	foreach(QString key, toAdd.description.keys())
	{
		if (description.keys().contains(key))
		{
			if (description[key].isNumeric())
			{
				double total = data.size() + toAdd.data.size() ;
				description[key] = description[key].numericValue()*data.size()/total + toAdd.description[key].numericValue()*toAdd.data.size()/total ;
			}
			else if (!descriptor(key).contains(toAdd.descriptor(key)))
				description[key] = descriptor(key).append(", ").append(toAdd.descriptor(key)) ;
		}
		else
			description[key] = toAdd.description[key] ;
	}
	if (!descriptor("").contains(toAdd.descriptor("")) )
		changeDescriptor("",toAdd.descriptor("").prepend(descriptor("").isEmpty() ?"" : descriptor("").append(", ")) ) ;
	// merging actual data
	for (QVector<double>::size_type i = 0 ; i < toAdd.data.size() ; i++)
	{
		specDataPoint point = toAdd.data[i] ;
		toAdd.applyCorrection(point) ;
		reverseCorrection(point) ;
		data << point ;
	}
	return (*this) ;
}

bool compareDataPoints(const specDataPoint& a, const specDataPoint& b)
{
	return a.nu == b.nu ;
}

void specDataItem::flatten(bool oneTime)
{
	qSort(data) ;
	QVector<specDataPoint> newData ;
	averageToNew(data.begin(), data.end(), compareDataPoints, std::back_inserter(newData)) ;

    data.swap(newData) ;
	// average Time
	if(oneTime)
	{
		double time = 0 ;
		for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
			time += data[i].t ;
		time /= data.size() ;
		for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
			data[i].t = time ;
	}
	invalidate() ;
}

spec::descriptorFlags specDataItem::descriptorProperties(const QString& key) const
{
	if (description.contains(key)) return description[key].flags() ;
	return specModelItem::descriptorProperties(key) ;
}

void specDataItem::setDescriptorProperties(const QString &key, spec::descriptorFlags f)
{
    if (description.contains(key)) description[key].setFlags(f) ;
    else specModelItem::setDescriptorProperties(key, f) ;
}


void specDataItem::scaleBy(const double& mul)
{
    qDebug() << "Scaling by" << mul ;
    if (0 == mul)
        zeroMultiplications ++ ;
    else if (INFINITY == mul)
        zeroMultiplications = 0 ;
    else if (isnan(mul)) return ;
    else
    {
        slope  *= mul ;
        factor *= mul ;
        offset *= mul ;
    }
	invalidate();
}

void specDataItem::addToSlope(const double& off)
{
	slope += off ;
	invalidate();
}

void specDataItem::moveYBy(const double& off)
{
	offset += off ;
	invalidate();
}

void specDataItem::moveXBy(const double & value)
{
	xshift += value ;
	invalidate();
}

void specDataItem::exportData(const QList<QPair<bool,QString> >& headerFormat, const QList<QPair<spec::value,QString> >& dataFormat, QTextStream& out) // TODO split into two
{
	revalidate();
	QVector<double> t = times(), w = wnums(), s = ints(), m = intensityData() ;
	
	for (int i = 0 ; i < headerFormat.size() ; i++)
		out << (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	out << endl ;
	for (size_t j = 0 ; j < dataSize() ; j++)
	{
		for (int i = 0 ; i < dataFormat.size() ; i++)
		{
			switch(dataFormat[i].first)
			{
				case spec::time: out << t[j] ; break ;
				case spec::wavenumber: out << w[j] ; break ;
				case spec::signal: out << s[j] ; break ;
				case spec::maxInt: out << m[j] ; break ;
			}
			out << dataFormat[i].second ;
		}
	}
	out << endl ;
}

int specDataItem::removeData(QList<specRange *> *listpointer)
{
	QVector<specDataPoint> newData ;
	for (int i = 0 ; i < data.size() ; ++i)
	{
		specDataPoint point = data[i] ;
		applyCorrection(point) ;
		foreach(specRange* range, *listpointer)
			if (range->contains(point.nu))
				continue ;
		newData << data[i] ;
	}
	int diff = data.size() - newData.size() ;
    data.swap(newData) ;
	invalidate() ;
	return diff ;
}

QVector<specDataPoint> specDataItem::getDataExcept(const QList<specRange*>& ranges)
{
	QVector<specDataPoint> newData ;
	for (int i = 0 ; i < data.size() ; ++i)
	{
		specDataPoint point = data[i] ;
		applyCorrection(point) ;
		bool include = true ;
		foreach(specRange* range, ranges)
		{
			if (range->contains(point.nu))
			{
				include = false ;
				break ;
			}
		}
		if (include)
			newData << data[i] ;
	}
	return newData ;
}

void specDataItem::applyCorrection(QVector<specDataPoint> &newData) const
{
	for (int i = 0 ; i < newData.size() ; ++i)
		applyCorrection(newData[i]) ;
}

void specDataItem::reverseCorrection(QVector<specDataPoint> &newData) const
{
	for (int i = 0 ; i < newData.size() ; ++i)
		reverseCorrection(newData[i]) ;
}

void specDataItem::setData(const QVector<specDataPoint> &newData)
{
	data = newData ;
	qSort(data) ;
	invalidate();
}

specDataItem::specDataItem(const specDataItem &other)
	: specModelItem(0,other.descriptor("",true)),
	  offset(other.offset),
	  slope(other.slope),
	  factor(other.factor),
	  xshift(other.xshift),
	  description(other.description),
      data(other.data),
      zeroMultiplications(other.zeroMultiplications)
{
}

specUndoCommand *specDataItem::itemPropertiesAction(QObject *parentObject)
{
	dataItemProperties propertiesDialog(this) ;
	propertiesDialog.exec() ;
	if (propertiesDialog.result() != QDialog::Accepted) return 0 ;
	return propertiesDialog.changeCommands(parentObject) ;
}

void specDataItem::attach(QwtPlot *pl)
{
	specModelItem::attach(pl) ;
	specPlot *p = qobject_cast<specPlot*>(plot()) ;
	if (p)
		p->attachToPicker(this) ;
}

void specDataItem::detach()
{
	specPlot *p = qobject_cast<specPlot*>(plot()) ;
	if (p)
		p->detachFromPicker(this) ;
	specModelItem::detach() ;
}
