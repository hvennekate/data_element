#include "changeplotstyleaction.h"
#include <QDialog>
#include <QGridLayout>
#include <QVBoxLayout>

changePlotStyleAction::changePlotStyleAction(QObject *parent) :
    specUndoAction(parent)
{
}

void changePlotStyleAction::execute()
{
	QwtPlot *plot = (QwtPlot*) parent() ;
	QwtPlotItemList items = plot->itemList() ;

	QDialog lineTypeDialog(plot) ;
	lineTypeDialog.setLayout(new QGridLayout) ;



}

