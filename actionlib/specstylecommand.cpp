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
	qDebug() << "list size: " << items ;

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
		int oldSize = Genealogies.size() ;
		while (!i.value().isEmpty())
			Genealogies << specGenealogy(i.value()) ;
	}

	saveStyles(Genealogies) ;

	qDebug() << "styles: " << oldProperties << newProperty ;
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
	plot->replot();
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
	plot->replot();

}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::writeToStream(QDataStream &out) const
{
	out << quint32(Genealogies.size()) << newProperty << oldProperties ;
	for (int i = 0 ; i < Genealogies.size() ; ++i)
		out << Genealogies[i] ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::readFromStream(QDataStream &in)
{
//	clear() ;  TODO: where is this function?
	quint32 size ;
	in >> size >> newProperty >> oldProperties ;
	for (int i = 0 ; i < size ; ++i)
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
void specStyleCommandImplFuncTemplate::applyStyle(const specGenealogy &genealogy, int propertyIndex)
{
	property prop = (propertyIndex < 0 || !(propertyIndex < oldProperties.size())) ? newProperty : oldProperties[propertyIndex] ;
	QList<specModelItem*> items = genealogy.items() ;
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
void specStyleCommandImplFuncTemplate::saveStyles(const QList<specGenealogy> & list)
{
	oldProperties.clear();

	for (int i = 0 ; i < list.size() ; ++i)
		oldProperties << (list[i].items().first()->*getProperty)() ;
}

specStyleCommandImplTemplate
void specStyleCommandImplFuncTemplate::obtainStyle(specCanvasItem *item)
{
	newProperty = (item->*getProperty)();
	qDebug() << "getting property from: " << item << newProperty << "original:" << (item->*getProperty)() ;
}

specStyleCommand *generateStyleCommand(specStreamable::streamableType id)
{
	switch (id)
	{
	case specStreamable::penColorCommandId :
		return new specStyleCommandImplementation<QColor, &specCanvasItem::penColor, &specCanvasItem::setPenColor, specStreamable::penColorCommandId> ;
	case specStreamable::symbolStyleCommandId :
		return new specStyleCommandImplementation<int, &specCanvasItem::symbolStyle, &specCanvasItem::setSymbolStyle, specStreamable::symbolStyleCommandId > ;
	case specStreamable::symbolPenColorCommandId :
		return new specStyleCommandImplementation<QColor, &specCanvasItem::symbolPenColor, &specCanvasItem::setSymbolPenColor, specStreamable::symbolPenColorCommandId > ;
	default:
		return 0 ;
	}
	return 0 ;
}
