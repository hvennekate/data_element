#include "specmodelitem.h"
#include "specfolderitem.h"
#include "specdataitem.h"
#include "speclogentryitem.h"
#include "speclogmessage.h"
#include <QTextStream>
#include <QTime>
#include "kinetic/specmetaitem.h"
#include "utility-functions.h"
#include <QPainter>
#include "plot/specplotstyle.h"
#include "specsvgitem.h"


specModelItem::specModelItem(specFolderItem* par, QString desc)
	: specCanvasItem(),
	  iparent(0),
	  dataValid(false),
	  description(desc),
	  mergePlotData(true),
	  sortPlotData(true)
{
	setParent(par) ;
	setItemAttribute(Legend, true) ;
	setLegendAttribute(LegendNoAttribute, false) ;
	setLegendAttribute(LegendShowLine, true) ;
	setLegendAttribute(LegendShowSymbol, true) ;
}

specModelItem::~specModelItem()
{
	detach();
	foreach(specMetaItem * client, clients)
	disconnectClient(client) ;
	setParent(0) ;
}

void specModelItem::setParent(specFolderItem* par)
{
	//	if (par == iparent)
	//		return ;
	if(iparent)
		iparent->removeChild(this) ;
	if(!par)  // TODO review and maybe find better place
		detachChild(this) ;
	iparent = par ;
}

void specModelItem::detachChild(specModelItem* child)  // TODO review and maybe find better place
{
	child->detach();
	for(int i = 0 ; i < child->children() ; ++i)
		detachChild(((specFolderItem*) child)->child(i)) ;
}

specFolderItem* specModelItem::parent() const
{ return iparent ; }

QList<specModelItem*>::size_type specModelItem::children() const
{ return 0 ; }

void specModelItem::changeDescriptor(const QString &key, QString value)
{
	if(key == "")
	{
		description.setAppropriateContent(value) ;
		setTitle(value) ;
	}
}

void specModelItem::setActiveLine(const QString& key, quint32 line)
{
	if(key == "")
	{
		description.setActiveLine(line) ;
	}
}

quint32 specModelItem::activeLine(const QString& key) const
{
	return getDescriptor(key).activeLine() ;
}

void specModelItem::refreshPlotData()
{ }

template<typename T>
QVector<T> specModelItem::findDescendants()
{
	QVector<T> result ;
	T self = dynamic_cast<T>(this) ;
	if(self) result << self ;
	return result ;
}

void specModelItem::processData()
{
	if(!sortPlotData && !mergePlotData)
		return ;
	// Obtain data points
	QVector<QPointF> newData(dataSize()) ;
	for(size_t i = 0 ; i < dataSize() ; ++i)
		newData[i] = sample(i) ;  // TODO quicker!
	if(sortPlotData)
		qSort(newData.begin(), newData.end(), comparePoints) ;

	typedef QVector<QPointF>::const_iterator vecit ;
	if(mergePlotData)
	{
		QVector<QPointF> realData ;
		for(vecit i = newData.begin() ; i != newData.end() ; ++i)
		{
			double x = i->x(), y = i->y() ;
			vecit j ;
			for(j = i + 1 ; j != newData.end() && j->x() == x; ++j)
				y += j->y() ;
			realData << QPointF(x, y / (j - i)) ;  // TODO: Unit test!
		}
		setData(new QwtPointSeriesData(realData)) ;
	}
	else
		setData(new QwtPointSeriesData(newData)) ;

}

QString specModelItem::descriptor(const QString& key, bool full) const
{
	return getDescriptor(key).content(full) ;
}

double specModelItem::descriptorValue(const QString &key) const
{
	return getDescriptor(key).numericValue() ;
}

bool specModelItem::isFolder() const { return false ;}

QIcon specModelItem::indicator(const QString& Descriptor) const
{
	if(descriptor(Descriptor, true).contains(QRegExp("\n")))
	{
		if(Descriptor == "")
		{
			QIcon def = decoration(), arrow = QIcon::fromTheme("go-down") ;
			const QSize size = def.availableSizes().first() ; // TODO unsafe!
			const QSize small = size / 2. ;
			QPixmap map = def.pixmap(size) ;
			QPainter painter(&map) ;
			painter.drawPixmap(small.width(), small.height(), small.width(), small.height(), arrow.pixmap(small)) ;
			return QIcon(map) ;
		}
		else return QIcon::fromTheme("go-down") ;
	}
	if(Descriptor == "")
		return decoration() ;
	return QIcon() ;
}

QIcon specModelItem::decoration() const { return QIcon() ; }

bool specModelItem::addChild(specModelItem* child, QList<specModelItem*>::size_type position)
{
	Q_UNUSED(child) ;
	Q_UNUSED(position) ;
	return false ;
}

bool specModelItem::addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position)
{
	Q_UNUSED(list)
	Q_UNUSED(position)
	return false ;
}

QStringList specModelItem::descriptorKeys() const
{ return QStringList(QString("")) ;}

bool specModelItem::hasDescriptor(const QString& d) const
{
	return descriptorKeys().contains(d) ;
}

void specModelItem::setMultiline(const QString &key, bool on)
{
	if (key == "")
		description.setMultiline(on) ;
}

bool specModelItem::isMultiline(const QString &key) const
{
	return getDescriptor(key).isMultiline() ;
}

void specModelItem::writeToStream(QDataStream& out) const
{
	specCanvasItem::writeToStream(out) ;
	out << mergePlotData << sortPlotData
	    << description ;
}

void specModelItem::readFromStream(QDataStream& in)
{
	specCanvasItem::readFromStream(in) ;
	in >> mergePlotData >> sortPlotData
	   >> description ;
	setTitle(descriptor("", true));
	invalidate() ;
}

QString specModelItem::exportData(const QList<QPair<bool, QString> >& headerFormat, const QList<QPair<int, QString> >& dataFormat, const QStringList& numericDescriptors)
{
	revalidate();
	QString descriptorString ;
	for(int i = 0 ; i < headerFormat.size() ; i++)
		descriptorString += (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	if (!descriptorString.isEmpty())
		descriptorString += "\n" ;
	return descriptorString + exportCoreData(dataFormat, numericDescriptors) + "\n" ;
}

QString specModelItem::exportCoreData(const QList<QPair<int, QString> > & dataFormat, const QStringList &numericDescriptors) const
{
	QString result ;
	for(size_t j = 0 ; j < dataSize() ; j++)
	{
		for(int i = 0 ; i < dataFormat.size() ; i++)
		{
			switch (dataFormat[i].first)
			{
			case spec::wavenumber:
				result += exportX(j) ;
				break ;
			case spec::signal:
				result += exportY(j) ;
				break ;
			case spec::maxInt:
				result += exportZ(j) ;
				break ;
			default:
				result += descriptor(numericDescriptors[dataFormat[i].first - spec::numericDescriptor]) ;
			}
			result += dataFormat[i].second ;
		}
	}
	return result ;
}

QString specModelItem::exportX(int index) const
{
	return QString::number(sample(index).x()) ;
}

QString specModelItem::exportY(int index) const
{
	return QString::number(sample(index).y()) ;
}

QString specModelItem::exportZ(int index) const
{
	Q_UNUSED(index)
	return "NaN" ;
}


QVector<double> specModelItem::intensityData() const
{
	return QVector<double>() ;
}

bool specModelItem::connectClient(specMetaItem* clnt)
{
	if(clients.contains(clnt))
		return false ;
	if(!clnt->connectServer(this))
		return false ;
	clients << clnt ;
	return true ;
}

bool specModelItem::disconnectClient(specMetaItem* clnt)  // TODO template
{
	if(!clients.contains(clnt))
		return false ;
	if(!clnt->disconnectServer(this))
		return false ;
	return clients.remove(clnt) ;
}

void specModelItem::invalidate()
{
	dataValid = false ;
	foreach(specMetaItem * client, clients)  // TODO consider tying to the condition that item has been valid before
	client->invalidate();
	if(iparent) iparent->invalidate();
}

void specModelItem::revalidate()
{
	if(dataValid) return ;
	refreshPlotData();
	processData() ;
	dataValid = true ;
}

bool specModelItem::shortCircuit(specModelItem* server)
{
	if(server == this) return true ;
	foreach(specMetaItem * client, clients)
	if(client->shortCircuit(server)) return true ;
	return false;
}

bool specModelItem::connectServer(specModelItem* itm)
{
	Q_UNUSED(itm)
	return false ;
}

bool specModelItem::disconnectServer(specModelItem* itm)
{
	Q_UNUSED(itm)
	return false ;
}

specModelItem* specModelItem::itemFactory(specStreamable::type t)
{
	switch(t)
	{
		case specStreamable::dataItem : return new specDataItem ;
		case specStreamable::folder : return new specFolderItem ;
		case specStreamable::logEntry : return new specLogEntryItem ;
		case specStreamable::sysEntry : return new specLogMessage ;
		case specStreamable::svgItem : return new specSVGItem ;
		case specStreamable::metaItem : return new specMetaItem ;
		case specStreamable::legacyDataItem : return new specLegacyDataItem ;
		default: return 0 ;
	}
}

specModelItem* specModelItem::factory(const type& t) const
{
	return itemFactory(t) ;
}

specModelItem::descriptorComparison::descriptorComparison(const QStringList* d)
	: description(d)
{
}

bool specModelItem::descriptorComparison::operator()(specModelItem*& a, specModelItem*& b)
{
	if(!description) return false ;
	foreach(QString criterion, *description)
	{
		QString aDescriptor = a->descriptor(criterion, true),
			bDescriptor = b->descriptor(criterion, true) ;
		if(aDescriptor != bDescriptor)
			return aDescriptor < bDescriptor ;
	}
	return false ;
}

void specModelItem::attach(QwtPlot* plot)
{
	specCanvasItem::attach(plot) ;
	foreach(specMetaItem * client, clients)
		client->refreshOtherPlots();
}

void specModelItem::detach()
{
	specCanvasItem::detach() ;
	foreach(specMetaItem * client, clients)
		client->refreshOtherPlots();
}

QString specModelItem::toolTip(const QString& column) const
{
	return descriptor(column, true) ;
}

specModelItem::specModelItem(const specModelItem& other)
	: specCanvasItem(other.description.content()),  // TODO hm...
	  iparent(0),
	  dataValid(false),
	  description(other.description)
{}

void specModelItem::renameDescriptors(const QMap<QString, QString>& map)
{
	Q_UNUSED(map) ;
}

void specModelItem::deleteDescriptor(const QString& descriptors)
{
	Q_UNUSED(descriptors) ;
}

void specModelItem::dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const
{
	if(key == "")
		destination << description ;
	else
		destination << specDescriptor() ;
}

void specModelItem::restoreDescriptor(QListIterator<specDescriptor>& origin, const QString& key)
{
	if(!origin.hasNext()) return ;
	if(key == "")
		description = origin.next() ;
	else
		origin.next() ;
}

specDescriptor specModelItem::getDescriptor(const QString &key) const
{
	if (key == "") return description ;
	return specDescriptor() ;
}

QString specModelItem::editDescriptor(const QString& key) const
{
	return descriptor(key, true) ;
}

void specModelItem::initializeData()
{
	revalidate();
}

// Force instantiation
template QVector<specDataItem*> specModelItem::findDescendants() ;
