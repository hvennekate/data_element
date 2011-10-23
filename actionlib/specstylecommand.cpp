#include "specstylecommand.h"

specStyleCommand::specStyleCommand(specUndoCommand *parent )
	: specUndoCommand(parent)
{
}

void specStyleCommand::setItems(QModelIndexList items)
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
}

void specStyleCommand::redo()
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

void specStyleCommand::undo()
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

QDataStream& specStyleCommand::write(QDataStream &out) const
{
	out << quint32(Genealogies.size()) ;
	for (int i = 0 ; i < Genealogies.size() ; ++i)
		Genealogies[i].write(out) ;
	return out ;
}

QDataStream& specStyleCommand::read(QDataStream &in)
{
	quint32 size ;
	specModel* model = ((specView*) parentWidget())->model() ;
	in >> size ;
	for (int i = 0 ; i < size ; ++i)
		Genealogies << specGenealogy(model,in) ;
	return in ;
}

