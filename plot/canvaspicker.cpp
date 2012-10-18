#include <qapplication.h>
#include <qevent.h>
#include <qwhatsthis.h>
#include <qpainter.h>
#include "specplot.h"
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>
#include "canvaspicker.h"
#include <QTextStream>
#include <QBrush>
#include <QColor>
#include <QDoubleValidator>

#include "speccanvasitem.h"
#include <QGridLayout>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <qwt_plot_directpainter.h>

CanvasPicker::CanvasPicker ( specPlot *plot )
	: QObject ( plot ),
	  owning(false),
	  d_selectedCurve ( NULL ),
	  d_selectedPoint ( -1 ),
	  highlighting(true)
//     mode(spec::none)
{
	qDebug() << "creating canvas picker" << this ;
	QwtPlotCanvas *canvas = plot->canvas();

	canvas->installEventFilter ( this );

	// We want the focus, but no focus rect. The
	// selected point will be highlighted instead.

#if QT_VERSION >= 0x040000
	canvas->setFocusPolicy ( Qt::StrongFocus );
#ifndef QT_NO_CURSOR
	canvas->setCursor ( Qt::CrossCursor );
#endif
#else
	canvas->setFocusPolicy ( QWidget::StrongFocus );
#ifndef QT_NO_CURSOR
	canvas->setCursor ( Qt::CrossCursor );
#endif
#endif
	canvas->setFocusIndicator ( QwtPlotCanvas::ItemFocusIndicator );
	canvas->setFocus();

	const char *text =
	    "Points in this plot may be moved by dragging (if indicated) or positioned precisely by "
	    "double clicking on them.  If zooming is enabled, step-wise zoom may be performed using "
	    "the middle mouse button (click and drag the zoom region).  Clicking the right mouse "
	    "button will go back one zoom step.";
#if QT_VERSION >= 0x040000
	canvas->setWhatsThis ( text );
#else
	QWhatsThis::add ( canvas, text );
#endif

}

void CanvasPicker::setSelectable(const QSet<specCanvasItem *> &toSet)
{
	QSet<specCanvasItem*> toRemove(selectable - toSet),
						  toAdd(toSet - selectable) ;
	removeSelectable(toRemove) ;
	addSelectable(toAdd) ;
}

bool CanvasPicker::event ( QEvent *e )
{
	if ( e->type() == QEvent::User )
	{
		showCursor ( true );
		return true;
	}
	return QObject::event ( e );
}

bool CanvasPicker::eventFilter ( QObject *object, QEvent *e )
{
	if ( object != ( QObject * ) plot()->canvas() )
		return false;
	
	QMenu *contextMenu ;
	switch ( e->type() )
	{
		case QEvent::FocusIn:
			showCursor ( true );
		case QEvent::FocusOut:
			showCursor ( false );

		case QEvent::Paint:
		{
			QApplication::postEvent ( this, new QEvent ( QEvent::User ) );
			break;
		}
		case QEvent::MouseButtonDblClick:
		{
			select ( ( ( QMouseEvent* ) e )->pos() );
			if(d_selectedCurve)
				movePointExplicitly() ;
			return true;
		}
		case QEvent::MouseButtonPress:
		{
			select ( ( ( QMouseEvent * ) e )->pos() );
			if(((QMouseEvent*)e)->button() == Qt::RightButton && d_selectedCurve)
			{
				contextMenu = d_selectedCurve->contextMenu() ;
				if (contextMenu)
				{
					contextMenu->exec(( ( QMouseEvent * ) e )->globalPos()) ;
					delete contextMenu ;
					contextMenu = 0 ;
				}
				d_selectedCurve = NULL ;
				d_selectedPoint = -1 ;
				return true ;
			}
			return d_selectedCurve ;
		}
		case QEvent::MouseMove:
		{
			move ( ( ( QMouseEvent * ) e )->pos() );
			return d_selectedCurve ;
		}
		case QEvent::MouseButtonRelease:
		{
			showCursor ( false );
			d_selectedCurve = NULL;
			d_selectedPoint = -1;
		}
		case QEvent::KeyPress:
		{
			const int delta = 1; // TODO to settings
			switch ( ( ( const QKeyEvent * ) e )->key() )
			{
				case Qt::Key_Up:
					shiftCurveCursor ( true );
					return true;

				case Qt::Key_Down:
					shiftCurveCursor ( false );
					return true;

				case Qt::Key_Right:
				case Qt::Key_Plus:
					if ( d_selectedCurve )
						shiftPointCursor ( true );
					else
						shiftCurveCursor ( true );
					return true;

				case Qt::Key_Left:
				case Qt::Key_Minus:
					if ( d_selectedCurve )
						shiftPointCursor ( false );
					else
						shiftCurveCursor ( true );
					return true;

					// The following keys represent a direction, they are
					// organized on the keyboard.

				case Qt::Key_1:
					moveBy ( -delta, delta );
					break;
				case Qt::Key_2:
					moveBy ( 0, delta );
					break;
				case Qt::Key_3:
					moveBy ( delta, delta );
					break;
				case Qt::Key_4:
					moveBy ( -delta, 0 );
					break;
				case Qt::Key_6:
					moveBy ( delta, 0 );
					break;
				case Qt::Key_7:
					moveBy ( -delta, -delta );
					break;
				case Qt::Key_8:
					moveBy ( 0, -delta );
					break;
				case Qt::Key_9:
					moveBy ( delta, -delta );
					break;
				default:
					break;
			}
		}
		default:
			break;
	}
	return QObject::eventFilter ( object, e );
}

// Select the point at a position. If there is no point
// deselect the selected point

void CanvasPicker::select ( const QPoint &pos )
{
	specCanvasItem *curve = NULL;
	double dist = 10e10;
	int index = -1;
	d_selectedCurve = NULL;
	d_selectedPoint = -1;

	for ( QSet<specCanvasItem*>::iterator it = selectable.begin(); it != selectable.end(); ++it )
	{
		specCanvasItem *c = ( specCanvasItem* ) ( *it );

		double d;
		int idx = c->closestPoint ( pos, &d );
		if ( d < dist )
		{
			curve = c;
			index = idx;
			dist = d;
		}
	}

	showCursor ( false );

	if ( curve && dist < 10 ) // 10 pixels tolerance
	{
		d_selectedCurve = curve;
		d_selectedPoint = index;
		showCursor ( true );
	}
	lastSelected = d_selectedCurve ;
}


// Move the selected point
void CanvasPicker::moveBy ( int dx, int dy )
{
	if ( dx == 0 && dy == 0 )
		return;

	if ( !d_selectedCurve )
		return;

	const int x = plot()->transform ( d_selectedCurve->xAxis(),
					  d_selectedCurve->sample( d_selectedPoint ).x() ) + dx;
	const int y = plot()->transform ( d_selectedCurve->yAxis(),
					  d_selectedCurve->sample( d_selectedPoint ).y() ) + dy;

	move ( QPoint ( x, y ) );
}

// Move the selected point
void CanvasPicker::move ( const QPoint &pos )
{
	if ( !d_selectedCurve )
		return;

	emit pointMoved(d_selectedCurve,d_selectedPoint,
			plot()->invTransform ( d_selectedCurve->xAxis(), pos.x() ),
			plot()->invTransform ( d_selectedCurve->yAxis(), pos.y() ) ) ;

	showCursor ( true );
}

// Hightlight the selected point
void CanvasPicker::showCursor ( bool showIt )
{
Q_UNUSED(showIt)
// TODO this seems to be buggy in Qwt 6.0.1 and 6.0.0:  Ther is always a trace behind the cursor.  Reimplement!

//	QwtSymbol *symbol = new QwtSymbol(*(d_selectedCurve->symbol()));

//	QwtSymbol *newSymbol = new QwtSymbol(*symbol) ;
//	if ( showIt )
//		newSymbol->setBrush ( QColor ( 255-symbol->brush().color().red(),
//					      255-symbol->brush().color().green(),
//					      255-symbol->brush().color().blue() ) ) ;

//	const bool doReplot = plot()->autoReplot();

//	plot()->setAutoReplot ( false );
//	d_selectedCurve->setSymbol ( newSymbol );

////	d_selectedCurve->draw ( d_selectedPoint, d_selectedPoint );
//	QwtPlotDirectPainter directPainter;
//	directPainter.drawSeries(d_selectedCurve, d_selectedPoint, d_selectedPoint);

//	d_selectedCurve->setSymbol ( symbol );
//	plot()->setAutoReplot ( doReplot );
}

// Select the next/previous curve
void CanvasPicker::shiftCurveCursor ( bool up )
{
	QSet<specCanvasItem*>::iterator it;

	if ( selectable.isEmpty() )
		return;

	it = selectable.begin();

	if ( d_selectedCurve )
	{
		for ( it = selectable.begin(); it != selectable.end(); ++it )
		{
			if ( d_selectedCurve == *it )
				break;
		}
		if ( it == selectable.end() ) // not found
			it = selectable.begin();

		if ( up )
		{
			++it;
			if ( it == selectable.end() )
				it = selectable.begin();
		}
		else
		{
			if ( it == selectable.begin() )
				it = selectable.end();
			--it;
		}
	}

	showCursor ( false );
	d_selectedPoint = 0;
	d_selectedCurve = ( specCanvasItem* ) *it;
// 	if ( mode == spec::newZero )
// 		selectedRange = ranges.indexOf ( ( specRange* ) *it ) ;
	showCursor ( true );
}

// Select the next/previous neighbour of the selected point
void CanvasPicker::shiftPointCursor ( bool up )
{
	if ( !d_selectedCurve )
		return;

	int index = d_selectedPoint + ( up ? 1 : -1 );
	index = ( index + d_selectedCurve->dataSize() ) % d_selectedCurve->dataSize();

	if ( index != d_selectedPoint )
	{
		showCursor ( false );
		d_selectedPoint = index;
		showCursor ( true );
	}
}

// void CanvasPicker::pointMoved(int mode,int no,double x, double y) {}

void CanvasPicker::movePointExplicitly()
{
	specCanvasItem *curve = d_selectedCurve ;
	int point = d_selectedPoint ;
	QDialog *query = new QDialog ;
	query->setWindowTitle ( "Move point to value" ) ;
	QGridLayout *layout = new QGridLayout ;
	QLineEdit *xBox = new QLineEdit ( QString("%1").arg(curve->sample (point).x() )),
	*yBox = new QLineEdit ( QString("%1").arg(curve->sample (point).y() )) ;
	xBox->setValidator(new QDoubleValidator(xBox)) ;
	yBox->setValidator(new QDoubleValidator(yBox)) ;
	
	QLabel *xLabel = new QLabel ( "x:" ), *yLabel = new QLabel ( "y:" ) ;
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel) ;
	connect(buttons,SIGNAL(accepted()),query,SLOT(accept())) ;
	connect(buttons,SIGNAL(rejected()),query,SLOT(reject())) ;
	layout->addWidget(xLabel,0,0) ;
	layout->addWidget(yLabel,1,0) ;
	layout->addWidget(xBox,0,1) ;
	layout->addWidget(yBox,1,1) ;
	layout->addWidget(buttons,2,0,1,2) ;
	query->setLayout(layout) ;
	if(query->exec() == QDialog::Accepted)
		emit pointMoved(curve, point, xBox->text().toDouble(), yBox->text().toDouble());
}

void CanvasPicker::highlightSelectable(bool highlight)
{
	highlighting = highlight ;
	highlightSelectable();
}

void CanvasPicker::highlightSelectable()
{
	switchHighlighting(highlighting);
}

void CanvasPicker::switchHighlighting(bool on)
{
	foreach(specCanvasItem* item, selectable)
		item->highlight(on) ;
	plot()->replot();

}

void CanvasPicker::addSelectable(specCanvasItem *item)
{
	QSet<specCanvasItem*> list ;
	list << item ;
	addSelectable(list) ;
}

void CanvasPicker::removeSelectable(specCanvasItem *item)
{
	QSet<specCanvasItem*> list ;
	list << item ;
	removeSelectable(list) ;
}

void CanvasPicker::addSelectable(const QSet<specCanvasItem *> &list)
{
	selectable += list ;
	highlightSelectable() ;
}

void CanvasPicker::removeSelectable(QSet<specCanvasItem *> &list)
{
	if (highlighting) switchHighlighting(false) ;
	if (list.contains(d_selectedCurve))
	{
		d_selectedCurve = 0 ;
		d_selectedPoint = -1 ;
	}
	if (owning)
	{
		foreach(specCanvasItem* item, list)
		{
			item->detach();
			delete item ;
		}
	}
	foreach(specCanvasItem* item, list)
		selectable.remove(item) ;

	if (highlighting) highlightSelectable() ;
	plot()->replot() ;
}

void CanvasPicker::removeSelected()
{
	if (selectable.contains(lastSelected))
		removeSelectable(lastSelected);
}

void CanvasPicker::removeSelectable()
{
	removeSelectable(selectable) ;
}



CanvasPicker::~CanvasPicker()
{
	qDebug() << "Deleting canvas picker" << this ;
	highlightSelectable(false) ;
	if (owning) removeSelectable(selectable);
}

QList<specCanvasItem*> CanvasPicker::items() const
{
	return selectable.toList() ;
}

void CanvasPicker::purgeSelectable()
{
	selectable.clear() ;
}
