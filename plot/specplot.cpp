#include "specplot.h"
#include <QTextStream>
#include <QInputDialog>
#include <qwt_text_label.h>
#include <qwt_scale_div.h>
#include <qwt_symbol.h>
#include <textEditor/specsimpletextedit.h>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "specmetaitem.h"
#include "specsvgitem.h"
#include "specmetarange.h"
#include "specsvgitem.h"
#include "actionlib/specresizesvgcommand.h"
#include "actionlib/specprintplotaction.h"

// TODO solve the myth of autoscaleaxis...
// TODO remove kineticRanges

specPlot::specPlot(QWidget *parent)
	: QwtPlot(parent),
	  replotting(false),
	  canBeSelected(NULL),
	  metaPicker(new CanvasPicker(this)),
	  textEdit(new specSimpleTextEdit(this)),
	  select(false),
	  SVGpicker(0),
	  undoP(0),
	  view(0)
{
	modifySVGs    = new QAction("Modify SVGs",this) ;
	modifySVGs->setCheckable(true) ;
	connect(modifySVGs,SIGNAL(triggered(bool)),this,SLOT(modifyingSVGs(bool))) ;

	connect(metaPicker,SIGNAL(pointMoved(specCanvasItem*,int,double,double)), this, SIGNAL(metaRangeModified(specCanvasItem*,int,double,double))) ;
	setAutoReplot(false) ;
	zoom  = new specZoomer(this->canvas()) ;
	ranges = new QList<specCanvasItem*> ;
	ordinary = new QList<specCanvasItem*> ;
	kineticRanges = new QList<specCanvasItem*> ;
	selectRanges = new QList<specCanvasItem*> ;
	textEdit->hide();

	titleAction = new QAction(QIcon(":/changetitle.png"), tr("&Titel ändern..."), this);
	titleAction->setShortcut(tr("Ctrl+T"));
	titleAction->setStatusTip(tr("Change the plot's title"));
	connect(titleAction, SIGNAL(triggered()), this, SLOT(changeTextLabel())) ;
	
	xlabelAction = new QAction(QIcon(":/changexlabel.png"), tr("&x-Achsenbeschriftung ändern..."), this);
	xlabelAction->setShortcut(tr("Ctrl+X"));
	xlabelAction->setStatusTip(tr("Change the plot's x-label"));
	connect(xlabelAction, SIGNAL(triggered()), this, SLOT(changeTextLabel())) ;
	
	ylabelAction = new QAction(QIcon(":/changeylabel.png"), tr("&y-Achsenbeschriftung ändern..."), this);
	ylabelAction->setShortcut(tr("Ctrl+Y"));
	ylabelAction->setStatusTip(tr("Change the plot's y-label"));
	connect(ylabelAction, SIGNAL(triggered()), this, SLOT(changeTextLabel())) ;
	
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

specZoomer *specPlot::zoomer()
{
	return zoom ;
}

void specPlot::changeTitle(QString newTitle)
{
		setTitle(newTitle) ;
}

void specPlot::changeXLabel(QString newTitle)
{
		setAxisTitle(QwtPlot::xBottom,newTitle) ;
}

void specPlot::changeTextLabel()
{
	if (sender() != titleAction && sender() != xlabelAction && sender() != ylabelAction) return ;
	QString oldText = (sender() == titleAction ? title().text() :
						  (sender() == xlabelAction ? axisTitle(QwtPlot::xBottom) : axisTitle(QwtPlot::yLeft))).text() ;
	///// First Try  TODO change into in-place editor
	QDialog textDialog ; // TODO subclass this
	textDialog.setLayout(new QVBoxLayout(&textDialog)) ;
	textEdit->setText(oldText) ;
	textDialog.layout()->addWidget(textEdit) ;
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal,&textDialog) ;
	textDialog.layout()->addWidget(buttonBox) ;

	if (sender() == titleAction)
		connect(textEdit,SIGNAL(contentChanged(QString)),this,SLOT(changeTitle(QString))) ;
	else if (sender() == xlabelAction)
		connect(textEdit,SIGNAL(contentChanged(QString)),this,SLOT(changeXLabel(QString))) ;
	else if (sender() == ylabelAction)
		connect(textEdit,SIGNAL(contentChanged(QString)),this,SLOT(changeYLabel(QString))) ;
	connect(buttonBox,SIGNAL(accepted()),&textDialog,SLOT(accept())) ;
	connect(buttonBox,SIGNAL(rejected()),&textDialog,SLOT(reject())) ;

	if (QDialog::Rejected == textDialog.exec())
	{
		if (sender() == titleAction)
			changeTitle(oldText) ;
		if (sender() == xlabelAction)
			changeXLabel(oldText) ;
		if (sender() == ylabelAction)
			changeYLabel(oldText) ;
	}
}

void specPlot::changeYLabel(QString newTitle)
{
//	bool ok = false ; // TODO: QTextEdit
//	QString newTitle = QInputDialog::getText((QWidget*) this,tr("New y-axis label"),tr("New y-axis label:"), QLineEdit::Normal,title().text(),&ok) ;
//	if (ok)
		setAxisTitle(QwtPlot::yLeft,newTitle) ;
}

void specPlot::replot()
{
	if (replotting) return ;
	emit startingReplot();
	replotting = true ;
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
	
	highlightSelectable() ;

	specModelItem *pointer = 0 ; // TODO find a more concise version.
	foreach(QwtPlotItem *item, allItems)
		if((pointer = dynamic_cast<specModelItem*>(item)) || (pointer = dynamic_cast<specMetaItem*>(item)))
			pointer->revalidate();

	
	QRectF boundaries ; //= allItems[0]->boundingRect() ; // DANGER: actually we must check, if the item plots on yLeft
	foreach(QwtPlotItem *item, allItems) // TODO omit this if fixing of axis is enabled
		if(item->yAxis() == QwtPlot::yLeft
				&& !dynamic_cast<specSVGItem*>(item)
				&& !dynamic_cast<QwtPlotSvgItem*>(item)
				&& !dynamic_cast<specRange*>(item)) // TODO change
			boundaries |= item->boundingRect() ;
	
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

	zoom->changeZoomBase(boundaries.isValid() ? boundaries : QRectF(-10,-10,20,20)) ;

	QRectF zoomRect = zoom->zoomRect() ;
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
	return (QList<QAction*>() << modifySVGs) ;
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

void specPlot::writeToStream(QDataStream &out) const
{
	out << title().text()
	    << axisTitle(QwtPlot::xBottom).text()
	    << axisTitle(QwtPlot::yLeft).text() ;
}

void specPlot::readFromStream(QDataStream &in)
{
	QString Title, xlabel, ylabel ;
	in >> Title >> xlabel >> ylabel ;
	setTitle(Title) ;
	setAxisTitle(QwtPlot::xBottom, xlabel) ;
	setAxisTitle(QwtPlot::yLeft, ylabel) ;
}

void specPlot::resizeSVG(specCanvasItem *item, int point, double x, double y)
{
	specSVGItem *pointer = dynamic_cast<specSVGItem*>(item) ;
	if (!pointer || !view || !undoPartner()) return ;

	specSVGItem::bounds oldBounds = pointer->getBounds() ;
	pointer->pointMoved(point,x,y) ;

	specResizeSVGcommand *command = new specResizeSVGcommand ;
	command->setParentObject(this) ;
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
	undoP = lib ; ((specUndoAction*) printAction)->setLibrary(lib);
}

specActionLibrary* specPlot::undoPartner() const
{
	return undoP ;
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
