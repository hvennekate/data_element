#include "changeplotstyleaction.h"
#include <QDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include "specgenealogy.h"
#include <QColorDialog>
#include <qwt_symbol.h>
#include <QPainter>
#include "specstylecommand.h"
#include <QInputDialog>
#include "specdataitem.h"

changePlotStyleAction::changePlotStyleAction(QObject *parent) :
	specRequiresItemAction(parent),
	currentTrigger(0)
{
	setIcon(QIcon(":/lineStyle.png"));
	setToolTip(tr("Plot Style")) ;
	setText(tr("Set plot style")) ;
	setWhatsThis(tr("Plot Style -- In this menu you will find commands for modifying the appearance of plots (colors, line widths, symbols).\nClick the \"triple circle\" icon at the top of a list for defining a color/size/width yourself."));
	QWidget *p = qobject_cast<QWidget*>(parent) ;
	setMenu(new QMenu(p)) ;
	lineColorMenu = new QMenu("Line color",menu()) ;
	symbolMenu = new QMenu("Symbol",menu()) ;
	symbolInnerColorMenu = new QMenu("Symbol Inner Color",menu()) ;
	symbolOuterColorMenu = new QMenu("Symbol Outer Color",menu()) ;
	symbolSizeMenu = new QMenu("Symbol Size",menu()) ;
	lineWidthMenu = new QMenu("Line Width",menu()) ;
	penStyleMenu = new QMenu("Line Style", menu()) ;
	menu()->addMenu(lineColorMenu) ;
	menu()->addMenu(lineWidthMenu) ;
	menu()->addMenu(symbolMenu) ;
	menu()->addMenu(symbolInnerColorMenu) ;
	menu()->addMenu(symbolOuterColorMenu) ;
	menu()->addMenu(symbolSizeMenu) ;
	menu()->addMenu(penStyleMenu) ;

	colors << Qt::black << Qt::red << Qt::green << Qt::blue << Qt::yellow << Qt::magenta << Qt::cyan << Qt::darkYellow ;
	sizes << 1 << 2 << 2.5 << 3 << 5 << 7 << 10 ;

	QSize iconSize(16,16) ;
	QPixmap icon(iconSize) ;
	QPainter painter(&icon) ;
	//	QPen pen ;
	QwtSymbol symbol ;
	symbol.setSize(iconSize) ;

	icon.fill(Qt::white);
	int l = icon.size().width()/5 ;
	painter.drawEllipse(l,l,l,l);
	painter.drawEllipse(2*l,2*l,l,l);
	painter.drawEllipse(3*l,3*l,l,l);
	QPixmap customPixmap(icon) ;

	QStringList symbolNames ;
	symbolNames << "No symbol"
		    << "Circle"
		    << "Square"
		    << "Diamond"
		    << "Triangle"
		    << "Inverted triangle"
		    << "Up triangle"
		    << "Left triangle"
		    << "Right triangle"
		    << "Cross"
		    << "XCross"
		    << "Horizontal line"
		    << "Vertical line"
		    << "Star 1"
		    << "Star 2"
		    << "Hexagon" ;
	for(int i = -1 ; i < QwtSymbol::Hexagon+1 ; ++i)
	{
		symbol.setStyle(QwtSymbol::Style(i)) ;
		icon.fill(Qt::white); // change to transparent
		symbol.drawSymbol(&painter,QPointF(iconSize.width()/2.,iconSize.height()/2.)) ;
		QAction *newSymbolAction = new QAction(icon,symbolNames[i+1],this) ;
		symbolActions << newSymbolAction ;
	}

	lineColorActions << new QAction(customPixmap,"",this) ;
	symbolInnerColorActions << new QAction(customPixmap,"",this) ;
	symbolOuterColorActions << new QAction(customPixmap,"",this) ;
	for (QVector<Qt::GlobalColor>::iterator i = colors.begin() ; i != colors.end() ; ++i)
	{
		icon.fill(*i) ;
		lineColorActions << new QAction(icon,"",this) ;
		symbolInnerColorActions << new QAction(icon,"",this) ;
		symbolOuterColorActions << new QAction(icon,"",this) ;
	}

	lineWidthActions << new QAction(customPixmap,"Custom line width...",this) ;
	symbolSizeActions << new QAction(customPixmap,"Custom symbol size...",this) ;
	for (QVector<double>::iterator i = sizes.begin() ; i != sizes.end() ; ++i)
	{
		lineWidthActions << new QAction("Line width " + QString::number(*i),this) ;
		symbolSizeActions << new QAction("Symbol size " + QString::number(*i),this) ;
	}

	penStyleActions << new QAction("No line",this) <<
			   new QAction("Solid line", this) <<
			   new QAction("Dashed line", this) <<
			   new QAction("Dotted line", this) <<
			   new QAction("Dash-dotted line", this) <<
			   new QAction("Dash-dot-dotted line", this) ;

	symbolMenu->addActions(symbolActions.toList());
	lineColorMenu->addActions(lineColorActions.toList()) ;
	lineWidthMenu->addActions(lineWidthActions.toList()) ;
	symbolInnerColorMenu->addActions(symbolInnerColorActions.toList()) ;
	symbolOuterColorMenu->addActions(symbolOuterColorActions.toList());
	symbolSizeMenu->addActions(symbolSizeActions.toList());
	penStyleMenu->addActions(penStyleActions.toList()) ;

	connect(symbolMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*))) ;
	connect(lineColorMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*))) ;
	connect(lineWidthMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*))) ;
	connect(symbolInnerColorMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*))) ;
	connect(symbolOuterColorMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*))) ;
	connect(symbolSizeMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*))) ;
	connect(penStyleMenu, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*))) ;
}

QColor changePlotStyleAction::getColor(int index)
{
	if(!index) return QColorDialog::getColor() ;
	return colors[index-1] ;
}

double changePlotStyleAction::getSize(int index, bool &ok)
{
	if (!index) return QInputDialog::getDouble(0,"Value?","Value:",1,0,100,1,&ok) ;
	return sizes[index-1] ;
}

void changePlotStyleAction::actionTriggered(QAction* action)
{
	currentTrigger = sender() ;
	currentAction = action ;
	execute() ;
}

specUndoCommand* changePlotStyleAction::generateUndoCommand()
{
	specStyleCommand *newCommand = 0 ;
	if (!currentTrigger || !currentAction) return 0 ;

	specDataItem item ;
	if (currentTrigger == lineColorMenu)
	{
		QColor color = getColor(lineColorActions.indexOf(currentAction)) ;
		if (!color.isValid()) return 0;
		newCommand = generateStyleCommand(specStreamable::penColorCommandId) ;
		item.setPenColor(color) ;
		newCommand->setText(tr("Change line color"));
	}
	if (currentTrigger == symbolInnerColorMenu)
	{
		QColor color = getColor(symbolInnerColorActions.indexOf(currentAction)) ;
		if (!color.isValid()) return 0;
		newCommand = generateStyleCommand(specStreamable::symbolBrushColorCommandId) ;
		item.setSymbolBrushColor(color) ;
		newCommand->setText(tr("Change symbol inner color"));
	}
	if (currentTrigger == symbolOuterColorMenu)
	{
		QColor color = getColor(symbolOuterColorActions.indexOf(currentAction)) ;
		if (!color.isValid()) return 0;
		newCommand = generateStyleCommand(specStreamable::symbolPenColorCommandId) ;
		item.setSymbolPenColor(color) ;
		newCommand->setText(tr("Change symbol outer color"));
	}

	bool ok = true;
	if (currentTrigger == lineWidthMenu)
	{
		double value = getSize(lineWidthActions.indexOf(currentAction), ok) ;
		if (!ok) return 0;
		newCommand = generateStyleCommand(specStreamable::lineWidthCommandId) ;
		item.setLineWidth(value) ;
		newCommand->setText(tr("Change line width"));
	}
	if (currentTrigger == symbolSizeMenu)
	{
		double value = getSize(symbolSizeActions.indexOf(currentAction), ok) ;
		if (!ok) return 0;
		newCommand = generateStyleCommand(specStreamable::symbolSizeCommandId) ;
		item.setSymbolSize(value) ;
		newCommand->setText(tr("Change symbol size"));
	}

	if (currentTrigger == symbolMenu)
	{
		newCommand = generateStyleCommand(specStreamable::symbolStyleCommandId) ;
		int symbolStyle = symbolActions.indexOf(currentAction) - 1 ;
		//        if (symbolStyle < 0)
		//            item.setSymbolStyle(specPlotStyle::noSymbol);
		//        else
		item.setSymbolStyle(symbolStyle);
		newCommand->setText(tr("Change symbol type"));
	}

	if (currentTrigger == penStyleMenu)
	{
		newCommand = generateStyleCommand(specStreamable::penStyleCommandId) ;
		item.setPenStyle(penStyleActions.indexOf(currentAction));
		newCommand->setText(tr("Change line style"));
	}

	if (!newCommand) return 0;
	newCommand->obtainStyle(&item) ;
	newCommand->setParentObject(model) ;
	newCommand->setItems(pointers) ;
	return newCommand ;
}

