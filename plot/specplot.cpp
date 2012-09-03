#include "specplot.h"
#include "speccanvasitem.h"
#include "speczoomer.h"
#include <QAction>
#include "canvaspicker.h"
#include "actionlib/specactionlibrary.h"
#include "specview.h"
#include "actionlib/specprintplotaction.h"
#include "specsvgitem.h"
#include "specmetaitem.h"

// TODO solve the myth of autoscaleaxis...

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
	modifySVGs    = new QAction(QIcon(":/resizeImage.png"),"Modify SVGs",this) ;
	modifySVGs->setToolTip(tr("Resize image items")) ;
	modifySVGs->setWhatsThis(tr("Modify the size of images displayed on the plot's canvas.")) ;
	modifySVGs->setCheckable(true) ;

	connect(MetaPicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this, SIGNAL(metaRangeModified(specCanvasItem*,int,double,double))) ;
	setAutoReplot(false) ;
	zoom  = new specZoomer(this->canvas()) ;

	fixYAxisAction = new QAction(QIcon(":/fixyaxis.png"), tr("fixate &y axis"), this);
	fixYAxisAction->setShortcut(tr("Ctrl+y"));
	fixYAxisAction->setWhatsThis(tr("Disables auto scaling for the y axis and fixates the current axis range."));
	fixYAxisAction->setIcon(QIcon(":/fixYAxis.png")) ;
	fixYAxisAction->setCheckable(true) ;
	fixYAxisAction->setChecked(false) ;
	
	fixXAxisAction = new QAction(QIcon(":/fixxaxis.png"), tr("fixate &x axis"), this);
	fixXAxisAction->setShortcut(tr("Ctrl+x"));
	fixXAxisAction->setWhatsThis(tr("Disables auto scaling for the y axis and fixates the current axis range.")) ;
	fixXAxisAction->setIcon(QIcon(":/fixXAxis.png")) ;
	fixXAxisAction->setCheckable(true) ;
	fixXAxisAction->setChecked(false) ;

	printAction = new specPrintPlotAction(this) ;

	setAxisTitle(QwtPlot::yLeft,"<font face=\"Symbol\">D</font>mOD") ;
	setAxisTitle(QwtPlot::xBottom,"cm<sup>-1</sup>") ;
}

void specPlot::replot()
{
	if (replotting) return ;
	replotting = true ;
	emit startingReplot();
	QwtPlotItemList allItems = itemList();
	QSet<specCanvasItem*> newMetaRanges; // TODO local variable
	QVector<specSVGItem*> svgitems ;
	foreach(QwtPlotItem* item, allItems)
	{
		if (dynamic_cast<specMetaRange*>(item))
			newMetaRanges += ((specCanvasItem*) item) ;
		else if (dynamic_cast<specSVGItem*>(item))
			svgitems << (specSVGItem*) item ;
	}

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
		if(!(dynamic_cast<specSVGItem*>(item))
				&& (pointer = dynamic_cast<specModelItem*>(item)))
		{
			pointer->revalidate() ;
			if (pointer->boundingRect().isValid())
				boundaries |= pointer->boundingRect() ;
		}
	}

	boundaries = boundaries.normalized() ;
	boundaries.translate(-.05*boundaries.width(),-.05*boundaries.height()) ;
	boundaries.setWidth(1.1*boundaries.width());
	boundaries.setHeight(1.1*boundaries.height());
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

CanvasPicker* specPlot::metaPicker()
{
	return MetaPicker ;
}

CanvasPicker* specPlot::svgPicker()
{
	return SVGpicker ;
}

void specPlot::resizeEvent(QResizeEvent *e)
{
	QwtPlot::resizeEvent(e) ;
	replot() ;
}

void specPlot::attachToPicker(specCanvasItem *i)
{
	Q_UNUSED(i)
}

void specPlot::detachFromPicker(specCanvasItem *i)
{
	Q_UNUSED(i)
}

QAction* specPlot::svgAction() const
{
	return modifySVGs ;
}
