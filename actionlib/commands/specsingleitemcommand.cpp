#include "specsingleitemcommand.h"
#include "specdataitem.h"
#include "specsvgitem.h"
#include "specmetaitem.h"

template <class itemType>
specSingleItemCommand<itemType>::specSingleItemCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}

template <class itemType>
void specSingleItemCommand<itemType>::setItem(specModelItem *pointer)
{
	if (!model()) return ;
	if (dynamic_cast<itemType*>(pointer))
		item = specGenealogy(pointer, model()) ;
}

template <class itemType>
void specSingleItemCommand<itemType>::readCommand(QDataStream &in)
{
	readItem(in) ;
}

template <class itemType>
void specSingleItemCommand<itemType>::writeCommand(QDataStream &out) const
{
	writeItem(out) ;
}

template <class itemType>
void specSingleItemCommand<itemType>::readItem(QDataStream& in)
{
	in >> item ;
}

template <class itemType>
void specSingleItemCommand<itemType>::writeItem(QDataStream &out) const
{
	out << item ;
}

template <class itemType>
void specSingleItemCommand<itemType>::parentAssigned()
{
	specModel* model = dynamic_cast<specModel*>(parentObject()) ;
	Q_ASSERT(model) ;
	item.setModel(model) ;
}

template <class itemType>
itemType* specSingleItemCommand<itemType>::itemPointer() const
{
	return (item.size() ? dynamic_cast<itemType*>( item.items().first() ) : 0) ;
}

// Force instatiation
template class specSingleItemCommand<specModelItem> ;
template class specSingleItemCommand<specDataItem> ;
template class specSingleItemCommand<specMetaItem> ;
template class specSingleItemCommand<specSVGItem> ;
