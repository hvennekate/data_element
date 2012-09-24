#include "specstylecommand.h"
#include <qwt_symbol.h>


specStyleCommandImplTemplate
specStyleCommandImplFuncTemplate::specStyleCommandImplementation(specUndoCommand *parent )
	: specStyleCommand(parent)
{
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::setItems(QModelIndexList items)
{

	//	for (int i = 0 ; i < items.size() ; ++i)
	//		if (!dynamic_cast<specCanvasItem*>(model->itemPointer(items[i])))
	//			items.takeAt(i--) ;

	specView *view = (specView*) parentObject() ;
	specModel *model = view->model() ;

	QMap<int, QModelIndexList > groups ;
	for (int i = 0 ; i < items.size() ; ++i)
	{
		specModelItem* item = dynamic_cast<specModelItem*>(model->itemPointer(items[i])) ;
		if (!item) continue ;
		groups[styleNo(item)] << items[i] ;
	}

	for (QMap<int, QModelIndexList>::iterator i = groups.begin() ; i != groups.end() ; ++i)
	{
		qSort(i.value()) ;
		while (!i.value().isEmpty())
			Genealogies << specGenealogy(i.value()) ;
	}

	saveStyles(Genealogies) ;

}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::doIt()
{
	specPlot *plot = 0 ;
	for (int i = 0 ; i < Genealogies.size() ; ++i)
	{
		for (int j = 0 ; j < Genealogies[i].items().size() ; ++ j)
			if (Genealogies[i].items()[j]->plot())
				plot = (specPlot*) Genealogies[i].items()[j]->plot() ;
		applyStyle(Genealogies[i],-1) ;
	}
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::undoIt()
{
	specPlot *plot = 0 ;
	for (int i = 0 ; i < Genealogies.size() ; ++i)
	{
		for (int j = 0 ; j < Genealogies[i].items().size() ; ++ j)
			if (Genealogies[i].items()[j]->plot())
				plot = (specPlot*) Genealogies[i].items()[j]->plot() ;
		applyStyle(Genealogies[i],i) ;
	}
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::writeCommand(QDataStream &out) const
{
	out << quint32(Genealogies.size()) << newProperty << oldProperties ;
	for (int i = 0 ; i < Genealogies.size() ; ++i)
		out << Genealogies[i] ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::readCommand(QDataStream &in)
{
//	clear() ;  TODO: where is this function?
	quint32 size ;
	in >> size >> newProperty >> oldProperties ;
	for (quint32 i = 0 ; i < size ; ++i)
	{
		Genealogies << specGenealogy() ;
		in >> Genealogies.last() ;
	}
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::parentAssigned()
{
	if (!parentObject()) return ;
	specModel *model = (specModel*) (((QAbstractItemView*) parentObject())->model()) ;
	foreach(specGenealogy genealogy, Genealogies)
		genealogy.setModel(model) ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::applyStyle(specGenealogy &genealogy, int propertyIndex)
{
	property prop = (propertyIndex < 0 || !(propertyIndex < oldProperties.size())) ? newProperty : oldProperties[propertyIndex] ;
	QVector<specModelItem*> items = genealogy.items() ;
	for (int j = 0 ; j < items.size() ; ++j)
		(items[j]->*setProperty)(prop) ;
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
	switch (id)
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
	default:
		return 0 ;
	}
	return 0 ;
}
