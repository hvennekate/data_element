#include "cutbyintensitydialog.h"
#include <qwt_scale_div.h>

cutByIntensityDialog::cutByIntensityDialog(QWidget *parent) :
    QDialog(parent), huevalue(0)
{
	layout = new QVBoxLayout(this) ;
	plot = new specPlot(this) ;
	newRange = new QPushButton("new range",this) ;
	deleteRange = new QPushButton("delete range",this) ;
	buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,this) ;

	layout->addWidget(plot) ;
	layout->addWidget(newRange) ;
	layout->addWidget(deleteRange) ;
	layout->addWidget(buttons) ;

	plot->select = true ;
	plot->refreshMoveMode();
	plot->enableAxis(QwtPlot::yRight,true) ;
	plot->setAxisScale(QwtPlot::yRight,0,33e3) ;
	plot->setAutoReplot(false) ;

	connect(deleteRange,SIGNAL(clicked()),plot,SLOT(deleteRange())) ;
	connect(newRange,SIGNAL(clicked()),this,SLOT(addRange())) ;
	connect(buttons,SIGNAL(accepted()),this,SLOT(accept())) ;
	connect(buttons,SIGNAL(rejected()),this,SLOT(reject())) ;
}

void cutByIntensityDialog::assignSpectra(QList<specModelItem *> spectra)
{
	foreach(specModelItem* item, spectra)
	{
		if (!items.contains(item))
		{
			items << item ;
			item->attach(plot) ;
			intensities << new QwtPlotCurve ;
			QwtArray<double> xdat ;
			for (int i = 0 ; i < item->dataSize() ; i++)
				xdat << item->x(i) ;
			intensities.last()->setData(xdat,item->intensityData()) ;
			intensities.last()->setYAxis(QwtPlot::yRight);
			QPen pen = item->pen() ;
			pen.setStyle(Qt::DashLine) ;
			intensities.last()->setPen(pen) ;
			intensities.last()->attach(plot) ;
		}
	}

	double y1min = INFINITY, y1max = -INFINITY;
	foreach(specModelItem* item, items)
	{
		QwtDoubleRect boundaries = item->boundingRect() ;
		y1min = qMin(y1min,boundaries.top()) ;
		y1max = qMax(y1max,boundaries.bottom()) ;
	}
	qDebug("ymin: %f, ymax: %f",y1min,y1max) ;
	plot->setAxisScale(QwtPlot::yLeft,y1min,10.) ;
//	plot->updateAxes();
	plot->replot() ;
}

void cutByIntensityDialog::performDeletion()
{
	qDebug("performing deletion") ;
	QList<specCanvasItem*>* selectable = plot->selectable() ;
	QList<specRange*>* ranges = new QList<specRange*> ;
	for (int i = 0 ; i < selectable->size() ; i++)
		ranges->append((specRange*) selectable->at(i)) ;
	foreach(specModelItem* item, items)
		item->removeData(ranges) ;
	delete ranges ;
}

void cutByIntensityDialog::addRange()
{
	double min = plot->axisScaleDiv(QwtPlot::xBottom)->lowerBound(), max = plot->axisScaleDiv(QwtPlot::xBottom)->upperBound() ;
	specRange* range = new specSelectRange(min-.1*(max-min),max+.1*(max-min)) ;
	range->attach(plot) ;
	plot->refreshRanges();
}
cutByIntensityDialog::~cutByIntensityDialog()
{
	for (int i = 0 ; i < items.size() ; i++)
		items[i]->detach();
}
