#include "specgenealogy.h"

//specGenealogy::specGenealogy(const QList<specModelItem *> &Items, specModel *mod)
//	: Model(mod)
//{
//	getItemPointers(Items) ;
//}

specGenealogy::specGenealogy(const QModelIndex &index)
	: owning(false)
{
	Model = (specModel*) index.model() ;
	Items << Model->itemPointer(index) ;
	indexes = Model->hierarchy(index) ;
	Parent = Items.first()->parent() ;
}

specGenealogy::specGenealogy(QModelIndexList &list)
	: owning(false)
{
	if (list.isEmpty()) return ;
	QModelIndexList::iterator last = list.begin() ;
	Model = (specModel*) last->model() ;
	QModelIndex parentIndex= last->parent() ;
	int next = last->row() ;

	// find iterator of end item
	while (	last != list.end() &&
			last->parent() == parentIndex &&
			last->row() == next)
	{
		next ++ ;
		last ++ ;
	}

	// getting pointers to Items to put into our list
	for (QModelIndexList::iterator i = list.begin() ; i != last ; ++i)
		Items << Model->itemPointer(*i) ;

	// getting index cascade and parent
	indexes = Model->hierarchy(list.first()) ;
	Parent = Items.first()->parent() ;

	// rid the list of those entries we took
	list.erase(list.begin(),last) ;
}

bool specGenealogy::valid()
{
	return Model && Parent && !indexes.isEmpty() && !Items.isEmpty() ;
}


void specGenealogy::takeItems()
{
	if (owning) return ;
	if (!Parent) // TODO: Haeh?  wird doch schon bei valid() geprueft?
		seekParent() ;
	if (!valid()) return ;
	foreach(specModelItem* item, Items)
        if (item) item->setParent(0) ;
	owning = true ;
}

void specGenealogy::returnItems()
{
	if (!owning) return ;
	if (!Parent)
		seekParent() ;
	if (!valid()) return ;
	Parent->addChildren(Items.toList(),indexes.first()) ;
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
	out << indexes
		<< qint8(owning)
		<< qint32(Items.size())
		<< quint64(Model) ;
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
	quint64 m ;
	in >> indexes >> own >> itemCount >> m;
	owning = own ;
	Model = (specModel*) m ; // TODO invalidate?
	Items.fill(0,itemCount);
	if (owning)
		for (int i = 0 ; i < itemCount ; ++i)
			Items[i] = (specModelItem*) produceItem(in) ;
}

specModelItem* specGenealogy::factory(const type &t) const
{
	return specModelItem::itemFactory(t) ;
}

specGenealogy::specGenealogy(specModel* mod, QDataStream &in)
{
	Parent = 0 ;
	Model = mod ;
	readFromStream(in);
}

specGenealogy::specGenealogy()
	: Model(0),
	  Parent(0),
	  owning(false)
{
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
//	seekParent() ;
}

void specGenealogy::getItemPointers()
{
	for (int i = 0 ; i < Items.size() ; ++i) // TODO maybe precautions...
		Items[i] = Parent->child(i+indexes.first()) ;
}


specGenealogy::~specGenealogy()
{
	indexes.clear();
	if(owning)
		foreach(specModelItem* item, Items)
			delete item ;
}

QVector<specModelItem*> specGenealogy::items()
{
	if (!Parent) seekParent() ;
	if (!valid()) return QVector<specModelItem*>() ;
	return Items ;
}

specModelItem* specGenealogy::firstItem()
{
	if (!Parent)
		seekParent() ;
	if (!valid()) return 0 ;
	if (Items.isEmpty()) return 0 ;
	return Items.first() ;
}

QModelIndex specGenealogy::firstIndex()
{
	return model()->index(firstItem()) ;
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

specGenealogy& specGenealogy::operator =(const specGenealogy& other)
{
	Model = other.Model ;
	indexes = other.indexes ;
	Parent = other.Parent ;
	Items = other.Items ;
	owning  = false;
	return *this ;
}

specGenealogy& specGenealogy::operator =(specGenealogy& other)
{
	*this = *(const_cast<const specGenealogy*>(&other)) ; // call the const-version first
	owning  = other.owning ; // then strip it of possible ownership
	other.owning = false ; // taking possession of the items
	return *this ;
}
