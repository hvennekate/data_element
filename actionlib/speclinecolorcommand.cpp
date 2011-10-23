#include "speclinecolorcommand.h"

specLineColorCommand::specLineColorCommand(specUndoCommand* parent)
	: specStyleCommand(parent)
{
}

void specLineColorCommand::applyStyle(const specGenealogy & genealogy, int colorNum)
{
	QColor color = (colorNum < 0 || !(colorNum < oldColors.size()) ) ? newColor : oldColors[colorNum] ;
	QList<specModelItem*> items = genealogy.items() ;
	for (int j = 0 ; j < items.size() ; ++j)
	{
		specModelItem *item = items[j] ;
		QPen pen = item->pen() ;
		pen.setColor(color) ;
		item->setPen(pen) ;
	}
}

int specLineColorCommand::styleNo(specCanvasItem *item)
{
	QColor color = item->pen().color() ;
	if (!oldColors.contains(color))
		oldColors << color ;
	return oldColors.indexOf(color) ;
}

void specLineColorCommand::saveStyles(const QList<specGenealogy> & list)
{
	oldColors.clear();

	for (int i = 0 ; i < list.size() ; ++i)
		oldColors << list[i].items().first()->pen().color() ;
}

void specLineColorCommand::setStyle(specCanvasItem *item)
{
	newColor = item->pen().color() ;
}

QDataStream& specLineColorCommand::write(QDataStream &out) const
{
	out << newColor << oldColors ;
	return specStyleCommand::write(out) ;
}

QDataStream& specLineColorCommand::read(QDataStream &in)
{
	in >> newColor >> oldColors ;
	return specStyleCommand::read(in) ;
}
