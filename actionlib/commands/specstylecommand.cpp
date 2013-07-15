#include "specstylecommand.h"
#include <qwt_symbol.h>


specStyleCommandImplTemplate
specStyleCommandImplFuncTemplate::specStyleCommandImplementation(specUndoCommand *parent )
	: specStyleCommand(parent)
{
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::setItems(QList<specModelItem*> items)
{
	specModel* model = qobject_cast<specModel*>(parentObject()) ;
	QMap<int, QList<specModelItem*> > groups ;
	foreach(specModelItem* item, items)
		groups[styleNo(item)] << item ;

	for (QMap<int, QList<specModelItem*> >::iterator i = groups.begin() ; i != groups.end() ; ++i)
	{
		qSort(i.value().begin(), i.value().end(), model->lessThanItemPointer) ;
		while (!i.value().isEmpty())
			Genealogies << specGenealogy(i.value(), model) ;
	}

	saveStyles(Genealogies) ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::doIt()
{
	for (int i = 0 ; i < Genealogies.size() ; ++i)
		applyStyle(Genealogies[i],-1) ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::undoIt()
{
	for (int i = 0 ; i < Genealogies.size() ; ++i)
		applyStyle(Genealogies[i],i) ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::writeCommand(QDataStream &out) const
{
	out << qint32(Genealogies.size()) << newProperty << oldProperties ;
	for (int i = 0 ; i < Genealogies.size() ; ++i)
		out << Genealogies[i] ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::readCommand(QDataStream &in)
{
	qint32 size ;
	in >> size >> newProperty >> oldProperties ;
	for (qint32 i = 0 ; i < size ; ++i)
	{
		Genealogies << specGenealogy() ;
		in >> Genealogies.last() ;
	}
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::parentAssigned()
{
	specModel *model = qobject_cast<specModel*>(parentObject()) ;
	for (QList<specGenealogy>::iterator i = Genealogies.begin() ; i != Genealogies.end() ; ++i)
		i->setModel(model) ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::applyStyle(specGenealogy &genealogy, int propertyIndex)
{
	property prop = (propertyIndex < 0 || !(propertyIndex < oldProperties.size())) ? newProperty : oldProperties[propertyIndex] ;
	genealogy.seekParent() ;
	if (!genealogy.valid()) return ;
	QVector<specModelItem*> items = genealogy.items() ;
	for (int j = 0 ; j < items.size() ; ++j)
		(items[j]->*setProperty)(prop) ;
	genealogy.signalChange();
}

specStyleCommandImplTemplate
int specStyleCommandImplFuncTemplate::styleNo(specCanvasItem *item)
{
	property prop = (item->*getProperty)() ;
	if (!oldProperties.contains(prop))
		oldProperties << prop ;
	return oldProperties.indexOf(prop) ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::saveStyles(QList<specGenealogy> & list)
{
	oldProperties.clear();

	for (int i = 0 ; i < list.size() ; ++i)
		oldProperties << (list[i].firstItem()->*getProperty)() ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::obtainStyle(specCanvasItem *item)
{
	newProperty = (item->*getProperty)();
}

specStyleCommand *generateStyleCommand(specStreamable::type id, specUndoCommand* parent)
{
	switch (id) // TODO build object containing the template types
	{
	case specStreamable::penColorCommandId :
		return new specStyleCommandImplementation<QColor, &specCanvasItem::penColor, &specCanvasItem::setPenColor, specStreamable::penColorCommandId>(parent) ;
	case specStreamable::symbolStyleCommandId :
		return new specStyleCommandImplementation<int, &specCanvasItem::symbolStyle, &specCanvasItem::setSymbolStyle, specStreamable::symbolStyleCommandId >(parent) ;
	case specStreamable::symbolPenColorCommandId :
		return new specStyleCommandImplementation<QColor, &specCanvasItem::symbolPenColor, &specCanvasItem::setSymbolPenColor, specStreamable::symbolPenColorCommandId >(parent) ;
	case specStreamable::symbolBrushColorCommandId:
		return new specStyleCommandImplementation<QColor, &specCanvasItem::symbolBrushColor, &specCanvasItem::setSymbolBrushColor, specStreamable::symbolBrushColorCommandId>(parent) ;
	case specStreamable::symbolSizeCommandId:
		return new specStyleCommandImplementation<QSize, &specCanvasItem::symbolSize, &specCanvasItem::setSymbolSize, specStreamable::symbolSizeCommandId >(parent) ;
	case specStreamable::lineWidthCommandId:
		return new specStyleCommandImplementation<double, &specCanvasItem::lineWidth, &specCanvasItem::setLineWidth, specStreamable::lineWidthCommandId>(parent) ;
	case specStreamable::penStyleCommandId:
		return new specStyleCommandImplementation<qint8, &specCanvasItem::penStyle, &specCanvasItem::setPenStyle, specStreamable::penStyleCommandId>(parent) ;
	default:
		return 0 ;
	}
	return 0 ;
}
