#include "specmodelitem.h"
#include "specfolderitem.h"
#include "specdataitem.h"
#include "speclogentryitem.h"
#include "speclogmessage.h"
#include "speckinetic.h"
#include <QTextStream>
#include "speckineticrange.h"
#include <QTime>
#include <QDebug>
#include "kinetic/specmetaitem.h"
#include "utility-functions.h"
#include <QPainter>
#include "plot/specplotstyle.h"


specModelItem::specModelItem(specFolderItem* par, QString description)
	: specCanvasItem(description),
	  iparent(0),
	  mergePlotData(true),
	  sortPlotData(true),
	  description("",spec::editable)
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
	if (par == iparent)
		return ;
	if (iparent)
		iparent->removeChild(this) ;
	if (!par) // TODO review and maybe find better place
		detachChild(this) ;
	iparent = par ;
}

void specModelItem::detachChild(specModelItem* child) // TODO review and maybe find better place
{
	child->detach();
	for (int i = 0 ; i < child->children() ; ++i)
		detachChild	(((specFolderItem*) child)->child(i)) ;
}

specFolderItem* specModelItem::parent() const
{ return iparent ; }

QList<specModelItem*>::size_type specModelItem::children() const
{ return 0 ; }

bool specModelItem::isEditable(QString key) const
{
	if (key == "")
		return true ;
	return descriptorProperties(key) & spec::editable ;
}

bool specModelItem::changeDescriptor(QString key, QString value)
{
	if (key == "" && description.isEditable())
	{
		description.setContent(value) ;
		return true ;
	}
	
	return false ;
}

bool specModelItem::setActiveLine(const QString& key, int line)
{
	if (key == "")
		return description.setActiveLine(line) ;
	return false ;
}

int specModelItem::activeLine(const QString& key) const
{
	if (key == "")
		return description.activeLine() ;
	return -1 ;
}

void specModelItem::refreshPlotData()
{ }

void specModelItem::processData(QVector<double> &x, QVector<double> &y) const
{
	QTime timer ;
	timer.start() ;
	if (sortPlotData)
	{
		QMultiMap<double,double> sortedValues;
		for(int i = 0 ; i < x.size() ; i++)
			sortedValues.insert(x[i],y[i]) ;
		x = sortedValues.keys().toVector() ;
		y = sortedValues.values().toVector() ;
	}
	QVector<double> xt, yt ;

	if (mergePlotData)
	{
		for (int i = 0 ; i < x.size() ; i)
		{
			int j = i ;
			double ysum = 0, xtemplate = x[i] ;
			while (i < x.size() && x[i] == xtemplate)
				ysum += y[i++] ;
			xt << xtemplate ;
			yt << ysum/(i-j) ;
		}
		x = xt ;// TODO simply swap the vectors!!!
		y = yt ;
	}
}

QwtSeriesData<QPointF>* specModelItem::processData(QwtSeriesData<QPointF>* dat) const
{
	if (!sortPlotData && !mergePlotData)
		return dat ;
	QVector<QPointF> newData(dat->size()) ;
	for(int i = 0 ; i < dat->size() ; ++i)
		newData[i] = dat->sample(i) ; // TODO quicker!
	if (sortPlotData)
		qSort(newData.begin(), newData.end(), comparePoints) ;

	QwtSeriesData<QPointF>* retVal = dat ;
	typedef QVector<QPointF>::const_iterator vecit ;
	if (mergePlotData)
	{
		QVector<QPointF> realData ;
		for(vecit i = newData.begin() ; i != newData.end() ; ++i)
		{
			double x = i->x(), y = i->y() ;
			vecit j ;
			for (j = i+1 ; j != newData.end() && j->x() == x; ++j)
				y += j->y() ;
			realData << QPointF(x,y/(j-i)) ; // TODO: Unit test!
		}
		retVal = new QwtPointSeriesData(realData) ;
	}
	else
		retVal = new QwtPointSeriesData(newData);
	delete dat ;
	return retVal ;
}

QString specModelItem::descriptor(const QString &key, bool full) const
{
	if (key == "") return description.content(full) ;
	return QString() ;
}

bool specModelItem::isFolder() const { return false ;}

QIcon specModelItem::indicator(const QString& Descriptor) const
{
	if (descriptor(Descriptor,true).contains(QRegExp("\n")))
	{
		if (Descriptor == "")
		{
			QIcon def = decoration(), arrow = QIcon::fromTheme("go-down") ;
			const QSize size = def.availableSizes().first() ; // TODO unsafe!
			const QSize small = size/2. ;
			QPixmap map = def.pixmap(size) ;
			QPainter painter(&map) ;
			painter.drawPixmap(small.width(), small.height(), small.width(), small.height(), arrow.pixmap(small)) ;
			return QIcon(map) ;
		}
		else return QIcon::fromTheme("go-down") ;
	}
	if (Descriptor == "")
		return decoration() ;
	return QIcon() ;
}

QIcon specModelItem::decoration() const { return QIcon() ; }

bool specModelItem::addChild(specModelItem *child, QList<specModelItem*>::size_type position)
{ return false ; }

bool specModelItem::addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position)
{ return false ; }

QStringList specModelItem::descriptorKeys() const
{ return QStringList(QString("")) ;}

void specModelItem::writeToStream(QDataStream &out) const
{
	out << mergePlotData << sortPlotData ;
}

void specModelItem::writeContents(QDataStream &out) const
{
	out << description << specPlotStyle(this) ;
}

void specModelItem::readFromStream(QDataStream & in)
{
	in >> mergePlotData >> sortPlotData ;
}

void specModelItem::readContents(QDataStream &in)
{
	specPlotStyle style ;
	in >> description >> style ;
	style.apply(this);
}

specModelItem *specModelItem::produce(specInStream &in)
{
	QVector<specStream::Type> types ;
	types << ;
	if (!in.expect(types)) return  0 ;
	specModelItem *item = 0 ;
	if (in.type() == ...) item = new ... ;
	if (!item->read(in))
	{
		delete item ;
		return 0 ;
	}
	return item ;
}

bool specModelItem::read(specInStream& in)
{
	if (in.type() != id()) return false ;
	readInternals(in) ;
	return true ; // TODO maybe consider error in readInternals...
}

void specModelItem::write(specOutStream& out) const
{
	out.next(id()) ;
	writeInternals(out) ;
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

QVector<double> specModelItem::intensityData() const
{
	return QVector<double>() ;
}

bool specModelItem::connectClient(specMetaItem *clnt)
{
	if (clients.contains(clnt))
		return false ;
	if (!clnt->connectServer(this))
		return false ;
	qDebug() << "connecting client" << clnt << this ;
	clients << clnt ;
	qDebug() << "done connecting client" ;
	return true ;
}

bool specModelItem::disconnectClient(specMetaItem *clnt) // TODO template
{
	if (!clients.contains(clnt))
		return false ;
	if (!clnt->disconnectServer(this))
		return false ;
	return clients.remove(clnt) ;
}

void specModelItem::invalidate()
{
	dataValid = false ;
	foreach(specMetaItem *client, clients) // TODO consider tying to the condition that item has been valid before
		client->invalidate();
}

void specModelItem::revalidate()
{
	if (dataValid) return ;
	refreshPlotData();
	dataValid = true ;
}

bool specModelItem::shortCircuit(specModelItem *server)
{
	if (server == this) return true ;
	foreach(specMetaItem* client, clients)
		if (client->shortCircuit(server)) return true ;
	return false;
}

bool specModelItem::connectServer(specModelItem *itm)
{
	return false ;
}

bool specModelItem::disconnectServer(specModelItem *itm)
{
	return false ;
}
