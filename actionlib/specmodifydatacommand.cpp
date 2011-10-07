#include "specmodifydatacommand.h"

#include "specmodel.h"
#include "specview.h"

specModifyDataCommand::specModifyDataCommand(specUndoCommand* parent)
	: specUndoCommand(parent)
{
}


void specModifyDataCommand::redo()
{
	for(QMap<specGenealogy,QList<int> >::iterator i = corrections.begin() ; i != corrections.end() ; ++i) // TODO implement canonically
	{
		for (int j = 0 ; j < i.key().items().size() ; ++j)
		{
			specModelItem *item = i.key().items()[j] ;
			switch (i.value().size())
			{ // TODO:  make sure to avoid scaling by zero!
			case 4:
				item->moveXBy(i.value()[3]) ;
				item->refreshPlotData(); // TODO verify!
			case 3:
				item->scaleBy(i.value()[2]);
			case 2:
				item->moveYBy(i.value()[1]) ;
			case 1:
				item->addToSlope(i.value()[0]);
			default:
			}
			item->refreshPlotData();
		}
	}
}

void specModifyDataCommand::undo()
{
	for(QMap<specGenealogy,QList<int> >::iterator i = corrections.begin() ; i != corrections.end() ; ++i)
	{
		for (int j = 0 ; j < i.key().items().size() ; ++j)
		{
			specModelItem *item = i.key().items()[j] ;
			switch (i.value().size())
			{
			case 4:
				item->moveXBy(-(i.value()[3])) ;
				item->refreshPlotData(); // TODO verify!
			case 3:
				item->scaleBy(1./(i.value()[2]));
			case 2:
				item->moveYBy(-(i.value()[1])) ;
			case 1:
				item->addToSlope(-(i.value()[0])) ; // TODO verify
			default:
			}
			item->refreshPlotData();
		}
	}
}
