#include "specfolderitem.h"
#include <QTextStream>
#include <QTime>

specFolderItem::specFolderItem(specFolderItem* par, QString description)
	: specModelItem(par, description)
{
}

specFolderItem::~specFolderItem()
{
	foreach(specModelItem* childPointer, ChildrenList)
		delete childPointer ;
	ChildrenList.clear() ;
}

bool specFolderItem::addChild(specModelItem* item, QList<specModelItem*>::size_type position)
{
	addChildren(QList<specModelItem*>() << item, position) ;
	return true ;
}

bool specFolderItem::addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position)
{
	list.removeAll(0) ;
	for(int i = 0 ; i < list.size() ; i++)
	{
		list[i]->setParent(this) ;
		ChildrenList.insert(position+i, list[i]) ;
	}
	invalidate();
	return true ;
}

QList<specModelItem*>::size_type specFolderItem::children() const
{
	return ChildrenList.size() ;
}

bool specFolderItem::isFolder() const { return true ;}

QIcon specFolderItem::decoration() const { return QIcon(":/folder.png") ; }

bool specFolderItem::isEditable(QString key) const
{
	if (key =="")
		return specModelItem::isEditable(key) ;

	foreach(specModelItem* item, ChildrenList)
		if(item->isEditable(key))
			return true ;

	return false ;
}

void specFolderItem::refreshPlotData()
{
	QVector<double> x, y ;
	foreach(specModelItem* item, ChildrenList)
	{
		item->revalidate();
		for (size_t i = 0 ; i < item->dataSize() ; i++)
		{
			x << item->sample(i).x() ;
			y << item->sample(i).y() ;
		}
	}
	processData(x,y) ;
	setSamples(x,y) ;
}

void specFolderItem::removeChild(specModelItem* child)
{
	ChildrenList.removeOne(child) ; // Maybe all
	invalidate();
}

void specFolderItem::readFromStream(QDataStream& in)
{
	foreach(specModelItem* child, ChildrenList)
		delete child ;
	quint64 numChildren ;
	specModelItem::readFromStream(in) ;
	in >> numChildren ;
	QList<specModelItem*> newChildren ;
	for (quint64 i = 0 ; i < numChildren ; ++i)
		newChildren << (specModelItem*) produceItem(in) ; // TODO remove all zeros
	newChildren.removeAll(0) ;
	addChildren(newChildren) ;
}

void specFolderItem::writeToStream(QDataStream& out) const
{
	specModelItem::writeToStream(out) ;
	out << (quint64) children() ;
	foreach(specModelItem* child, ChildrenList)
		out << *child ;
}


QStringList specFolderItem::descriptorKeys() const
{
	QStringList keys;
	keys << specModelItem::descriptorKeys() ;
	foreach(specModelItem *child, ChildrenList)
	{
		QStringList childKeys = child->descriptorKeys() ;
		for(QStringList::size_type i = 0 ; i < childKeys.size() ; i++)
			if(!keys.contains(childKeys[i]))
				keys << childKeys[i] ;
	}
	return keys ;
}

specModelItem* specFolderItem::child(QList<specModelItem*>::size_type i) const
{ return 0 <= i && i < ChildrenList.size() ? ChildrenList[i] : NULL ; }

int specFolderItem::childNo(specModelItem* child) const
{ return ChildrenList.indexOf(child) ; }

spec::descriptorFlags specFolderItem::descriptorProperties(const QString& key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
	spec::descriptorFlags flags = spec::numeric | spec::editable ;
	foreach(specModelItem* child, ChildrenList)
		flags = (spec::descriptorFlags) (flags & child->descriptorProperties(key)) ;
	return flags ;
}

void specFolderItem::scaleBy(const double& factor)
{
	foreach(specModelItem* child, ChildrenList)
		child->scaleBy(factor) ;
	invalidate(); // TODO include this in child's function
}

void specFolderItem::addToSlope(const double& offset)
{

	foreach(specModelItem* child, ChildrenList)
		child->addToSlope(offset) ;
	invalidate();
}

void specFolderItem::moveYBy(const double& offset)
{
	foreach(specModelItem* child, ChildrenList)
		child->moveYBy(offset) ;
	invalidate();
}

void specFolderItem::moveXBy(const double& offset)
{
	foreach(specModelItem* child, ChildrenList)
		child->moveXBy(offset) ;
	invalidate();
}

void specFolderItem::exportData(const QList<QPair<bool,QString> >& headerFormat, const QList<QPair<spec::value,QString> >& dataFormat, QTextStream& out)
{
	revalidate();
	for (int i = 0 ; i < headerFormat.size() ; i++)
		out << (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	out << endl ;
	/* 	if(mergePlotData) // TODO
	{
		for (int j = 0 ; j < dataSize() ; j++)
			for (int i = 0 ; i < dataFormat.size() ; i++)
				out << dataFormat[i].first << dataFormat[i].second ;
		out << endl ;
	}*/
	// 	else
	// 	{
	foreach(specModelItem* child, ChildrenList)
		child->exportData(headerFormat, dataFormat, out) ;
	// 	}
}

void specFolderItem::subMap(const QMap<double, double> & map)
{
	foreach(specModelItem *item, ChildrenList)
		item->subMap(map) ;
	invalidate();
}

void specFolderItem::deleteDescriptor(const QString &descriptorName)
{
	foreach(specModelItem* child, ChildrenList)
		child->deleteDescriptor(descriptorName) ;
}

void specFolderItem::renameDescriptors(const QMap<QString, QString> &map)
{
	foreach(specModelItem* child, ChildrenList)
		child->renameDescriptors(map) ;
}

void specFolderItem::dumpDescriptor(QList<specDescriptor> &destination, const QString &key) const
{
	specModelItem::dumpDescriptor(destination, key) ;
	foreach(specModelItem* child, ChildrenList)
		child->dumpDescriptor(destination, key) ;
}

void specFolderItem::restoreDescriptor(QListIterator<specDescriptor> &origin, const QString &key)
{
	specModelItem::restoreDescriptor(origin, key) ;
	foreach(specModelItem *child, ChildrenList)
		child->restoreDescriptor(origin, key) ;
}

QList<specModelItem*> specFolderItem::childrenList() const
{
	return ChildrenList ;
}
