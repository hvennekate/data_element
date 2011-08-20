#include "specplot.h"
#include <QTextStream>
#include <speclabeldialog.h>
#include <QInputDialog>
#include <qwt_text_label.h>
#include <qwt_scale_div.h>
#include "speckineticrange.h"
#include <qwt_symbol.h>

// TODO solve the myth of autoscaleaxis...

specPlot::specPlot(QWidget *parent)
	: canBeSelected(NULL), QwtPlot(parent), select(false)
{
	setAutoReplot(false) ;
	pick  = new CanvasPicker(this) ;
	zoom  = new specZoomer(this->canvas()) ;
	ranges = new QList<specCanvasItem*> ;
	ordinary = new QList<specCanvasItem*> ;
	kineticRanges = new QList<specCanvasItem*> ;
	selectRanges = new QList<specCanvasItem*> ;

	titleAction = new QAction(QIcon(":/changetitle.png"), tr("&Titel ändern..."), this);
	titleAction->setShortcut(tr("Ctrl+T"));
	titleAction->setStatusTip(tr("Change the plot's title"));
	connect(titleAction, SIGNAL(triggered()), this, SLOT(changeTitle())) ;
	
	xlabelAction = new QAction(QIcon(":/changexlabel.png"), tr("&x-Achsenbeschriftung ändern..."), this);
	xlabelAction->setShortcut(tr("Ctrl+X"));
	xlabelAction->setStatusTip(tr("Change the plot's x-label"));
	connect(xlabelAction, SIGNAL(triggered()), this, SLOT(changeXLabel())) ;
	
	ylabelAction = new QAction(QIcon(":/changeylabel.png"), tr("&y-Achsenbeschriftung ändern..."), this);
	ylabelAction->setShortcut(tr("Ctrl+Y"));
	ylabelAction->setStatusTip(tr("Change the plot's y-label"));
	connect(ylabelAction, SIGNAL(triggered()), this, SLOT(changeYLabel())) ;
	
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
	
	setupActions() ;
	
	contextMenu = new QMenu(this) ;
	contextMenu->addAction(titleAction) ;
	contextMenu->addAction(xlabelAction);
	contextMenu->addAction(ylabelAction);
	contextMenu->addAction(fixXAxisAction);
	contextMenu->addAction(fixYAxisAction);
	
	setAxisTitle(QwtPlot::yLeft,"<font face=\"Symbol\">D</font>mOD") ;
	setAxisTitle(QwtPlot::xBottom,"cm<sup>-1</sup>") ;
	refreshMoveMode() ;
}

CanvasPicker *specPlot::picker() { return pick ;}
specZoomer   *specPlot::zoomer() { return zoom ;}

void specPlot::changeTitle()
{
	bool ok = false ; // TODO: QTextEdit
	QString newTitle = QInputDialog::getText((QWidget*) this,tr("New plot title"),tr("New plot title:"), QLineEdit::Normal,title().text(),&ok) ;
	if (ok)
		setTitle(newTitle) ;
}

void specPlot::changeXLabel()
{
	bool ok = false ; // TODO: QTextEdit
	QString newTitle = QInputDialog::getText((QWidget*) this,tr("New x-axis label"),tr("New x-axis label:"), QLineEdit::Normal,title().text(),&ok) ;
	if (ok)
		setAxisTitle(QwtPlot::xBottom,newTitle) ;
}

void specPlot::changeYLabel()
{
	bool ok = false ; // TODO: QTextEdit
	QString newTitle = QInputDialog::getText((QWidget*) this,tr("New y-axis label"),tr("New y-axis label:"), QLineEdit::Normal,title().text(),&ok) ;
	if (ok)
		setAxisTitle(QwtPlot::yLeft,newTitle) ;
}

void specPlot::replot()
{
	QwtPlotItemList allItems = itemList();
	ranges->clear() ;
	ordinary->clear() ;
	kineticRanges->clear() ;
	selectRanges->clear();
	qDebug("rebuilding plot item pointer lists") ;
	foreach(QwtPlotItem* item, allItems)
	{
		if (dynamic_cast<specKineticRange*>(item))
			kineticRanges->append((specCanvasItem*) item) ;
		else if (dynamic_cast<specSelectRange*>(item))
			selectRanges->append((specCanvasItem*) item) ;
		else if (dynamic_cast<specRange*>(item))
			ranges->append((specCanvasItem*) item) ;
		else if (dynamic_cast<specCanvasItem*>( item))
			ordinary->append((specCanvasItem*) item) ;
	}
	if (allItems.isEmpty())
	{
		QwtPlot::replot() ;
		return ;
	}
	
	highlightSelectable() ;
	
	QRectF boundaries = allItems[0]->boundingRect() ; // DANGER: actually we must check, if the item plots on yLeft
	foreach(QwtPlotItem *item, allItems) // TODO omit this if fixing of axis is enabled
	{
		if(item->yAxis() == QwtPlot::yLeft)
		{
			QRectF bnd = item->boundingRect() ;
			boundaries.setLeft(qMin(bnd.left(),boundaries.left())) ;
			boundaries.setRight(qMax(bnd.right(),boundaries.right())) ;
			boundaries.setBottom(qMax(bnd.bottom(),boundaries.bottom())) ;
			boundaries.setTop(qMin(bnd.top(),boundaries.top())) ;
		}
// 		boundaries |= item->boundingRect() ;
	}
	
	double xOffset = boundaries.width()*.05, yOffset = boundaries.height()*.05, left, right, top, bottom ;
	if (fixXAxisAction->isChecked())
	{
		QRectF zoomBase = zoom->zoomBase() ;
		left = zoomBase.left() ;
		right = zoomBase.right() ;
	}
	else
	{
		left = qMin(boundaries.left(),boundaries.right()) - xOffset;
		right = qMax(boundaries.left(),boundaries.right()) + xOffset ;
	}
	
	if (fixYAxisAction->isChecked())
	{
		QRectF zoomBase = zoom->zoomBase() ;
		top = zoomBase.top() ;
		bottom = zoomBase.bottom() ;
	}
	else
	{
		top = qMin(boundaries.top(),boundaries.bottom()) - yOffset;
		bottom = qMax(boundaries.top(),boundaries.bottom()) + yOffset ;
	}
	
	boundaries.setLeft(left) ;
	boundaries.setRight(right) ;
	boundaries.setTop(top);
	boundaries.setBottom(bottom);//*/
	
	zoom->changeZoomBase(boundaries) ;
	QwtPlot::replot() ;
}

specPlot::~specPlot()
{
}

void specPlot::contextMenuEvent(QContextMenuEvent* event)
{
	if(!canvas()->geometry().contains(event->x(),event->y()))
	{
		contextMenu->exec(event->globalPos()) ;
		// axes and plot properties...
	}
	event->accept() ;
}

QList<QAction*> specPlot::actions()
{
	return (QList<QAction*>() << multipleCorrections << modifications->actions() << zeroCorrectionAction << zero->actions()) ;
}

void specPlot::setupActions()
{
	zero          = new QActionGroup(this) ;
	modifications = new QActionGroup(this) ;
	scaleAction           = new QAction(QIcon(":/scale.png"), tr("Spektrum skalieren"), modifications) ;
	offsetAction          = new QAction(QIcon(":/offset.png"),         tr("Spektrum verschieben (y)"), modifications) ;
	offlineAction         = new QAction(QIcon(":/offline.png"),        tr("Geradenkorrektur"), modifications) ;
	shiftXAction          = new QAction(QIcon(),                       tr("Shift X"), modifications) ;
	zeroCorrectionAction  = new QAction(QIcon(":/zeroCorrection.png"), tr("Automatische Geradenkorrektur"), this) ;
	addZeroRangeAction    = new QAction(QIcon(":/addZeroRange.png"),   tr("Neuer Nullbereich"), zero) ;
	deleteZeroRangeAction = new QAction(QIcon(":/deleteZeroRange.png"),tr("Aktuellen Nullbereich löschen"), zero) ;
	individualZeroAction  = new QAction(QIcon(":/individualZero.png"), tr("Nullbereiche auf unterster Ebene"), zero) ;
	multipleCorrections   = new QAction(QIcon(":/multiaction.png"), tr("Only one Correction at a time"), this) ;
	
	multipleCorrections->setCheckable(true) ; // TODO deselect if toggled if necessary
	multipleCorrections->setChecked(true) ;
	connect(multipleCorrections,SIGNAL(toggled(bool)),modifications,SLOT(setExclusive(bool))) ;
	
	foreach(QAction *pointer,QList<QAction*>() << modifications->actions() << zeroCorrectionAction << individualZeroAction)
	{
		pointer->setCheckable(true) ;
		pointer->setChecked  (false) ;
		connect(pointer, SIGNAL(triggered()), this, SLOT(refreshMoveMode())) ;
	}
	
	modifications->setExclusive(false) ;
	connect(zeroCorrectionAction, SIGNAL(toggled(bool)), modifications, SLOT(setDisabled(bool))) ;
	connect(zeroCorrectionAction, SIGNAL(toggled(bool)), multipleCorrections, SLOT(setDisabled(bool))) ;
	
	zero->setExclusive(false) ;
	zero->setEnabled(false) ;
	deleteZeroRangeAction->setEnabled(false) ;

	connect(zeroCorrectionAction, SIGNAL(toggled(bool)), zero, SLOT(setEnabled(bool))) ;
	
	connect(addZeroRangeAction,   SIGNAL(triggered()), this, SLOT(addRange())) ;
	connect(deleteZeroRangeAction,SIGNAL(triggered()), this, SLOT(deleteRange())) ;	

	refreshMoveMode() ;
}
// TODO fix axes via checkable actions
spec::moveMode specPlot::moveMode()
{ return mm ; }

void specPlot::refreshMoveMode()
{
	mm = spec::none | (scaleAction->isChecked()  ? spec::scale : spec::none)
			| (offsetAction->isChecked() ? spec::shift : spec::none)
			| (offlineAction->isChecked()? spec::slope : spec::none)
			| (shiftXAction->isChecked() ? spec::shiftX: spec::none)
			| (zeroCorrectionAction->isChecked() ? spec::newZero : spec::none)
			| (individualZeroAction->isChecked() ? spec::individualZero : spec::none) ;
		// TODO set highlighting (specCanvasItems) and selectability (canvasPicker)
	highlightSelectable(false) ;
	if ( mm & spec::newZero )
		canBeSelected = ranges ;
	else if ( mm ^ (mm & spec::individualZero) ) // TODO Highlighting
		canBeSelected = ordinary ;
	else canBeSelected = select ? selectRanges : kineticRanges ;
	
	if (selectable() != ranges)
		while(!ranges->isEmpty())
			delete ranges->takeLast() ;
	
	replot() ;
}

void specPlot::addRange()
{
	double min = axisScaleDiv(QwtPlot::xBottom)->lowerBound(), max = axisScaleDiv(QwtPlot::xBottom)->upperBound() ;
	ranges->append((specCanvasItem*) (new specRange(min-.1*(max-min),max+.1*(max-min)))) ;
	ranges->last()->attach(this) ;
	refreshRanges() ;
}

void specPlot::deleteRange()
{
	if(picker()->current() && ranges->indexOf(picker()->current()) >= 0)
		delete ranges->takeAt(ranges->indexOf(picker()->current())) ;
	else if(picker()->current() && selectRanges->indexOf(picker()->current()) >= 0)
		delete selectRanges->takeAt(selectRanges->indexOf(picker()->current())) ;
	else if(picker()->current() && kineticRanges->indexOf(picker()->current()) >= 0)
		delete kineticRanges->takeAt(kineticRanges->indexOf(picker()->current())) ;
	refreshRanges() ;
}

QList<specCanvasItem*>* specPlot::selectable()
{
	return canBeSelected ;
}

void specPlot::highlightSelectable(bool highlight)
{
	if (selectable()) // TODO put highlight check outside for loop for performance reasons
		foreach(specCanvasItem* pointer, *selectable())
			pointer->setSymbol ( highlight ? new QwtSymbol ( QwtSymbol::Ellipse,pointer->pen().brush(), ( QPen ) Qt::black, QSize ( 5,5 ) ) : 0 ) ;
}

void specPlot::refreshRanges()
{
	QList<QwtInterval*> rangeArray ;
	foreach(specCanvasItem* pointer, *ranges)
		rangeArray << (QwtInterval*) ((specRange*) pointer) ;
	foreach(specCanvasItem* pointer, *ordinary)
		pointer->applyRanges(rangeArray) ;
	deleteZeroRangeAction->setEnabled(ranges->size()) ;
	replot() ;
}
