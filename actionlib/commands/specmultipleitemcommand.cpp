#include "specmultipleitemcommand.h"

specMultipleItemCommand::specMultipleItemCommand(specUndoCommand* parent)
	: specUndoCommand(parent)
{
}

void specMultipleItemCommand::clearItems()
{
	items.clear();
}

void specMultipleItemCommand::addItems(QList<specModelItem*>& list)
{
	specModel* m = model() ;
	if(!m) return ;
	while(!list.isEmpty())
		items << specGenealogy(list, m) ;
}

void specMultipleItemCommand::setItems(QList<specModelItem*>& list)
{
	clearItems();
	addItems(list) ;
}

QList<specModelItem*> specMultipleItemCommand::firstItems()
{
	QList<specModelItem*> l ;
	for(int i = 0 ; i < items.size() ; ++i)
		l << items[i].firstItem() ;
	return l ;
}

QList<specModelItem*> specMultipleItemCommand::itemPointers()
{
	QList<specModelItem*> l ;
	for(int i = 0 ; i < items.size() ; ++i)
		l << items[i].items().toList() ; // TODO avoid!
	return l ;
}

qint32 specMultipleItemCommand::itemCount() const
{
	return items.size() ;
}

void specMultipleItemCommand::writeCommand(QDataStream& out) const
{
	out << itemCount() ;
	writeItems(out);
}

void specMultipleItemCommand::readCommand(QDataStream& in)
{
	qint32 num = 0;
	in >> num;
	readItems(in, num);
}

void specMultipleItemCommand::writeItems(QDataStream& out) const
{
	for(int i = 0 ; i < items.size() ; ++i)
		out << items[i] ;
}

void specMultipleItemCommand::takeItems()
{
	specModel* m = model() ;
	if(!m) return ;
	m->signalBeginReset();
	for(int i = 0 ; i < items.size() ; ++i)
		items[i].takeItems();
}

void specMultipleItemCommand::restoreItems()
{
	specModel* m = model() ;
	if(!m) return ;
	m->signalBeginReset();
	for(int i = 0 ; i < items.size() ; i++)
		items[i].returnItems();
}

void specMultipleItemCommand::readItems(QDataStream& in, qint32 n)
{
	items.clear();
	items.resize(n);
	for(int i = 0 ; i < n ; ++i)
		in >> items[i] ;
}

void specMultipleItemCommand::parentAssigned()
{
	specModel* model = dynamic_cast<specModel*>(parentObject()) ;
	Q_ASSERT(model) ;
	for(int i = 0 ; i < items.size() ; ++i)
		items[i].setModel(model) ;
}

QSet<specFolderItem*> specMultipleItemCommand::parents() const
{
	QSet<specFolderItem*> ps ;
	for(int i = 0 ; i < items.size() ; ++i)
		ps << items[i].parent() ;
	ps.remove(0) ;
	return ps ;
}
