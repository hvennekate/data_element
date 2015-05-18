#include "specplot.h"
#include "speccanvasitem.h"
#include "speczoomer.h"
#include <QAction>
#include "canvaspicker.h"
#include "specactionlibrary.h"
#include "specview.h"
#include "specprintplotaction.h"
#include "specplotexportaction.h"
#include "specsvgitem.h"
#include "specmetaitem.h"
#include <qwt_scale_widget.h>
#include <QApplication>
#include <qwt_legend.h>
#include <QMouseEvent>
#include <qwt_plot_marker.h>
#include "specfitcurve.h"
#include <qwt_plot_layout.h>
#include <cmath>

// TODO solve the myth of autoscaleaxis...

QwtPlotMarker* specPlot::initializeZeroLine()
{
	QwtPlotMarker* zeroLine = new QwtPlotMarker ;
	zeroLine->setLinePen(QPen(Qt::DotLine)) ;
	zeroLine->attach(this) ;
	return zeroLine ;
}

specPlot::specPlot(QWidget* parent)
	: QwtPlot(parent),
	  replotting(false),
	  MetaPicker(0),
	  SVGpicker(0),
	  zeroYLine(0),
	  zeroXLine(0),
	  autoScaling(true),
	  undoP(0),
	  xminEdit(this),
	  xmaxEdit(this),
	  yminEdit(this),
	  ymaxEdit(this),
	  view(0)
{
	setAutoDelete(false);
	zeroYLine = initializeZeroLine() ;
	zeroYLine->setLineStyle(QwtPlotMarker::HLine);
	zeroXLine = initializeZeroLine() ;
	zeroXLine->setLineStyle(QwtPlotMarker::VLine);

	MetaPicker = new CanvasPicker(this) ;
	SVGpicker  = new CanvasPicker(this) ;
	modifySVGs    = new QAction(QIcon(":/resizeImage.png"), "Modify SVGs", this) ;
	modifySVGs->setToolTip(tr("Resize image items")) ;
	modifySVGs->setWhatsThis(tr("Modify the size of images displayed on the plot's canvas.")) ;
	modifySVGs->setCheckable(true) ;

	connect(MetaPicker, SIGNAL(pointMoved(specCanvasItem*, int, double, double)), this, SIGNAL(metaRangeModified(specCanvasItem*, int, double, double))) ;
	setAutoReplot(false) ;
	zoom  = new specZoomer(this->canvas()) ;

	fixYAxisAction = new QAction(QIcon(":/fixyaxis.png"), tr("Fixate &y axis"), this);
	fixYAxisAction->setShortcut(tr("Ctrl+Shift+y"));
	fixYAxisAction->setWhatsThis(tr("Disables auto scaling for the y axis and fixates the current axis range."));
	fixYAxisAction->setIcon(QIcon(":/fixYAxis.png")) ;
	fixYAxisAction->setCheckable(true) ;
	fixYAxisAction->setChecked(false) ;

	fixXAxisAction = new QAction(QIcon(":/fixxaxis.png"), tr("Fixate &x axis"), this);
	fixXAxisAction->setShortcut(tr("Ctrl+Shift+x"));
	fixXAxisAction->setWhatsThis(tr("Disables auto scaling for the y axis and fixates the current axis range.")) ;
	fixXAxisAction->setIcon(QIcon(":/fixXAxis.png")) ;
	fixXAxisAction->setCheckable(true) ;
	fixXAxisAction->setChecked(false) ;

	printAction = new specPrintPlotAction(this) ;
	exportAction = new specPlotExportAction(this) ;
	legendAction = new QAction(QIcon(":/legend.png"), tr("Legend"), this) ;
	legendAction->setToolTip(tr("Legend")) ;
	legendAction->setWhatsThis(tr("Turn legend on/off")) ;
	legendAction->setCheckable(true) ;
	legendAction->setChecked(false) ;
	connect(legendAction, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool))) ;

	//	setAxisTitle(QwtPlot::yLeft,"<p><font face=\"Symbol\">D</font>mOD</p>") ;
	//	setAxisTitle(QwtPlot::xBottom,"cm<sup>-1</sup>") ;

	QList<QLineEdit*> lineEdits ;
	lineEdits << &yminEdit << &ymaxEdit << &xminEdit << &xmaxEdit ;
	foreach(QLineEdit * lineEdit, lineEdits)
	{
		connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(setPlotAxis())) ;
		connect(lineEdit, SIGNAL(editingFinished()), lineEdit, SLOT(hide())) ;
		lineEdit->hide();
		lineEdit->setFrame(false);
		lineEdit->adjustSize();
	}
	zoom->changeZoomBase(QRectF(-10, -10, 20, 20));

	// Standard legend
	showLegend(legendAction->isChecked()) ;
}

void specPlot::showLegend(bool l)
{
	if(!plotLayout()) return ;
	if(l)
	{
		QwtLegend* newLegend = new QwtLegend(this) ;
		newLegend->setDefaultItemMode(QwtLegendData::Checkable) ;
		insertLegend(newLegend) ;
		//		connect(this, SIGNAL(legendChecked(QwtPlotItem*,bool)), SLOT(toggleItem(QwtPlotItem*,bool))) ;
		connect(newLegend, SIGNAL(checked(QVariant, bool, int)), this, SLOT(toggleItem(QVariant, bool))) ;
	}
	else
		insertLegend(0);
}

void specPlot::updateLegend()
{
	if (!legend()) return ;
	showLegend(true) ;
}

void specPlot::toggleItem(const QVariant& v, bool on)
{
	QwtPlotItem* item = infoToItem(v) ;
	if(item) item->setVisible(!on) ;
	replot() ;
}

void specPlot::toggleItem(QwtPlotItem* item, bool invisible)
{
	if(!item) return ;
	item->setVisible(!invisible) ;
	replot() ;
}

void specPlot::setAutoScaling(bool on)
{
	autoScaling = on ;
}

void specPlot::haltPlotting(bool b)
{
	replotting = b ;
	if (!replotting) replot() ;
}

void specPlot::replot()
{
	if(replotting) return ;
	replotting = true ;
	emit startingReplot();
	QwtPlotItemList allItems = itemList();
	QSet<specCanvasItem*> newMetaRanges; // TODO local variable
	QVector<specSVGItem*> svgitems ;
	foreach(QwtPlotItem * item, allItems)
	{
		if(dynamic_cast<specMetaRange*>(item))
			newMetaRanges << ((specCanvasItem*) item) ;
		else if(dynamic_cast<specSVGItem*>(item))
			svgitems << (specSVGItem*) item ;
	}

	if(allItems.isEmpty())
	{
		QwtPlot::replot() ;
		emit replotted() ;
		replotting = false ;
		return ;
	}

	autoScale(allItems) ;

	QwtPlot::replot() ; // TODO optimize
	foreach(specSVGItem * svgitem, svgitems)
	svgitem->refreshSVG() ;
	QwtPlot::replot() ;
	emit replotted();
	replotting = false ;
}

void specPlot::autoScale(const QwtPlotItemList& allItems)
{
	if(!autoScaling) return ;
	QRectF boundaries, zoomBase = zoom->zoomBase() ;
	specModelItem* pointer = 0 ; // TODO find a more concise version.
	bool firstItem = true ;
	foreach(QwtPlotItem * item, allItems)
	{
		if(!(dynamic_cast<specSVGItem*>(item))
			//				&& !(dynamic_cast<specRange*>(item))
			&& !(dynamic_cast<QwtPlotSvgItem*>(item))
			&& !(dynamic_cast<specFitCurve*>(item))
			&& item != zeroYLine
			&& item != zeroXLine)
		{
			if((pointer = dynamic_cast<specModelItem*>(item)))
				pointer->revalidate() ;
			QRectF br = item->boundingRect() ;
			if(br.isValid() || (std::isnormal(br.width()) && std::isnormal(br.height())))  // TODO scale axes independently
				boundaries |= br ;
			else
			{
				QPointF p = br.topLeft() ;
				if(!boundaries.contains(br) && !isnan(p.x()) && !isnan(p.y()))
				{
					if(firstItem)
						boundaries.moveTopLeft(p) ;
					else
					{
						if(p.y() > boundaries.bottom()) boundaries.setBottom(p.y()) ;
						else if(p.y() < boundaries.top()) boundaries.setTop(p.y()) ;
						if(p.x() < boundaries.left()) boundaries.setLeft(p.x()) ;
						else if(p.x() > boundaries.right()) boundaries.setRight(p.x()) ;
					}
				}
			}
			firstItem = false ;
		}
		//		if (specMetaRange* r = dynamic_cast<specMetaRange*>(item))
		//		{
		//			boundaries.setLeft(qMin(boundaries.left(),r->minValue()));
		//			boundaries.setRight(qMax(boundaries.right(), r->maxValue()));
		//		}
	}

	boundaries = boundaries.normalized() ;
	if(!boundaries.width())
	{
		boundaries.moveLeft(boundaries.left() - 5. / 1.1);
		boundaries.setWidth(10) ;
	}
	else
		boundaries.setWidth(1.1 * boundaries.width());

	if(!boundaries.height())
	{
		boundaries.moveTop(boundaries.top() - 5. / 1.1) ;
		boundaries.setHeight(10) ;
	}
	else
		boundaries.setHeight(1.1 * boundaries.height()) ;

	boundaries.translate(-.05 / 1.1 * boundaries.width(), -.05 / 1.1 * boundaries.height()) ;
	if(fixXAxisAction->isChecked())
	{
		boundaries.setLeft(zoomBase.left()) ;
		boundaries.setRight(zoomBase.right());
	}
	if(fixYAxisAction->isChecked())
	{
		boundaries.setTop(zoomBase.top()) ;
		boundaries.setBottom(zoomBase.bottom()) ;
	}

	//	if (boundaries.width() <= 0)
	//	{
	//		boundaries.setLeft(-10);
	//		boundaries.setWidth(20) ;
	//	}
	//	if (boundaries.height() <= 0)
	//	{
	//		boundaries.setTop(-10);
	//		boundaries.setHeight(20);
	//	}
	zoom->changeZoomBase(boundaries) ;
}

specPlot::~specPlot()
{
	delete zeroYLine ;
	delete zeroXLine ;
	MetaPicker->purgeSelectable();
	SVGpicker->purgeSelectable();
	delete MetaPicker ;
	delete SVGpicker ;
}

QList<QAction*> specPlot::actions()
{
	return (QList<QAction*>() << modifySVGs << printAction << exportAction << fixXAxisAction << fixYAxisAction << legendAction) ;
}

void specPlot::writeToStream(QDataStream& out) const
{
	out << title().text()
	    << axisTitle(QwtPlot::xBottom).text()
	    << axisTitle(QwtPlot::yLeft).text()
	    << axisTitle(QwtPlot::xTop).text()
	    << axisTitle(QwtPlot::yRight).text() ;
}

void specPlot::readFromStream(QDataStream& in)
{
	QString Title, xlabel, ylabel, x2label, y2label ;
	in >> Title >> xlabel >> ylabel >> x2label >> y2label ;
	setTitle(Title) ;
	setAxisTitle(QwtPlot::xBottom, xlabel) ;
	setAxisTitle(QwtPlot::yLeft, ylabel) ;
	setAxisTitle(QwtPlot::xTop, x2label) ;
	setAxisTitle(QwtPlot::yRight, y2label) ;
}

void specPlot::setUndoPartner(specActionLibrary* lib)
{
	undoP = lib ;
	((specUndoAction*) printAction)->setLibrary(lib);
	((specUndoAction*) exportAction)->setLibrary(lib);
}

specActionLibrary* specPlot::undoPartner() const
{
	return undoP ;
}

void specPlot::setView(specView* mod)
{
	if (view && view->model())
		disconnect(view->model(), 0, this, 0) ;
	view = mod ;
	if (view && view->model())
		connect(view->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateLegend())) ;
}

CanvasPicker* specPlot::metaPicker()
{
	return MetaPicker ;
}

CanvasPicker* specPlot::svgPicker()
{
	return SVGpicker ;
}

void specPlot::resizeEvent(QResizeEvent* e)
{
	QwtPlot::resizeEvent(e) ;
	replot() ;
}

void specPlot::attachToPicker(specCanvasItem* i)
{
	Q_UNUSED(i)
}

void specPlot::detachFromPicker(specCanvasItem* i)
{
	Q_UNUSED(i)
}

QAction* specPlot::svgAction() const
{
	return modifySVGs ;
}

void specPlot::mouseDoubleClickEvent(QMouseEvent* e)
{
	QWidget* child = childAt(e->pos()) ;
	QwtScaleWidget* xAxisWidget = axisWidget(QwtPlot::xBottom) ;
	QwtScaleWidget* yAxisWidget = axisWidget(QwtPlot::yLeft) ;

	xmaxEdit.setText(QString::number(zoom->zoomBase().right())) ;
	xminEdit.setText(QString::number(zoom->zoomBase().left()));
	ymaxEdit.setText(QString::number(zoom->zoomBase().bottom())) ;
	yminEdit.setText(QString::number(zoom->zoomBase().top()));

	ymaxEdit.selectAll();
	xmaxEdit.selectAll();
	yminEdit.selectAll();
	xminEdit.selectAll();

	if(child == (QWidget*) xAxisWidget && fixXAxisAction->isChecked())
	{
		QRect geom = xAxisWidget->geometry() ;
		if((e->pos() - xAxisWidget->pos()).x() < xAxisWidget->width() / 2)
		{
			geom.setWidth(qMin(xminEdit.width(), geom.width()));
			geom.moveLeft(xAxisWidget->geometry().left());
			xminEdit.setGeometry(geom) ;
			xminEdit.show();
			connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), &xminEdit, SLOT(hideAfterEditing(QWidget*, QWidget*))) ;
			xminEdit.setFocus();
		}
		else
		{
			geom.setWidth(qMin(xmaxEdit.geometry().width(), geom.width()));
			geom.moveRight(xAxisWidget->geometry().right());
			xmaxEdit.setGeometry(geom);
			xmaxEdit.show();
			connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), &xmaxEdit, SLOT(hideAfterEditing(QWidget*, QWidget*))) ;
			xmaxEdit.setFocus();
		}
	}
	if(child == (QWidget*) yAxisWidget && fixYAxisAction->isChecked())
	{
		QRect geom = yAxisWidget->geometry() ;
		if((e->pos() - yAxisWidget->pos()).y() < yAxisWidget->height() / 2)
		{
			geom.setHeight(qMin(ymaxEdit.height(), geom.height()));
			geom.moveTop(yAxisWidget->geometry().top());
			ymaxEdit.setGeometry(geom) ;
			ymaxEdit.show();
			connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), &ymaxEdit, SLOT(hideAfterEditing(QWidget*, QWidget*))) ;
			ymaxEdit.setFocus();
		}
		else
		{
			geom.setHeight(qMin(yminEdit.geometry().height(), geom.height()));
			geom.moveBottom(yAxisWidget->geometry().bottom());
			yminEdit.setGeometry(geom);
			yminEdit.show();
			connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), &yminEdit, SLOT(hideAfterEditing(QWidget*, QWidget*))) ;
			yminEdit.setFocus();
		}
	}
}

void specPlot::setPlotAxis()
{
	plotAxisEdit* edit = qobject_cast<plotAxisEdit*> (sender()) ;
	if(!edit) return ;
	QDoubleValidator dv ;
	QString text = edit->text() ;
	int pos = 0 ;
	if(dv.validate(text, pos) != QValidator::Acceptable) return ;

	double value = text.toDouble() ;
	QRectF zoomBase = zoom->zoomBase() ;
	if(edit == &yminEdit && value < zoomBase.bottom())
		zoomBase.setTop(value) ;
	if(edit == &ymaxEdit && value > zoomBase.top())
		zoomBase.setBottom(value) ;
	if(edit == &xminEdit && value < zoomBase.right())
		zoomBase.setLeft(value);
	if(edit == &xmaxEdit && value > zoomBase.left())
		zoomBase.setRight(value) ;
	zoom->changeZoomBase(zoomBase);
}
