#include "specmodelitem.h"
#include "specfolderitem.h"
#include "specdataitem.h"
#include "speclogentryitem.h"
#include "speclogmessage.h"
#include "speckinetic.h"
#include <QTextStream>
#include "speckineticrange.h"

specModelItem::specModelItem(specFolderItem* par, QString description)
	: specCanvasItem(description), iparent(0), mergePlotData(true), sortPlotData(true)
{
	setParent(par) ;
}

specModelItem::~specModelItem()
{
	setParent(0) ;
}
	
QMultiMap<double,QPair<double,double> >* specModelItem::kinetics(QList<specKineticRange*>) const
{ return new QMultiMap<double,QPair<double,double> > ; }
	
void specModelItem::setParent(specFolderItem* par)
{
	if (iparent) iparent->removeChild(this) ;
	iparent = par ;
}

specFolderItem* specModelItem::parent() const
{ return iparent ; }

QList<specModelItem*>::size_type specModelItem::children() const
{ return 0 ; }

bool specModelItem::isEditable(QString key) const
{
	if (key == "")
		return true ;
	return false ;
}

bool specModelItem::changeDescriptor(QString key, QString value)
{
	if (key == "")
	{
		QwtPlotCurve::setTitle(value) ;
		return true ;
	}
	
	return false ;
}

void specModelItem::refreshPlotData()
{ }

void specModelItem::processData(QwtArray<double> &x, QwtArray<double> &y) const
{
	if (sortPlotData)
	{
		QMultiMap<double,double> sortedValues;
		for(int i = 0 ; i < x.size() ; i++)
			sortedValues.insert(x[i],y[i]) ;
		x = sortedValues.keys().toVector() ;
		y = sortedValues.values().toVector() ;
	}
	if (mergePlotData)
	{
		for (int i = 0 ; i < x.size() ; i++)
		{
			int j ;
			for (j = 1 ; i+1 < x.size() && x[i+1] == x[i]; j++)
			{
				y[i] += y[i+1] ;
				x.remove(i+1) ;
				y.remove(i+1) ;
			}
			y[i] /= j ;
		}
	}
}

QString specModelItem::descriptor(const QString &key) const
{
	if (key == "") return QwtPlotCurve::title().text() ;
	return QString() ;
}

bool specModelItem::isFolder() const { return false ;}

QIcon specModelItem::decoration() const { return QIcon() ; }

bool specModelItem::addChild(specModelItem *child, QList<specModelItem*>::size_type position)
{ return false ; }

bool specModelItem::addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position)
{ return false ; }

QStringList specModelItem::descriptorKeys() const
{ return QStringList(QString("")) ;}

QDataStream& operator>>(QDataStream& stream, specModelItem*& pointer)
{
	quint8 id ;// TODO read/write plot properties etc.
	stream >> id ;
	switch (id)
	{
		case spec::data :
			pointer = new specDataItem(QList<specDataPoint>(),
					QHash<QString,specDescriptor>()) ; break ;
		case spec::folder :
			pointer = new specFolderItem ; break ;
		case spec::logEntry :
			pointer = new specLogEntryItem(QHash<QString,specDescriptor>()) ; break ;
		case spec::sysEntry :
			pointer = new specLogMessage(QHash<QString,specDescriptor>()) ; break ;
		case spec::kinetic :
			pointer = new specKinetic ; break ;
	}
	QString title ;
	QPen pen ;
	pointer->readFromStream(stream)
			>> pointer->mergePlotData >> pointer->sortPlotData >> title >> pen ;
	pointer->QwtPlotCurve::setTitle(title) ;
	pointer->setPen(pen) ;
	return stream ;
}

QDataStream& specModelItem::writeOut(QDataStream& stream) const
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "called operator<<" << endl ;
	return (writeToStream(stream)
			<< mergePlotData << sortPlotData << QwtPlotCurve::title().text() << pen()) ;
}

spec::descriptorFlags specModelItem::descriptorProperties(const QString& key) const
{
	if(key=="") return spec::editable ;
	return spec::def ;
}

void specModelItem::exportData(const QList<QPair<bool,QString> >& headerFormat, const QList<QPair<spec::value,QString> >& dataFormat, QTextStream& out)
{
	for (int i = 0 ; i < headerFormat.size() ; i++)
		out << (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	out << endl ;
	for (int j = 0 ; j < dataSize() ; j++)
		for (int i = 0 ; i < dataFormat.size() ; i++)
			out << dataFormat[i].first << dataFormat[i].second ;
	out << endl ;
}

QwtArray<double> specModelItem::intensityData()
{
	return QwtArray<double>() ;
}
