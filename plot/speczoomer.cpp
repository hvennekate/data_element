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

void specZoomer::changeZoomBase(const QwtDoubleRect& rect)
{
// 	qDebug("changing zoom base") ;
	if(zoomStack()[0] == rect) return ;
	QStack<QwtDoubleRect> stack = zoomStack() ; // TODO ueberarbeiten!
	stack.remove(0) ;
	for(QStack<QwtDoubleRect>::size_type i = 0 ; i < stack.size() ; i++)
		if (! rect.contains(stack[i]))
			stack.remove(i--) ;
	stack.prepend(rect) ;
	uint saveIndex = zoomRectIndex() ;
	setZoomStack(stack) ;
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
	if (saveIndex != zoomRectIndex()) zoom((int) saveIndex-zoomRectIndex()) ;
}

QwtText specZoomer::trackerText(const QwtDoublePoint &pos) const
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


