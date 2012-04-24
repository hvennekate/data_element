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

bool specModelItem::setActiveLine(const QString&, int)
{
	return false ;
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

QString specModelItem::descriptor(const QString &key, bool full) const
{
	if (key == "") return QwtPlotCurve::title().text() ;
	return QString() ;
}

bool specModelItem::isFolder() const { return false ;}

QIcon specModelItem::indicator(const QString& Descriptor) const
{
	if (Descriptor == "") return decoration() ;
	if (descriptor(Descriptor,true).contains(QRegExp("\n")))
		return QIcon::fromTheme("go-down") ;
	return QIcon() ;
}

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
	qDebug()<< "merge from copy:" << pointer->mergePlotData ;
	pointer->invalidate();
	return stream ;
}

QDataStream& specModelItem::writeOut(QDataStream& stream) const
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "called operator<<" << endl ;
	return (writeToStream(stream)
			<< mergePlotData << sortPlotData << QwtPlotCurve::title().text() << pen()) ;
	qDebug()<< "merge from original:" << mergePlotData ;
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

QVector<double> specModelItem::intensityData()
{
	return QVector<double>() ;
}

bool specModelItem::connectClient(specModelItem *clnt)
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

bool specModelItem::disconnectClient(specModelItem *clnt) // TODO template
{
	if (!clients.contains(clnt))
		return false ;
	if (!clnt->disconnectServer(this))
		return false ;
	clients.removeOne(clnt) ;
	return true ;
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
}

bool specModelItem::shortCircuit(specMetaItem *server)
{
	if (clients.contains(server))
		return true ;
	bool sc = false ;
	foreach(specMetaItem* client, clients)
		sc = sc || client->shortCircuit(server) ;
	return sc ;
}

bool specModelItem::connectServer(specModelItem *itm)
{
	return false ;
}

bool specModelItem::disconnectServer(specModelItem *itm)
{
	return false ;
}
