#include "specdataitem.h"
#include <QVariant>
#include <QTextStream>
#include "speckineticrange.h"
#include <QDebug>


void specDataFilter::applyCorrection(double &t, double &wn, double &in) const
{
	in = in*factor+offset+slope*wn ;
}

void specDataFilter::subMap(QList<specDataPoint> &data, const QMap<double, double> &toSub)
{
	for (int i = 0 ; i < data.size() ; i++)
	{
		double t= data[i][0], n = data[i][x], in = data[i][y] ;
		applyCorrection(t,n,in) ;
		if (toSub.contains(n))
			in -= toSub[n] ;
		reverseCorrection(t,n,in) ;
		data[i].sig = in ;
	}
}

void specDataFilter::reverseCorrection(double &t, double &wn, double &in) const
{
	in = (in-offset-slope*wn)/factor ;
}

bool specDataFilter::addData(QList<specDataPoint>& toBeAppended, const QVector<double>& times, const QVector<double>& wnums, const QVector<double>& ints, const QVector<double>& mints)
{
	QVector<double>::size_type size = qMax(times.size(),qMax(wnums.size(),qMax(ints.size(),mints.size()))) ;
	for (QVector<double>::size_type i = 0 ; i < size ; i++)
	{
		double t = times[i], w = wnums[i], j = ints[i], m = mints[i] ;
		reverseCorrection(t,w,j) ;
		toBeAppended << specDataPoint(t,w,j,m) ;
	}
	return true ;
}


//TODO naechste drei Funktionen vereinheitlichen...
QVariant specDataFilter::plotData(const QList<specDataPoint>& data) const
{
	QList<QVariant> xdata, ydata, alldata ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		double xVal = data[i][x], yVal = data[i][y], t = data[i][0] ;
		applyCorrection(t,xVal,yVal) ;
		if (xVal > xmin && xVal < xmax && yVal > ymin && yVal < ymax)
		{
			xdata += xVal ;
			ydata += yVal ;
		}
	}
	foreach (QVariant item, xdata) alldata << item ;
	foreach (QVariant item, ydata) alldata << item ;
	return alldata ;
}

QVector<double> specDataFilter::wnums(const QList<specDataPoint>& data) const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		double xVal = data[i][x], yVal = data[i][y], t = data[i][0] ;
		applyCorrection(t,xVal,yVal) ;
		if (xVal > xmin && xVal < xmax && yVal > ymin && yVal < ymax)
			retval += xVal ;
	}
	return retval ;
}

QVector<double> specDataFilter::mints(const QList<specDataPoint>& data) const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		double xVal = data[i][x], yVal = data[i][y], t = data[i][0] ;
		applyCorrection(t,xVal,yVal) ;
		if (xVal > xmin && xVal < xmax && yVal > ymin && yVal < ymax)
			retval += data[i].mint ;
	}
	return retval ;
}

QVector<double> specDataFilter::ints(const QList<specDataPoint>& data) const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		double xVal = data[i][x], yVal = data[i][y], t = data[i][0] ;
		applyCorrection(t,xVal,yVal) ;
		if (xVal > xmin && xVal < xmax && yVal > ymin && yVal < ymax)
			retval += yVal ;
	}
	return retval ;
}

QVector<double> specDataFilter::times(const QList<specDataPoint>& data) const
{
	QVector<double> retval ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		double xVal = data[i][x];
		double yVal = data[i][y]*factor+offset+slope*xVal ;
		if (xVal > xmin && xVal < xmax && yVal > ymin && yVal < ymax)
			retval += data[i].t ;
	}
	return retval ;
}



specDataItem::specDataItem(QList<specDataPoint> dat, QHash<QString,specDescriptor> desc, specFolderItem* par, QString description)
	:specModelItem(par,description), data(dat), description(desc), addedItems(1)
{
}

specDataItem::~specDataItem()
{
	data.clear() ;
	description.clear() ;
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
	if (key == "")
		return specModelItem::setActiveLine(key,line) ;
	if (description.contains(key))
		return description[key].setActiveLine(line) ;

	return false ;
}

void specDataItem::refreshPlotData()
{
	qDebug() << "wnumsplot: " << wnums() << ints() ;
	QVector<double> x=filter.wnums(data), y=filter.ints(data);
	qDebug() << "collected data:" << x ;
	processData(x,y) ;
	qDebug() << "processed:" << x << "merge:" << mergePlotData ;
	setSamples(x,y) ;
}

QString specDataItem::descriptor(const QString &key, bool full) const
{
	if (key == "") return specModelItem::descriptor(key, full) ;
	
	if (description.contains(key))
		return description[key].content(full) ;
	
	return "" ;
}

QIcon specDataItem::decoration() const { return QIcon(":/data.png") ; }

QStringList specDataItem::descriptorKeys() const
{
	return (specModelItem::descriptorKeys() << description.keys()) ;
}

QVector<double> specDataItem::wnums() const { return filter.wnums(data) ; }
QVector<double> specDataItem::ints() const { return filter.ints(data) ; }
QVector<double> specDataItem::mints() const { return filter.mints(data) ; }
QVector<double> specDataItem::times() const { return filter.times(data) ; }

QDataStream& specDataItem::readFromStream(QDataStream& stream)
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "Reading data item " << endl ;
	QVector<specDataPoint>::size_type dataSize ;
	QHash<QString,specDescriptor>::size_type descriptionSize ;
	stream >> dataSize >> descriptionSize ;
	// Lese Filterdaten
	stream >> filter.x >> filter.y >> filter.xmin >> filter.xmax >> filter.ymin >> filter.ymax >> filter.offset >> filter.slope >> filter.factor ;
	
	for (QVector<specDataPoint>::size_type i = 0 ; i < dataSize ; i++)
	{
// 		cout << "reading data point " << i << " of " << dataSize << endl ;
		data += specDataPoint() ;
		stream >> data.last() ;
	}
	for (QHash<QString,specDescriptor>::size_type i=0 ; i < descriptionSize ; i++)
	{
// 		cout << "reading descriptor " << i << " of " << descriptionSize << endl ;
		QString key;
		stream >> key ;
		qDebug("reading descriptor: \"%s\"",key.data()) ;
		stream >> description[key] ;
	}
//	refreshPlotData() ;
// 	cout << "done reading data item." << endl ;
	qDebug("///////////// from stream is editable via this: %d", this->isEditable(""));
	qDebug() << "////////// wnums: " << wnums() << ints() ;
	return stream;
}

QDataStream& specDataItem::writeToStream(QDataStream& stream) const
{
// 	qDebug("writing data item") ;
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "ID export:  " << (quint8) spec::data << endl ;
	stream << (quint8) spec::data << data.size() << description.size() ; 
	// Filterdaten ausgeben:
	stream << filter.x << filter.y << filter.xmin << filter.xmax << filter.ymin << filter.ymax << filter.offset << filter.slope << filter.factor ;
	
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
		stream << data[i] ;
	for (QHash<QString,specDescriptor>::size_type i = 0 ; i < description.size() ; i++)
	{
		qDebug("writing descriptor \"%s\"",description.keys()[i].data()) ;
		stream << (description.keys()[i]) << (description[description.keys()[i]]) ;
	}
	return stream ;
}

specDataItem& specDataItem::operator+=(const specDataItem& toAdd)
{// TODO : merge descriptors
	qDebug("Descriptor is editable before: %d",this->isEditable("")) ;
	qDebug("Descriptor is editable other : %d",toAdd.isEditable("")) ;
	filter.addData(data, toAdd.times(),toAdd.wnums(),toAdd.ints(),toAdd.mints()) ;
	qDebug("merging descriptors") ;
	for(QStringList::size_type i = 0 ; i < toAdd.description.keys().size() ; i++)
	{
		QString key = toAdd.description.keys()[i] ;
		if (description.keys().contains(key))
		{
			if (description[key].isNumeric())
				description[key] = description[key].numericValue() + toAdd.description[key].numericValue() ;
			else if (!descriptor(key).contains(toAdd.descriptor(key)))
				description[key] = descriptor(key).append(", ").append(toAdd.descriptor(key)) ;
		}
		else
			description[key] = toAdd.description[key] ;
	}
	if (!descriptor("").contains(toAdd.descriptor("")) )
		changeDescriptor("",toAdd.descriptor("").prepend(descriptor("").isEmpty() ?"" : descriptor("").append(", ")) ) ;
	addedItems ++ ;
	return (*this) ;
}

void specDataItem::subMap(const QMap<double, double>& map)
{
	filter.subMap(data, map) ;
	refreshPlotData() ;
}

void specDataItem::flatten(bool timeAverage, bool oneTime)
{
	qSort(data) ;
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
	{
		specDataPoint toCompare = data[i] ;
		QVector<specDataPoint>::size_type number = 1 ;
		for (QVector<specDataPoint>::size_type j = i+1 ; j < data.size() ; j++)
		{
			if((timeAverage && data[j].nu == toCompare.nu) || data[j] == toCompare)
			{
				data[i] += data.takeAt(j--) ;
				number++ ;
			}
			else break ; // should be sorted here...
		}
		data[i] /= number ;
	}
	if(oneTime)
	{
		double time = 0 ;
		for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
			time += data[i].t ;
		time /= data.size() ;
		for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
			data[i].t = time ;
	}
	foreach(QString descriptorLabel, descriptorKeys())
		if (descriptorProperties(descriptorLabel) & spec::numeric)
			description[descriptorLabel] = description[descriptorLabel].numericValue() /(double) addedItems ;
	addedItems = 1 ;
	refreshPlotData() ;
}

spec::descriptorFlags specDataItem::descriptorProperties(const QString& key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
	if (!description.contains(key)) return spec::def ;
	return description[key].flags() ;
}

QMultiMap<double,QPair<double,double> >* specDataItem::kinetics(QList<specKineticRange*> ranges) const
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly);
	QMultiMap<double,QPair<double,double> > *kinetic = new QMultiMap<double,QPair<double,double> > ;
	QVector<double> nu = wnums(), t = times(), sig = ints() ;
	foreach (specKineticRange* range, ranges)
	{
		for (int i = 0 ; i < nu.size() ; i++)
		{
// 			cout << "nu:  " << nu[i] << "  interval:  " << range->minValue() << "  " << range->maxValue() << endl ;
			if (range->contains(nu[i])) // TODO omit conversion
			{
				kinetic->insert(t[i],QPair<double,double>(nu[i],sig[i])) ;
			}
		}
	}
	
// 	cout << "returning list with size:  " << kinetic->size() << endl ;
	return  kinetic; // TODO implementieren
}

void specDataItem::scaleBy(const double& mul)
{
	filter.slope  *= mul ;
	filter.factor *= mul ;
	filter.offset *= mul ;
}

void specDataItem::addToSlope(const double& off)
{ filter.slope += off ; }

void specDataItem::moveYBy(const double& off)
{ filter.offset += off ;}

void specDataItem::moveXBy(const double & value)
{
	filter.addX(data,value) ;
}

void specDataFilter::addX(QList<specDataPoint> &data, const double &value)
{
	for (QVector<specDataPoint>::size_type i = 0 ; i < data.size() ; i++)
		data[i].nu += value ;
}

void specDataItem::exportData(const QList<QPair<bool,QString> >& headerFormat, const QList<QPair<spec::value,QString> >& dataFormat, QTextStream& out)
{
	QVector<double> t = times(), w = wnums(), s = ints(), m = mints() ;
	
	for (int i = 0 ; i < headerFormat.size() ; i++)
		out << (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	out << endl ;
	for (int j = 0 ; j < dataSize() ; j++)
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

QVector<double> specDataItem::intensityData()
{
	return mints() ;
}

int specDataItem::removeData(QList<specRange *> *listpointer)
{
	QVector<double> wns = wnums() ;
	int count = wns.size() ;
	qDebug("data size before deletion: %d",data.size()) ;
	for (int j = 0 ; j < listpointer->size() ; j++)
	{
		specRange *range = listpointer->at(j) ;
		for(int i = data.size()-1 ; i >= 0 ; i--)
		{
			if(range->contains(wns[i]))
			{
				wns.remove(i) ;
				data.removeAt(i) ;
			}
		}
	}
	refreshPlotData() ;
	qDebug("data size after deletion: %d",data.size()) ;
	return count-wns.size() ;
}

void specDataItem::average(int num)
{
	QVector<double> times = filter.times(data), nus = filter.wnums(data), sigs = filter.ints(data), mints = filter.mints(data);
	data.clear();
	for (int i = 0 ; i+num < times.size() ; i += num)
	{
		double time = 0, nu = 0, sig = 0, mint = 0 ;
		for (int j = i  ; j < i+num ; j++)
		{
			time += times[j] ;
			nu   += nus  [j] ;
			sig  += sigs [j] ;
			mint = qMax(mint,mints[j]) ;
		}
		data << specDataPoint(time/(double) num, nu/(double) num, sig/(double) num, mint) ;
	}
	filter.offset = 0. ;
	filter.slope  = 0. ;
	filter.factor = 1.0;
	refreshPlotData() ;
}

void specDataItem::movingAverage(int num)
{
	QVector<double> times = filter.times(data), nus = filter.wnums(data), sigs = filter.ints(data), mints = filter.mints(data);
	data.clear();
	qDebug() << times << nus << sigs << mints ;
	for (int i = num ; i < times.size() - num ; i++)
	{
		double time = 0, nu = 0, sig = 0, mint = 0 ;
		for (int j = i-num ; j < i+num+1 ; j++)
		{
			time += times[j] ;
			nu   += nus  [j] ;
			sig  += sigs [j] ;
			mint = qMax(mint,mints[j]) ;
			qDebug() << "Addiere Werte:" << j << times[j] << nus[j] << sigs[j] ;
		}
		double div = 2.*num+1 ;
		qDebug() << "erste Werte:" << time << nu << sig << mint << num << div ;
		data << specDataPoint(time/div, nu/div, sig/div, mint) ;
	}
	filter.offset = 0. ;
	filter.slope  = 0. ;
	filter.factor = 1.0;
	refreshPlotData() ;
}
