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

#include "speccanvasitem.h"
#include "specminmaxvalidator.h"
#include <QGridLayout>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <qwt_plot_directpainter.h>

CanvasPicker::CanvasPicker ( specPlot *plot ) :
		QObject ( plot ),
		d_selectedCurve ( NULL ),
		d_selectedPoint ( -1 ) //,
//     mode(spec::none)
{
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
	    "All points can be moved using the left mouse button "
	    "or with these keys:\n\n"
	    "- Up:\t\tSelect next curve\n"
	    "- Down:\t\tSelect previous curve\n"
	    "- Left, �-�:\tSelect next point\n"
	    "- Right, �+�:\tSelect previous point\n"
	    "- 7, 8, 9, 4, 6, 1, 2, 3:\tMove selected point";
#if QT_VERSION >= 0x040000
	canvas->setWhatsThis ( text );
#else
	QWhatsThis::add ( canvas, text );
#endif

//	shiftCurveCursor ( true );  // What is this good for?  Happy debugging?!
	qDebug("CanvasPicker::CanvasPicker %d", d_selectedCurve) ;
}

specCanvasItem* CanvasPicker::current()
{ return lastSelected ; }

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
			qDebug("selected item %d %d",d_selectedCurve, d_selectedPoint) ;
			if(((QMouseEvent*)e)->button() == Qt::RightButton && d_selectedCurve)
			{
				qDebug("calling for contextMenu") ;
				contextMenu = d_selectedCurve->contextMenu() ;
				if (contextMenu)
				{
					contextMenu->exec(( ( QMouseEvent * ) e )->globalPos()) ;
					delete contextMenu ;
					contextMenu = 0 ;
				}
				d_selectedCurve = NULL ;
				d_selectedPoint = -1 ;
			}
			qDebug("Done with event filter") ;
			return true;
		}
		case QEvent::MouseMove:
		{
			move ( ( ( QMouseEvent * ) e )->pos() );
			return true;
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
	qDebug("CanvasPicker::select") ;
	specCanvasItem *curve = NULL;
	double dist = 10e10;
	int index = -1;
	const QwtPlotItemList& itmList = plot()->itemList();
	d_selectedCurve = NULL;
	d_selectedPoint = -1;
//	if ( plot()->itemList().isEmpty() ) return ;

	for ( QwtPlotItemIterator it = plot()->itemList().begin(); it != plot()->itemList().end(); ++it )
	{
		if ( ( *it )->rtti() == QwtPlotCurve::Rtti_PlotCurve )
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
	}

	qDebug("showing cursor") ;
	showCursor ( false );
	qDebug("done showing cursor") ;

	if ( curve && dist < 10 ) // 10 pixels tolerance
	{
		d_selectedCurve = curve;
		d_selectedPoint = index;
		showCursor ( true );
	}
	lastSelected = d_selectedCurve ;

	qDebug("done selecting") ;
// 	d_selectedCurve->selectPoint ( d_selectedPoint ) ; // TODO create parent of both range and modelitem, must be child of QwtPlotCurve
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

// void CanvasPicker::addRange()
// {
// 	ranges << new specRange ( plot()->axisScaleDiv ( 2 )->lowerBound(),plot()->axisScaleDiv ( 2 )->upperBound() ) ;
// 	QPen pen ( QColor ( 255,139,15,100 ) ) ;
// 	pen.setWidth ( 5 ) ;
// 	ranges.last()->setPen ( pen ) ;
// 	pen.setColor ( QColor ( 255,139,15 ) ) ;
// 	ranges.last()->setSymbol ( QwtSymbol ( QwtSymbol::Ellipse,pen.brush(), ( QPen ) pen.color(),QSize ( 5,5 ) ) ) ;
// 	ranges.last()->attach ( plot() ) ;
// // 	emit rangesModified ( &ranges ) ;
// // 	emit haveZeroRanges ( true ) ;
// }
//
// void CanvasPicker::deleteRange()
// {
// 	if ( selectedRange >= 0 )
// 	{
// 		ranges[selectedRange]->detach() ;
// 		delete ranges[selectedRange] ;
// 		ranges.removeAt ( selectedRange ) ;
// 		selectedRange = -1 ; // TODO evtl. select last range, also upon adding ranges...
// 		emit rangesModified ( &ranges ) ;
// 		plot()->replot() ;
// 	}
// 	if ( ranges.isEmpty() )
// 		emit haveZeroRanges ( false ) ;
// }

// Move the selected point
void CanvasPicker::move ( const QPoint &pos )
{
	if ( !d_selectedCurve )
		return;

	qDebug("moving point") ;
	emit pointMoved(d_selectedCurve,d_selectedPoint,
			plot()->invTransform ( d_selectedCurve->xAxis(), pos.x() ),
			plot()->invTransform ( d_selectedCurve->yAxis(), pos.y() ) ) ;
//	d_selectedCurve->pointMoved ( d_selectedPoint,
//	                              plot()->invTransform ( d_selectedCurve->xAxis(), pos.x() ),
//	                              plot()->invTransform ( d_selectedCurve->yAxis(), pos.y() ) ) ;
	qDebug("emitting signal") ;
	emit moved(d_selectedCurve) ; // TODO remove this signal
	/* 	if ( mode == spec::newZero ) // TODO
		{
			( ( specRange* ) d_selectedCurve )->pointMoved ( mode, d_selectedPoint,
			        plot()->invTransform ( d_selectedCurve->xAxis(), pos.x() ),
			        plot()->invTransform ( d_selectedCurve->yAxis(), pos.y() ) ) ;
			emit rangesModified ( &ranges ) ;
			plot()->replot() ;
		}
		else
			( ( specModelItem* ) d_selectedCurve ) ->pointMoved ( mode, d_selectedPoint,
			        plot()->invTransform ( d_selectedCurve->xAxis(), pos.x() ),
			        plot()->invTransform ( d_selectedCurve->yAxis(), pos.y() ) ) ; */

	/*    QwtArray<double> xData(d_selectedCurve->dataSize());
	    QwtArray<double> yData(d_selectedCurve->dataSize());

	    for ( int i = 0; i < d_selectedCurve->dataSize(); i++ )
	    {
	        if ( i == d_selectedPoint )
	        {
	            xData[i] = plot()->invTransform(d_selectedCurve->xAxis(), pos.x());;
	            yData[i] = plot()->invTransform(d_selectedCurve->yAxis(), pos.y());;
	        }
	        else
	        {
	            xData[i] = d_selectedCurve->x(i);
	            yData[i] = d_selectedCurve->y(i);
	        }
	    }
	    d_selectedCurve->setData(xData, yData);
	QTextStream cout(stdout, QIODevice::WriteOnly);
	for (int i = 0 ; i < d_selectedCurve->dataSize() ; i++)
		cout << "(" << d_selectedCurve->x(i) << " " << d_selectedCurve->y(i) << ")  " ;
	cout << endl ;
	    plot()->replot();*/
	showCursor ( true );
}

// Hightlight the selected point
void CanvasPicker::showCursor ( bool showIt )
{

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
	qDebug("CanvasPicker::shiftCurveCursor") ;
	QwtPlotItemIterator it;

	const QwtPlotItemList &itemList = plot()->itemList();

	QwtPlotItemList curveList;
	for ( it = itemList.begin(); it != itemList.end(); ++it )
	{
		if ( ( *it )->rtti() == QwtPlotCurve::Rtti_PlotCurve )
			curveList += *it;
	}
	if ( curveList.isEmpty() )
		return;

	it = curveList.begin();

	if ( d_selectedCurve )
	{
		for ( it = curveList.begin(); it != curveList.end(); ++it )
		{
			if ( d_selectedCurve == *it )
				break;
		}
		if ( it == curveList.end() ) // not found
			it = curveList.begin();

		if ( up )
		{
			++it;
			if ( it == curveList.end() )
				it = curveList.begin();
		}
		else
		{
			if ( it == curveList.begin() )
				it = curveList.end();
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
		curve->pointMoved(point, xBox->text().toDouble(), yBox->text().toDouble()) ;
}
