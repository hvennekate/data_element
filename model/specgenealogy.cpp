#include "specgenealogy.h"

//specGenealogy::specGenealogy(const QList<specModelItem *> &Items, specModel *mod)
//	: Model(mod)
//{
//	getItemPointers(Items) ;
//}

specGenealogy::specGenealogy(QModelIndexList &list)
	: owning(false)
{
	qDebug("initializing genealogy") ;
	QList<specModelItem*> Items ;
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

	// getting pointers to items to put into our list
	qDebug("adding items to own list") ;
	for (QModelIndexList::iterator i = list.begin() ; i != last ; ++i)
		items << Model->itemPointer(*i) ;

	qDebug("retrieving indexes") ;
	// getting index cascade and parent
	QModelIndex item = list.first() ;
	while(item.isValid())
	{
		indexes << item.row() ;
		item = item.parent() ;
	}
	Parent = items.first()->parent() ;

	qDebug("removing indexes from list") ;
	// rid the list of those entries we took
	list.erase(list.begin(),last) ;
}

bool specGenealogy::valid()
{
	return Model && Parent && !indexes.isEmpty() && !items.isEmpty() ;
}


void specGenealogy::takeItems()
{
	qDebug("starting to remove items") ;
	if (owning) return ;
	if (!valid()) return ;
	qDebug("checks done") ;
	foreach(specModelItem* item, items)
		item->setParent(0) ;
	owning = true ;
	qDebug("done removing") ;
}

void specGenealogy::returnItems()
{
	if (!owning) return ;
	if (!valid()) return ;
	Parent->addChildren(items,indexes.first()) ;
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

QDataStream &specGenealogy::write(QDataStream &out)
{
	out << indexes ;
	out << qint8(owning) ;
	out << qint32(items.size()) ;
	if (owning)
	{
		for(int i = 0 ; i < items.size() ; ++i)
			items[i]->writeOut(out) ;
	}
	return out ;
}

specGenealogy::specGenealogy(specModel* mod, QDataStream &in)
{
	qDebug("reading genealogy...") ;
	Parent = 0 ;
	Model = mod ;
	in >> indexes ;
	qint8 tempOwning ;
	in >> tempOwning ;
	owning = tempOwning ;
	qint32 toRead ;
	in >> toRead ;
	if (owning)
	{
		qDebug("owning items") ;
		specModelItem *pointer ;
		for (int i = 0 ; i < toRead ; ++i)
		{
			in >> pointer ;
			items << pointer ;
		}
	}
	else
	{
		qDebug("not owning items") ;
		seekParent() ;
		for (int i = indexes.first() ; i < indexes.first()+toRead ; ++i)
			items << Parent->child(i) ;
	}
}

bool specGenealogy::seekParent()
{
	if (!Model) return false ;
	QModelIndex parentIndex = QModelIndex() ;
	for (int i = indexes.size()-1 ; i > 0 ; --i)
		parentIndex = Model->index(indexes[i],0,parentIndex) ;
	Parent = (specFolderItem*) Model->itemPointer(parentIndex) ;

	return Parent ;
}

