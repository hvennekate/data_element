#include "specfolderitem.h"
#include <QTextStream>
#include <QTime>

specFolderItem::specFolderItem(specFolderItem* par, QString description)
	: specModelItem(par, description)
{
}

specFolderItem::~specFolderItem()
{
	foreach(specModelItem * childPointer, ChildrenList)
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
		ChildrenList.insert(position + i, list[i]) ;
	}
	invalidate();
	return true ;
}

QList<specModelItem*>::size_type specFolderItem::children() const
{
	return ChildrenList.size() ;
}

bool specFolderItem::isFolder() const { return true ;}

QIcon specFolderItem::decoration() const { return QIcon::fromTheme("folder") ; }

void specFolderItem::refreshPlotData()
{
	QVector<double> x, y ;
	foreach(specModelItem * item, ChildrenList)
	{
		item->revalidate();
		for(size_t i = 0 ; i < item->dataSize() ; i++)
		{
			x << item->sample(i).x() ;
			y << item->sample(i).y() ;
		}
	}
	setSamples(x, y) ;
}

void specFolderItem::removeChild(specModelItem* child)
{
	ChildrenList.removeOne(child) ;  // Maybe all
	invalidate();
}

void specFolderItem::readFromStream(QDataStream& in)
{
	// delete old children
	foreach(specModelItem * child, ChildrenList)
		delete child ;
	quint64 numChildren ;
	specModelItem::readFromStream(in) ;
	in >> numChildren ;
	QList<specModelItem*> newChildren ;
	for(quint64 i = 0 ; i < numChildren ; ++i)
		newChildren << (specModelItem*) produceItem(in) ;  // TODO remove all zeros
	newChildren.removeAll(0) ;
	addChildren(newChildren) ;
}

void specFolderItem::writeToStream(QDataStream& out) const
{
	specModelItem::writeToStream(out) ;
	out << (quint64) children() ;
	foreach(specModelItem * child, ChildrenList)
	out << *child ;
}


QStringList specFolderItem::descriptorKeys() const
{
	QStringList keys = specModelItem::descriptorKeys() ;
	foreach(specModelItem * child, ChildrenList)
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

void specFolderItem::addDataFilter(const specDataPointFilter& filter)
{
	foreach(specModelItem * child, ChildrenList)
	child->addDataFilter(filter);
	invalidate(); // TODO include this in child's function
}

QString specFolderItem::exportData(const QList<QPair<bool, QString> > & headerFormat, const QList<QPair<int, QString> > & dataFormat, const QStringList &numericDescriptors)
{
	revalidate();
	QString result ;
	for(int i = 0 ; i < headerFormat.size() ; i++)
		result += (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	result += '\n' ;
	/* 	if(mergePlotData) // TODO
	{
		for (int j = 0 ; j < dataSize() ; j++)
			for (int i = 0 ; i < dataFormat.size() ; i++)
				out << dataFormat[i].first << dataFormat[i].second ;
		out << endl ;
	}*/
	// 	else
	// 	{
	foreach(specModelItem * child, ChildrenList)
		result += child->exportData(headerFormat, dataFormat, numericDescriptors) ;
	// 	}
	return result ;
}

bool specFolderItem::hasDescriptor(const QString &d) const
{
	return specModelItem::descriptorKeys().contains(d) ;
}

void specFolderItem::deleteDescriptor(const QString& descriptorName)
{
	foreach(specModelItem * child, ChildrenList)
	child->deleteDescriptor(descriptorName) ;
}

void specFolderItem::renameDescriptors(const QMap<QString, QString>& map)
{
	foreach(specModelItem * child, ChildrenList)
	child->renameDescriptors(map) ;
}

void specFolderItem::dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const
{
	specModelItem::dumpDescriptor(destination, key) ;
	foreach(specModelItem * child, ChildrenList)
	child->dumpDescriptor(destination, key) ;
}

void specFolderItem::restoreDescriptor(QListIterator<specDescriptor>& origin, const QString& key)
{
	specModelItem::restoreDescriptor(origin, key) ;
	foreach(specModelItem * child, ChildrenList)
	child->restoreDescriptor(origin, key) ;
}

QList<specModelItem*> specFolderItem::childrenList() const
{
	return ChildrenList ;
}

template<typename T>
QVector<T> specFolderItem::findDescendants()
{
	QVector<T> items = specModelItem::findDescendants<T>();
	// This has been changed from a previous version that would have included
	// items twice (once here and once in their own findDescendants().
	// Since it would only have applied if the item, for which findDescendants() would have
	// been called, had been a specFolderItem and the only usage was in
	// specExchangeFilterCommand::doIt() and specExchangeFilterCommand::setAbsoluteFilter(),
	// and specExchangeFilterCommand is only created in
	// - specNormalizeAction (which is of type specRequiresDataItemAction
	// - specCommandGenerator (obviously)
	// - specSpectrumPlot::pointMoved (which uses the correction picker, which only specDataItem attaches to
	//	(specDataItem::attach) // TODO rework
	// - specSpectrumPlot::applyZeroRanges (which only takes from spec::spectrum rtti
	// - dataItemProperties
	// this should never have been a problem.
	// (also:  the only forced instantiation in specmodelitem.cpp is for
	// specDataItem -- which is necessary, but not sufficient.)
	// In future usage, however, it may very well be
	//
	foreach (specModelItem* item, ChildrenList)
		items << item->findDescendants<T>() ;
	return items ;
}
