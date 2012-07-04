#include "changeplotstyleaction.h"
#include <QDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDebug>
#include "specgenealogy.h"
#include <QColorDialog>
#include <qwt_symbol.h>
#include <QPainter>
#include "specstylecommand.h"

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

//	specStyleCommand *newCommand = new specLineColorCommand ;
//	newCommand->setParentWidget(view) ;
//	newCommand->setItems(items) ;
//	library->push(newCommand) ;
}

void changePlotStyleAction::actionTriggered()
{
	qDebug("////////////style action");
	specView *view = (specView*) parent() ;
	specStyleCommand *newCommand = 0 ;
	QObject *source = sender() ;
	specDataItem item(QVector<specDataPoint>(),QHash<QString,specDescriptor>(),0,"") ;
	if (source == lineColorAction)
	{
		qDebug("new pen color command") ;
		newCommand = generateStyleCommand(spec::penColorId) ;
		QColor color = QColorDialog::getColor() ;
		if (!color.isValid()) return ;
		item.setPenColor(color) ;
	}
	else if (symbolActions.contains((QAction*) source))
	{
		qDebug("new symbol id") ;
		newCommand = generateStyleCommand(spec::symbolStyleId) ;
		qDebug("setting style of item %d", symbolActions.indexOf((QAction*) source) ) ;
		item.setSymbolStyle(QwtSymbol::Style(symbolActions.indexOf((QAction*) source))) ;
	}
	else return ;

	qDebug() << "commiting style" << &item ;
	newCommand->obtainStyle(&item) ;
	qDebug("committing action") ;
	newCommand->setParentWidget(view) ;
	newCommand->setItems(view->getSelection()) ;
	library->push(newCommand) ;
}

