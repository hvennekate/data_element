#include "specfolderitem.h"
#include <QTextStream>
#include <QTime>

specFolderItem::specFolderItem(specFolderItem* par, QString description)
 : specModelItem(par, description), suspendRefresh(false)
{
}

specFolderItem::~specFolderItem()
{
	foreach(specModelItem* childPointer, childrenList)
		delete childPointer ;
	childrenList.clear() ;
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
		childrenList.insert(position+i, list[i]) ;
	}
	invalidate();
	return true ;
}
	
QList<specModelItem*>::size_type specFolderItem::children() const
{
	return childrenList.size() ;
}

bool specFolderItem::isFolder() const { return true ;}

QIcon specFolderItem::decoration() const { return QIcon(":/folder.png") ; }

bool specFolderItem::isEditable(QString key) const
{
	if (key =="")
		return specModelItem::isEditable(key) ;
	
	foreach(specModelItem* item, childrenList)
		if(item->isEditable(key))
			return true ;
	
	return false ;
}

void specFolderItem::refreshPlotData()
{ ///! @todo suspend frequent refreshes
	QTime timer ;
	timer.start();
	if (suspendRefresh) return ;
	QVector<double> x, y ;
	foreach(specModelItem* item, childrenList)
	{
		for (size_t i = 0 ; i < item->dataSize() ; i++)
		{
			x << item->sample(i).x() ;
			y << item->sample(i).y() ;
		}
	}
	processData(x,y) ;
	setSamples(x,y) ;
//	processData() ;
}

void specFolderItem::removeChild(specModelItem* child)
{// TODO this is terrible!
	QVector<specModelItem*> newChildren ;
	foreach (specModelItem* childPointer, childrenList)
		if (childPointer != child)
			newChildren << childPointer ;
    childrenList.swap(newChildren) ;
	invalidate();///! @todo move this to view/plotwidget (suspend frequent refreshes) -- maybe removeChildren instead
}

void specFolderItem::readFromStream(QDataStream& in)
{
	foreach(specModelItem* child, childrenList)
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
	foreach(specModelItem* child, childrenList)
		out << *child ;
}


QStringList specFolderItem::descriptorKeys() const
{
	QStringList keys; 
	keys << specModelItem::descriptorKeys() ;
	foreach(specModelItem *child, childrenList)
	{
		QStringList childKeys = child->descriptorKeys() ;
		for(QStringList::size_type i = 0 ; i < childKeys.size() ; i++)
			if(!keys.contains(childKeys[i]))
				keys << childKeys[i] ;
	}
	return keys ;
}

void specFolderItem::buildTree(QStringList descriptors)
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "Starting tree-building... (descriptors: " ;
// 	for (int i = 0 ; i < descriptors.size() ; i++)
// 		cout << descriptors[i] << ", " ;
// 	cout << ")" << endl ;
	if(descriptors.isEmpty())
		return ;
	QString compDescriptor = descriptors.takeFirst() ;
// 	cout << "starting comparison on:  " << compDescriptor << endl ;
	suspendRefresh = true ;
	for (int i = 0 ; i < children() ; i++)
	{
		QList<specModelItem*> list ;
		QString compValue = childrenList[i]->descriptor(compDescriptor) ;
		// Generate List of items to put in one folder
		for (int j = i ; j < children() ; j++)
			if(childrenList[j]->descriptor(compDescriptor) == compValue)
				list << childrenList[j] ;
		
		// if list is more than one item and not merely all the childrenList create new item,
		// move selected items there, perform tree-building within that item and add it as
		// a child.
		if(list.size() == children()) break ;
// 		cout << "Starting to build up new child with " << list.size() << "items.  (Common value:  " << compDescriptor << ")" << endl ;
		if(list.size() > 1)
		{
// 			cout << "Creating new folder... " << endl ;
			specFolderItem *newItem = new specFolderItem(0,compValue) ;
// 			cout << "Moving selected items over to new folder..." << endl ;
			newItem->addChildren(list,0) ;
// 			cout << "Initiating tree-building in newly created folder..." << endl ;
			newItem->buildTree(descriptors) ;
// 			cout << "Adding new folder as child" << endl ;
			addChild(newItem,i) ;
		}
	}
	// Maybe the remaining criteria are usable for some tree-building...
	buildTree(descriptors) ;
	suspendRefresh = false ;
	invalidate();
}

specModelItem* specFolderItem::child(QList<specModelItem*>::size_type i) const
{ return 0 <= i && i < childrenList.size() ? childrenList[i] : NULL ; }

int specFolderItem::childNo(specModelItem* child) const
{ return childrenList.indexOf(child) ; }

spec::descriptorFlags specFolderItem::descriptorProperties(const QString& key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
	spec::descriptorFlags flags = spec::numeric | spec::editable ;
	foreach(specModelItem* child, childrenList)
		flags = (spec::descriptorFlags) (flags & child->descriptorProperties(key)) ;
	return flags ;
}

void specFolderItem::scaleBy(const double& factor)
{
	foreach(specModelItem* child, childrenList)
		child->scaleBy(factor) ;
	invalidate(); // TODO include this in child's function
}

void specFolderItem::addToSlope(const double& offset)
{

	foreach(specModelItem* child, childrenList)
		child->addToSlope(offset) ;
	invalidate();
}

void specFolderItem::moveYBy(const double& offset)
{
	foreach(specModelItem* child, childrenList)
		child->moveYBy(offset) ;
	invalidate();
}

void specFolderItem::moveXBy(const double& offset)
{
	foreach(specModelItem* child, childrenList)
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
		foreach(specModelItem* child, childrenList)
			child->exportData(headerFormat, dataFormat, out) ;
// 	}
}

void specFolderItem::haltRefreshes(bool halt) // TODO redundant
{
	suspendRefresh = halt ;
	refreshPlotData() ;
}

void specFolderItem::subMap(const QMap<double, double> & map)
{
	haltRefreshes(true) ;
	foreach(specModelItem *item, childrenList)
		item->subMap(map) ;
	haltRefreshes(false) ;
}

void specFolderItem::deleteDescriptor(const QString &descriptorName)
{
    foreach(specModelItem* child, childrenList)
        child->deleteDescriptor(descriptorName) ;
}

void specFolderItem::renameDescriptors(const QMap<QString, QString> &map)
{
    foreach(specModelItem* child, childrenList)
        child->renameDescriptors(map) ;
}

void specFolderItem::dumpDescriptor(QList<specDescriptor> &destination, const QString &key) const
{
    specModelItem::dumpDescriptor(destination, key) ;
    foreach(specModelItem* child, childrenList)
        child->dumpDescriptor(destination, key) ;
}

void specFolderItem::restoreDescriptor(QListIterator<specDescriptor> &origin, const QString &key)
{
    specModelItem::restoreDescriptor(origin, key) ;
    foreach(specModelItem *child, childrenList)
        child->restoreDescriptor(origin, key) ;
}
