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

	specView *view = (specView*) parentWidget() ;
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
void specStyleCommandImplFuncTemplate::redo()
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
void specStyleCommandImplFuncTemplate::undo()
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
QDataStream& specStyleCommandImplFuncTemplate::write(QDataStream &out) const
{
	out << quint32(Genealogies.size()) ;
	for (int i = 0 ; i < Genealogies.size() ; ++i)
		Genealogies[i].write(out) ;
	out << newProperty << oldProperties ;
	return out ;
}

specStyleCommandImplTemplate
QDataStream& specStyleCommandImplFuncTemplate::read(QDataStream &in)
{
	quint32 size ;
	specModel* model = ((specView*) parentWidget())->model() ;
	in >> size ;
	for (int i = 0 ; i < size ; ++i)
		Genealogies << specGenealogy(model,in) ;
	in >> newProperty >> oldProperties ;
	return in ;
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

specStyleCommand *generateStyleCommand(spec::undoActionIds id)
{
	switch (id)
	{
	case spec::penColorId :
		return new specStyleCommandImplementation<QColor, &specCanvasItem::penColor, &specCanvasItem::setPenColor, spec::penColorId> ;
	case spec::symbolStyleId :
		return new specStyleCommandImplementation<int, &specCanvasItem::symbolStyle, &specCanvasItem::setSymbolStyle, spec::symbolStyleId > ;
	case spec::symbolPenColorId :
		return new specStyleCommandImplementation<QColor, &specCanvasItem::symbolPenColor, &specCanvasItem::setSymbolPenColor, spec::symbolPenColorId > ;
	}
}
