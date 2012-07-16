#include "specgenealogy.h"

//specGenealogy::specGenealogy(const QList<specModelItem *> &Items, specModel *mod)
//	: Model(mod)
//{
//	getItemPointers(Items) ;
//}

specGenealogy::specGenealogy(QModelIndexList &list)
	: owning(false),
	  knowingParent(false)
{
	qDebug("initializing genealogy") ;
	if (list.isEmpty()) return ;
	QModelIndexList::iterator last = list.begin() ;
	Model = (specModel*) last->model() ;
	QModelIndex parentIndex= last->parent() ;
	int next = last->row() ;

	// find iterator of end item
	qDebug("finding end item") ;
	while (	last != list.end() &&
			last->parent() == parentIndex &&
			last->row() == next)
	{
		next ++ ;
		last ++ ;
	}

	// getting pointers to Items to put into our list
	qDebug("adding Items to own list") ;
	for (QModelIndexList::iterator i = list.begin() ; i != last ; ++i)
		Items << Model->itemPointer(*i) ;

	qDebug("retrieving indexes") ;
	// getting index cascade and parent
	indexes = Model->hierarchy(list.first()) ;
	Parent = Items.first()->parent() ;

	qDebug("removing indexes from list") ;
	// rid the list of those entries we took
	list.erase(list.begin(),last) ;
	qDebug("items list size: %d  Validity: %d %d %d %d",Items.size(), Model, Parent, !indexes.isEmpty(), !Items.isEmpty()) ;
}

bool specGenealogy::valid()
{
	qDebug("Validity: %d %d %d %d", Model, Parent, !indexes.isEmpty(), !Items.isEmpty()) ;
	return Model && Parent && !indexes.isEmpty() && !Items.isEmpty() ;
}


void specGenealogy::takeItems()
{
	qDebug("starting to remove Items") ;
	if (owning) return ;
	if (!valid()) return ;
	qDebug() << "before parent search" << Parent ;
	if (!Parent)
		seekParent() ;
	qDebug() << "checks done" << Parent ;
	foreach(specModelItem* item, Items)
		item->setParent(0) ;
	owning = true ;
	qDebug("done removing") ;
}

void specGenealogy::returnItems()
{
	if (!owning) return ;
	if (!valid()) return ;
	if (!Parent)
		seekParent() ;
	Parent->addChildren(Items,indexes.first()) ;
	owning = false ;
}

specModel* specGenealogy::model()
{
	return Model ;
}

specFolderItem* specGenealogy::parent()
{
	return Parent ;
}

void specGenealogy::writeToStream(QDataStream &out) const
{
	out << indexes << qint8(owning) << qint32(Items.size()) ;
	if (owning)
	{
		for(int i = 0 ; i < Items.size() ; ++i)
			out << *(Items[i]) ;
	}
}

void specGenealogy::readFromStream(QDataStream &in)
{
	qint8 own ;
	qint32 itemCount ;
	in >> indexes >> own >> itemCount ;
	owning = own ;
	Items.fill(0,itemCount);
	if (owning)
		for (int i = 0 ; i < itemCount ; ++i)
			Items[i] = (specModelItem*) produceItem(in) ;
}

specStreamable* specGenealogy::factory(const type &t) const
{
	return specModelItem::itemFactory(t) ;
}

specGenealogy::specGenealogy(specModel* mod, specInStream &in)
	: knowingParent(false)
{
	qDebug("reading genealogy...") ;
	Parent = 0 ;
	Model = mod ;
	readFromStream(in);
}

bool specGenealogy::seekParent()
{
	if (!Model) return false ;
	Parent = (specFolderItem*) Model->itemPointer(indexes.mid(1)) ;
	if (!owning)
		getItemPointers();

	return Parent ;
}

void specGenealogy::setModel(specModel *model)
{
	Model = model ;
}

void specGenealogy::getItemPointers()
{
	for (int i = 0 ; i < Items.size() ; ++i)
		Items[i] = Parent->child(i+indexes.first()) ;
}


specGenealogy::~specGenealogy()
{
	indexes.clear();
	if(owning)
		foreach(specModelItem* item, Items)
			delete item ;
}

const QList<specModelItem*>& specGenealogy::items() const
{
	return Items ;
}

bool specGenealogy::operator ==(const specGenealogy& other)
{
	bool returnValue = true ;
	if (other.Items.size() != Items.size()) return false ;
	for (int i = 0 ; i < Items.size() ; ++i)
		returnValue = returnValue && (Items[i] == other.Items[i]) ;
	return returnValue ;
}

bool specGenealogy::operator !=(const specGenealogy& other)
{
	return !(*this == other) ;
}
