#include "specgenealogy.h"

specGenealogy::specGenealogy(const QList<specModelItem *> &Items, specModel *mod)
	: Model(mod)
{
	getItemPointers(Items) ;
}

specGenealogy::specGenealogy(const QModelIndexList &list)
	: Model(mod)
{
	QList<specModelItem*> Items ;
	if (list.isEmpty()) return ;
	specModel* model = list.first().model() ;
	foreach(QModelIndex item, list)
		if (item.model() == model)
			Items << model->itemPointer(item) ;
	getItemPointers(Items) ;
}

void specGenealogy::getItemPointers(const QList<specModelItem *> &Items)
{
	if (Items.isEmpty())
		return ;
	items.clear();
	indexes.clear() ;
	specFolderItem *parent = Items.first()->parent() ;
	int nextRow = parent->childNo(Items.first()) ;
	for (int i = 0 ; i < Items.size() ; ++i)
	{
		specModelItem* pointer = Items[i] ;
		if (pointer && pointer->parent() == parent && parent->childNo(pointer) == nextRow)
		{
			items << pointer ;
			nextRow++ ;
		}
	}

	indexes << parent->childNo(items.first()) ;
	while (parent)
	{
		indexes << parent->parent()->childNo(parent) ;
		parent = parent->parent() ;
	}
}

bool specGenealogy::valid()
{
	return Model && parent && !indexes.isEmpty() && !items.isEmpty() ;
}


void specGenealogy::takeItems()
{
	if (owning) return ;
	if (!valid()) return ;
	foreach(specModelItem* item, items)
		item->setParent(0) ;
	owning = true ;
}

void specGenealogy::returnItems()
{
	if (!owning) return ;
	if (!valid()) return ;
	parent->addChildren(items,indexes.first()) ;
	owning = false ;
}

QDataStream &specGenealogy::write(QDataStream &out)
{
	return out ;
}

QDataStream &specGenealogy::read(QDataStream &in)
{
	return in ;
}
