#include "changeplotstyleaction.h"
#include <QDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDebug>
#include "specgenealogy.h"
#include "speclinecolorcommand.h"
#include <QColorDialog>
#include <qwt_symbol.h>
#include <QPainter>
#include "specchangesymbolcommand.h"

changePlotStyleAction::changePlotStyleAction(QObject *parent) :
    specUndoAction(parent)
{
	setMenu(new QMenu) ;
	lineColorAction = new QAction("line color",this) ;
	menu()->addAction(lineColorAction) ;

	symbolAction = new QAction("symbol",this) ;
	symbolAction->setMenu(new QMenu) ;

	QSize iconSize(16,16) ;
	QPixmap icon(iconSize) ;
	QPainter painter(&icon) ;
	QPen pen ;
	QwtSymbol symbol ;
	symbol.setSize(iconSize) ;
	for(int i = -1 ; i < QwtSymbol::Hexagon ; ++i)
	{
		symbol.setStyle(QwtSymbol::Style(i)) ;
		icon.fill(Qt::white); // change to transparent
		symbol.drawSymbol(&painter,QPointF(iconSize.width()/2.,iconSize.height()/2.)) ;
		QAction *newSymbolAction = new QAction(icon,"",this) ;
		symbolAction->menu()->addAction(newSymbolAction) ;
		connect(newSymbolAction,SIGNAL(triggered()),this,SLOT(actionTriggered())) ;
		symbolActions << newSymbolAction ;
	}
	menu()->addAction(symbolAction) ;

	connect(lineColorAction,SIGNAL(triggered()),this,SLOT(actionTriggered())) ;
}

void changePlotStyleAction::execute()
{
	qDebug("triggered master action") ;
	specView *view = (specView*) parent() ;
	QModelIndexList items = view->getSelection() ;

	if (items.isEmpty()) return ;

	specStyleCommand *newCommand = new specLineColorCommand ;
	newCommand->setParentWidget(view) ;
	newCommand->setItems(items) ;
	library->push(newCommand) ;
}

void changePlotStyleAction::actionTriggered()
{
	qDebug("////////////style action");
	specView *view = (specView*) parent() ;
	if (sender() == lineColorAction)
	{
		qDebug("triggered slave action") ;
		qDebug() << lineColorAction << sender() ;
		specLineColorCommand *newCommand = new specLineColorCommand ;

		QColor color = QColorDialog::getColor() ;
		if (!color.isValid()) return ;

		newCommand->setParentWidget(view) ;
		newCommand->setItems(view->getSelection()) ;
		specModelItem *sample = view->model()->itemPointer(view->getSelection().first()) ;
		QPen pen = sample->pen() ;
		pen.setColor(color) ;
		sample->setPen(pen) ;
		newCommand->setStyle(sample) ;
		library->push(newCommand) ;
	}
	else if (symbolActions.contains((QAction*) sender()))
	{
		qDebug("symbol action triggered") ;
		specChangeSymbolCommand *newCommand = new specChangeSymbolCommand ;
		newCommand->setParentWidget(view) ;
		newCommand->setItems(view->getSelection()) ;
		specModelItem *sample = view->model()->itemPointer(view->getSelection().first()) ;
		sample->setSymbol(new QwtSymbol(QwtSymbol::Style(symbolActions.indexOf((QAction*) sender())),
										sample->brush(),sample->pen(),QSize(5,5))) ; // TODO: change
		newCommand->setStyle(sample) ;
		library->push(newCommand);
	}
}

