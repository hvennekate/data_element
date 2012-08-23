#include "cutbyintensitydialog.h"
#include <qwt_scale_div.h>
#include "canvaspicker.h"

cutByIntensityDialog::cutByIntensityDialog(QWidget *parent) :
    QDialog(parent), huevalue(0)
{
	layout = new QVBoxLayout(this) ;
	plot = new specPlot(this) ;
	newRange = new QPushButton("new range",this) ;
	deleteRange = new QPushButton("delete range",this) ;
	buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,this) ;
	picker = new CanvasPicker(plot) ;

	layout->addWidget(plot) ;
	layout->addWidget(newRange) ;
	layout->addWidget(deleteRange) ;
	layout->addWidget(buttons) ;

	plot->enableAxis(QwtPlot::yRight,true) ;
	plot->setAxisScale(QwtPlot::yRight,0,33e3) ;
	plot->setAutoReplot(false) ;
	plot->setAutoDelete(true) ;

	picker->setOwning() ;

	connect(deleteRange,SIGNAL(clicked()),this,SLOT(removeRange())) ;
	connect(newRange,SIGNAL(clicked()),this,SLOT(addRange())) ;
	connect(buttons,SIGNAL(accepted()),this,SLOT(accept())) ;
	connect(buttons,SIGNAL(rejected()),this,SLOT(reject())) ;
	connect(picker, SIGNAL(pointMoved(specCanvasItem*,int,double,double)),this,SLOT(rangeModified(specCanvasItem*,int,double,double))) ;
}

void cutByIntensityDialog::assignSpectra(QList<specModelItem *> spectra)
{
	foreach(specModelItem* item, spectra)
	{
		if (!items.contains(item))
		{
			items << item ;
			QwtPlotCurve *intensity = new QwtPlotCurve ;
			QwtPlotCurve *dataCurve = new QwtPlotCurve ;
			QVector<double> xdat, ydat ;
			for (size_t i = 0 ; i < item->dataSize() ; i++)
			{
				xdat << item->sample(i).x() ;
				ydat << item->sample(i).y() ;
			}
			intensity->setSamples(xdat,item->intensityData()) ;
			dataCurve->setSamples(xdat,ydat); ;
			intensity->setYAxis(QwtPlot::yRight);
			QPen pen = item->pen() ;
			dataCurve->setPen(pen) ;
			pen.setStyle(Qt::DashLine) ;
			intensity->setPen(pen) ;
			dataCurve->attach(plot) ;
			intensity->attach(plot) ;
		}
	}

	double y1min = INFINITY, y1max = -INFINITY;
	foreach(specModelItem* item, items)
	{
		QRectF boundaries = item->boundingRect() ;
		y1min = qMin(y1min,boundaries.top()) ;
		y1max = qMax(y1max,boundaries.bottom()) ;
	}
	plot->setAxisScale(QwtPlot::yLeft,y1min,10.) ;
	plot->replot() ;
}

QList<specRange*> cutByIntensityDialog::ranges() // TODO create access function in canvas picker
{
	QList<specCanvasItem*> selectable = picker->items() ;
	QList<specRange*> allRanges ;
	foreach(specCanvasItem* pointer, selectable)
		allRanges << (specRange*) pointer ;
	return allRanges ;
}

void cutByIntensityDialog::addRange()
{
	double min = plot->axisScaleDiv(QwtPlot::xBottom)->lowerBound(), max = plot->axisScaleDiv(QwtPlot::xBottom)->upperBound() ;
	specRange* range = new specSelectRange(min+.1*(max-min),max-.1*(max-min)) ;
	range->attach(plot) ;
	picker->addSelectable(range) ;
}

void cutByIntensityDialog::removeRange()
{
	picker->removeSelected();
}

cutByIntensityDialog::~cutByIntensityDialog()
{
}

void cutByIntensityDialog::rangeModified(specCanvasItem *range, int point, double newX, double newY)
{
	range->pointMoved(point,newX,newY);
	plot->replot();
}
