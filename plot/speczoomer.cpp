#include "speczoomer.h"
#include <QTextStream>

specZoomer::specZoomer(QwtPlotCanvas *canvas)
 : QwtPlotZoomer(canvas,false)
{
	setTrackerMode(AlwaysOn);
	
	setMousePattern(QwtEventPattern::MouseSelect1, Qt::MidButton) ;
	setMousePattern(QwtEventPattern::MouseSelect4, Qt::MidButton) ;
	setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
	setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
	
	setRubberBandPen(QColor(Qt::darkBlue)) ;
	setTrackerPen(QColor(Qt::darkBlue)) ;
}

void specZoomer::changeZoomBase(const QRectF& rect)
{
	if(zoomBase() == rect) return ;
	QStack<QRectF> stack = zoomStack() ; // TODO ueberarbeiten!
	stack.remove(0) ;
	for(QStack<QRectF>::size_type i = 0 ; i < stack.size() ; i++)
		if (! rect.contains(stack[i]))
			stack.remove(i--) ;
	stack.prepend(rect) ;
	setZoomStack(stack, stack.indexOf(zoomRect())) ;
}

QwtText specZoomer::trackerText(const QPointF &pos) const
{
	QColor bg(Qt::white);
#if QT_VERSION >= 0x040300
	bg.setAlpha(200);
#endif
	QwtText text ; //alt: = QwtPlotZoomer::trackerText(pos);
	text.setText(QString("%1, %2").arg(pos.x(),0,'f',2).arg(pos.y(),0,'f',3)) ;
	text.setBackgroundBrush( QBrush( bg ));
	return text;
}

specZoomer::~specZoomer()
{
}


