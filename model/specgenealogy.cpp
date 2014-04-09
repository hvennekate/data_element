#include "specgenealogy.h"
#include "specfolderitem.h"


specGenealogy::specGenealogy(specModelItem* p, specModel* m)
	: Model(m),
	  owning(false)
{
	Items << p ;
	indexes = Model->hierarchy(p) ;
	Parent = p->parent() ;
}

specGenealogy::specGenealogy(QList<specModelItem*>& l, specModel* m)
	: Model(m),
	  owning(false)
{
	if(l.empty()) return ;
	Parent = l.first()->parent() ;
	indexes = Model->hierarchy(l.first()) ;
	int row = Parent->childNo(l.first()) ;

	QList<specModelItem*>::iterator it = l.begin() ;
	while(it != l.end()
		&& (*it)->parent() == Parent
		&& Parent->childNo(*it) == row)
	{
		Items << *it ;
		++it ;
		++row ;
	}

	l.erase(l.begin(), it) ;
}

bool specGenealogy::valid() const // TODO this should actually be done by some sort of
{
	// state machine mechanism (like "valid" <-> "invalid")
	return Model && Parent && !indexes.isEmpty() && !Items.isEmpty() && Items.first() ;
}


void specGenealogy::takeItems()
{
	if(owning) return ;
	if(!Parent)  // TODO: Haeh?  wird doch schon bei valid() geprueft?
		seekParent() ;
	if(!valid()) return ;
	foreach(specModelItem * item, Items)
	if(item) item->setParent(0) ;
	owning = true ;
}

void specGenealogy::returnItems()
{
	if(!owning) return ;
	if(!Parent)
		seekParent() ;
	if(!valid()) return ;
	Parent->addChildren(Items.toList(), indexes.first()) ;
	owning = false ;
}

specModel* specGenealogy::model() const
{
	return Model ;
}

specFolderItem* specGenealogy::parent() const
{
	return Parent ;
}

void specGenealogy::writeToStream(QDataStream& out) const
{
	out << indexes
	    << qint8(owning)
	    << qint32(Items.size())
	    << quint64(Model) ;  // TODO: Why? Why?
	if(owning)
	{
		for(int i = 0 ; i < Items.size() ; ++i)
			out << * (Items[i]) ;
	}
}

void specGenealogy::readFromStream(QDataStream& in)
{
	clear();
	qint8 own ;
	qint32 itemCount ;
	quint64 m ;
	in >> indexes >> own >> itemCount >> m;
	owning = own ;
	Model = 0 ;
	Items.fill(0, itemCount);
	if(owning)
		for(int i = 0 ; i < itemCount ; ++i)
			Items[i] = (specModelItem*) produceItem(in) ;
}

specModelItem* specGenealogy::factory(const type& t) const
{
	return specModelItem::itemFactory(t) ;
}

specGenealogy::specGenealogy()
	: Model(0),
	  Parent(0),
	  owning(false)
{
}

bool specGenealogy::seekParent()
{
	if(!Model) return false ;
	specModelItem* item = Model->itemPointer(indexes) ;
	if(!item) return false ;
	Parent = item->parent() ; // Only look for the parent
	if(Parent && !owning)
		getItemPointers();
	return Parent ;
}

void specGenealogy::setModel(specModel* model)
{
	Model = model ;
	//	seekParent() ;
}

void specGenealogy::getItemPointers()
{
	for(int i = 0 ; i < Items.size() ; ++i)  // TODO maybe precautions...
		Items[i] = Parent->child(i + indexes.first()) ;
}

void specGenealogy::clear()
{
	indexes.clear();
	if(owning)
		foreach(specModelItem * item, Items)
		delete item ;
}

specGenealogy::~specGenealogy()
{
	clear() ;
}

QVector<specModelItem*> specGenealogy::items()
{
	if(!Parent) seekParent() ;
	if(!valid()) return QVector<specModelItem*>() ;
	return Items ;
}

QVector<specModelItem*> specGenealogy::items() const
{
	if(!valid()) return QVector<specModelItem*>() ;
	return Items ;
}

specModelItem* specGenealogy::firstItem()
{
	if(!Parent)
		seekParent() ;
	if(!valid()) return 0 ;
	if(Items.isEmpty()) return 0 ;
	return Items.first() ;
}

specModelItem* specGenealogy::firstItem() const
{
	if(!valid()) return 0 ;
	if(Items.isEmpty()) return 0 ;
	return Items.first() ;
}

QModelIndex specGenealogy::firstIndex()
{
	return model()->index(firstItem()) ;
}

bool specGenealogy::operator == (const specGenealogy& other) const
{
	bool returnValue = true ;
	if(other.Items.size() != Items.size()) return false ;
	for(int i = 0 ; i < Items.size() ; ++i)
		returnValue = returnValue && (Items[i] == other.Items[i]) ;
	return returnValue ;
}

bool specGenealogy::operator < (const specGenealogy& other) const
{
	return specModel::lessThanHierarchies(indexes, other.indexes) ;
}

bool specGenealogy::operator != (const specGenealogy& other) const
{
	return !(*this == other) ;
}

specGenealogy& specGenealogy::operator = (const specGenealogy& other)
{
	clear() ;
	Model = other.Model ;
	indexes = other.indexes ;
	Parent = other.Parent ;
	Items = other.Items ;
	owning  = false;
	return *this ;
}

specGenealogy& specGenealogy::operator = (specGenealogy& other)
{
	*this = * (const_cast<const specGenealogy*>(&other)) ;  // call the const-version first
	owning  = other.owning ; // then strip it of possible ownership
	other.owning = false ; // taking possession of the items
	return *this ;
}

void specGenealogy::signalChange() const
{
	if(!Model) return ;
	if(Items.isEmpty()) return ;
	if(owning) return ;
	Model->signalChanged(Model->index(Items.first()), Model->index(Items.last(), Model->descriptors().size() - 1));
}

QTextStream& operator<< (QTextStream& out, const specGenealogy& g)
{
	QString result = "Genealogy ";
	foreach(const int & i, g.indexes)
	result += QString::number(i) + "/" ;
	result += " size: " + QString::number(g.Items.size()) ;
	return out << result ;
}
