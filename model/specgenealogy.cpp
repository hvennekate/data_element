#include "specgenealogy.h"
#include "specfolderitem.h"


specGenealogy::specGenealogy(specModelItem *p, specModel *m)
	: Model(m),
	  owning(false)
{
	Items << p ;
	indexes = Model->hierarchy(p) ;
	Parent = p->parent() ;
}

specGenealogy::specGenealogy(QList<specModelItem *>& l, specModel *m)
	: Model(m),
	  owning(false)
{
	if (l.empty()) return ;
	Parent = l.first()->parent() ;
	indexes = Model->hierarchy(l.first()) ;
	int row = Parent->childNo(l.first()) ;

	QList<specModelItem*>::iterator it = l.begin() ;
	while (it != l.end()
		   && (*it)->parent() == Parent
		   && Parent->childNo(*it) == row)
	{
		Items << *it ;
		++it ;
		++row ;
	}

	l.erase(l.begin(), it) ;
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
	Parent = (specFolderItem*) (Model->itemPointer(indexes.mid(1))); // Only look for the parent
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
