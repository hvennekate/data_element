#include "specfolderitem.h"
#include <QTextStream>

specFolderItem::specFolderItem(specFolderItem* par, QString description)
 : specModelItem(par, description), suspendRefresh(false)
{
}

specFolderItem::~specFolderItem()
{
	while (children())
		delete childrenList.takeLast() ;
	childrenList.clear() ;
}

bool specFolderItem::addChild(specModelItem* item, QList<specModelItem*>::size_type position)
{
	addChildren(QList<specModelItem*>() << item, position) ;
	return true ;
}

bool specFolderItem::addChildren(QList<specModelItem*> list, QList<specModelItem*>::size_type position)
{
	for(int i = 0 ; i < list.size() ; i++)
	{
		list[i]->setParent(this) ;
		childrenList.insert(position+i, list[i]) ;
	}
	refreshPlotData() ;
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

bool specFolderItem::changeDescriptor(QString key, QString value)
{
	if (key=="")
		return specModelItem::changeDescriptor(key,value) ;
	
	bool success = false ;
	foreach(specModelItem* item, childrenList)
		success |= item->changeDescriptor(key,value) ;
	return success ;
}

void specFolderItem::refreshPlotData()
{ ///! @todo suspend frequent refreshes
	if (suspendRefresh) return ;
	qDebug("refreshing plot data %d",suspendRefresh)  ;
	QVector<double> x, y ;
	foreach(specModelItem* item, childrenList)
	{
		for (int i = 0 ; i < item->dataSize() ; i++)
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
	childrenList.removeAt(childrenList.indexOf(child)) ;
	refreshPlotData() ; ///! @todo move this to view/plotwidget (suspend frequent refreshes) -- maybe removeChildren instead
}

QDataStream& specFolderItem::readFromStream(QDataStream& stream)
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
	qint32 noOfChildrenList ;
	stream >> noOfChildrenList ;
// 	cout << "Folder with " << noOfChildrenList << " children." << endl ;
	QList<specModelItem*> tempChildrenList ;
	while(tempChildrenList.size() < noOfChildrenList)
	{
// 		cout << "Reading child no. " << tempChildrenList.size() << " of " << noOfChildrenList << endl ;
		specModelItem* pointer = 0 ;
		stream >> pointer ;
		tempChildrenList << pointer ;
	}
	addChildren(tempChildrenList,0) ;
// 	cout << "read all children, returning." << endl ;
	return stream ;
}

QDataStream& specFolderItem::writeToStream(QDataStream& stream) const
{
// 	qDebug("writing folder") ;
	stream << (quint8) spec::folder << (qint32) children() ;
	foreach(specModelItem* child, childrenList)
		child->writeOut(stream) ;
	return stream ;
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
	refreshPlotData() ;
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
	{
		child->scaleBy(factor) ;
		child->refreshPlotData() ;
	}
}

void specFolderItem::addToSlope(const double& offset)
{

	foreach(specModelItem* child, childrenList)
	{
		child->addToSlope(offset) ;
		child->refreshPlotData() ;
	}
}

void specFolderItem::moveYBy(const double& offset)
{
	foreach(specModelItem* child, childrenList)
	{
		child->moveYBy(offset) ;
		child->refreshPlotData();
	}
}

void specFolderItem::moveXBy(const double& offset)
{
	foreach(specModelItem* child, childrenList)
	{
		child->moveXBy(offset) ;
		child->refreshPlotData();
	}
}

void specFolderItem::exportData(const QList<QPair<bool,QString> >& headerFormat, const QList<QPair<spec::value,QString> >& dataFormat, QTextStream& out)
{
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

void specFolderItem::haltRefreshes(bool halt)
{
	qDebug("called suspend refresh %d %d",halt,children()) ;
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
