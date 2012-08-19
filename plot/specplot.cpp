#include "specplot.h"
#include "speccanvasitem.h"
#include "speczoomer.h"
#include <QAction>
#include "canvaspicker.h"
#include "actionlib/specactionlibrary.h"
#include "specview.h"

// TODO solve the myth of autoscaleaxis...
// TODO remove kineticRanges

specPlot::specPlot(QWidget *parent)
	: QwtPlot(parent),
	  replotting(false),
	  MetaPicker(0),
	  SVGpicker(0),
	  undoP(0),
	  view(0)
{
	MetaPicker = new CanvasPicker(this) ;
	SVGpicker  = new CanvasPicker(this) ;
	modifySVGs    = new QAction("Modify SVGs",this) ;
	modifySVGs->setCheckable(true) ;
	connect(modifySVGs,SIGNAL(triggered(bool)),this,SLOT(modifyingSVGs(bool))) ;

	connect(metaPicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this, SIGNAL(metaRangeModified(specCanvasItem*,int,double,double))) ;
	setAutoReplot(false) ;
	zoom  = new specZoomer(this->canvas()) ;

	fixYAxisAction = new QAction(QIcon(":/fixyaxis.png"), tr("&y-Achsenskalierung fixieren"), this);
	fixYAxisAction->setShortcut(tr("Ctrl+y"));
	fixYAxisAction->setStatusTip(tr("Fix y axis scaling"));
	fixYAxisAction->setCheckable(true) ;
	fixYAxisAction->setChecked(false) ;
	
	fixXAxisAction = new QAction(QIcon(":/fixxaxis.png"), tr("&x-Achsenskalierung fixieren"), this);
	fixXAxisAction->setShortcut(tr("Ctrl+x"));
	fixXAxisAction->setStatusTip(tr("Fix x axis scaling"));
	fixXAxisAction->setCheckable(true) ;
	fixXAxisAction->setChecked(false) ;

	printAction = new specPrintPlotAction(this) ;
	
	setupActions() ;
	
	setAxisTitle(QwtPlot::yLeft,"<font face=\"Symbol\">D</font>mOD") ;
	setAxisTitle(QwtPlot::xBottom,"cm<sup>-1</sup>") ;
	refreshMoveMode() ;
}

specZoomer *specPlot::zoomer()
{
	return zoom ;
}

void specPlot::replot()
{
	if (replotting) return ;
	replotting = true ;
	emit startingReplot();
	QwtPlotItemList allItems = itemList();
	ranges->clear() ;
	ordinary->clear() ;
	selectRanges->clear();
	QSet<specCanvasItem*> newMetaRanges; // TODO local variable
	QVector<specSVGItem*> svgitems ;
	foreach(QwtPlotItem* item, allItems)
	{
		if (dynamic_cast<specMetaRange*>(item))
			newMetaRanges += ((specCanvasItem*) item) ;
		else if (dynamic_cast<specSelectRange*>(item))
			selectRanges->append((specCanvasItem*) item) ;
		else if (dynamic_cast<specRange*>(item))
			ranges->append((specCanvasItem*) item) ;
		else if (dynamic_cast<specSVGItem*>(item))
			svgitems << (specSVGItem*) item ;
		else if (dynamic_cast<specCanvasItem*>( item))
			ordinary->append((specCanvasItem*) item) ;
	}
	metaPicker->setSelectable(newMetaRanges);
	if (allItems.isEmpty())
	{
		QwtPlot::replot() ;
		emit replotted() ;
		replotting = false ;
		return ;
	}
	
	QRectF boundaries, zoomBase = zoom->zoomBase() ;
	specModelItem *pointer = 0 ; // TODO find a more concise version.
	foreach(QwtPlotItem *item, allItems)
	{
		if((pointer = dynamic_cast<specModelItem*>(item)))
		{
			pointer->revalidate();
			boundaries |= pointer->boundingRect() ;
		}
	}

	boundaries = boundaries.normalized() ;
	boundaries.translate(-.05*boundaries.width(),-.5*boundaries.height()) ;
	boundaries.setWidth(1.1*boundaries.width());
	boundaries.setHeight(1.1*boundaries.height());
	double xOffset = boundaries.width()*.05, yOffset = boundaries.height()*.05, left, right, top, bottom ;
	if (fixXAxisAction->isChecked())
	{
		boundaries.setLeft(zoomBase.left()) ;
		boundaries.setRight(zoomBase.right());
	}
	if (fixYAxisAction->isChecked())
	{
		boundaries.setTop(zoomBase.top()) ;
		boundaries.setBottom(zoomBase.bottom()) ;
	}
	
	zoom->changeZoomBase(boundaries.isValid() ? boundaries : QRectF(-10,-10,20,20)) ;

	QwtPlot::replot() ; // TODO optimize
	foreach(specSVGItem* svgitem, svgitems)
		svgitem->refreshSVG() ;
	QwtPlot::replot() ;
	emit replotted();
	replotting = false ;
}

specPlot::~specPlot()
{
}

QList<QAction*> specPlot::actions()
{
	return (QList<QAction*>() << modifySVGs << printAction << fixXAxisAction << fixYAxisAction ) ;
}

void specPlot::writeToStream(QDataStream &out) const
{
	out << title().text()
	    << axisTitle(QwtPlot::xBottom).text()
		<< axisTitle(QwtPlot::yLeft).text()
		<< axisTitle(QwtPlot::xTop).text()
		<< axisTitle(QwtPlot::yRight).text() ;
}

void specPlot::readFromStream(QDataStream &in)
{
	QString Title, xlabel, ylabel, x2label, y2label ;
	in >> Title >> xlabel >> ylabel >> x2label >> y2label ;
	setTitle(Title) ;
	setAxisTitle(QwtPlot::xBottom, xlabel) ;
	setAxisTitle(QwtPlot::yLeft, ylabel) ;
	setAxisTitle(QwtPlot::xTop, x2label) ;
	setAxisTitle(QwtPlot::yRight, y2label) ;
}

void specPlot::resizeSVG(specCanvasItem *item, int point, double x, double y)
{
	specSVGItem *pointer = dynamic_cast<specSVGItem*>(item) ;
	if (!pointer || !view || !undoPartner()) return ;

	specSVGItem::bounds oldBounds = pointer->getBounds() ;
	pointer->pointMoved(point,x,y) ;

	specResizeSVGcommand *command = new specResizeSVGcommand ;
	command->setParentObject(view->model()) ;
	command->setItem(view->model()->index(pointer),oldBounds) ;
	undoPartner()->push(command) ;
}

void specPlot::modifyingSVGs(const bool &modify)
{
	if (SVGpicker)
	{
		disconnect(SVGpicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)),this,SLOT(resizeSVG(specCanvasItem*,int,double,double))) ;
		delete SVGpicker ;
		SVGpicker = 0 ;
	}
	if (!modify) return ;

	SVGpicker = new CanvasPicker(this) ;
	QList<specCanvasItem*> SVGitems ;

	foreach(QwtPlotItem *item, itemList())
		SVGitems << dynamic_cast<specSVGItem*>(item) ;
	SVGitems.removeAll(0) ;
	SVGpicker->addSelectable(SVGitems.toSet()) ;
	connect(SVGpicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)),this,SLOT(resizeSVG(specCanvasItem*,int,double,double))) ;
}

void specPlot::setUndoPartner(specActionLibrary *lib)
{
	undoP = lib ;
	((specUndoAction*) printAction)->setLibrary(lib);
}

specActionLibrary* specPlot::undoPartner() const
{
	return undoP ;
}

void specPlot::setView(specView *mod)
{
	view = mod ;
}
