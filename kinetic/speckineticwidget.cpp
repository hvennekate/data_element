#include "speckineticwidget.h"
#include <QTextStream>
#include "speccanvasitem.h"
#include "specgenericmimeconverter.h"
#include "specmimetextexporter.h"
#include "specsplitter.h"
#include "canvaspicker.h"
#include "specactionlibrary.h"
#include <QApplication>

specMetaView* specKineticWidget::view()
{ return items ; }

specKineticWidget::specKineticWidget(QWidget* parent)
	: specDockWidget(tr("Meta"), parent)
{
	setWhatsThis(tr("Meta dock widget - In this widget, further processing of the primary data can be done (integration, max, min, etc.)"));
	plot = new specPlot ;
	items = new specMetaView(this) ;

	plot->setMinimumHeight(100) ;
	//	plot->setAxisTitle(QwtPlot::yLeft,"cm<sup>-1</sup> <font face=\"Symbol\">D</font>mOD") ;
	//	plot->setAxisTitle(QwtPlot::xBottom,"ps") ;

	items->setModel(new specMetaModel(items)) ;
	new specGenericMimeConverter(items->model());
	new specMimeTextExporter(items->model()) ;

	connect(plot->svgAction(), SIGNAL(toggled(bool)), this, SLOT(svgModification(bool))) ;
	svgModification(false) ;
	setObjectName(tr("Meta window")) ;
	toggleViewAction()->setText(tr("Toggle meta window"));
	plot->setObjectName("metaPlot");
}

void specKineticWidget::writeToStream(QDataStream& out) const
{
	out << *plot << *items ;
}

void specKineticWidget::readFromStream(QDataStream& in)
{
	in >> *plot >> *items ;
}

// TODO crash when another meta item is added after the first has been assigned servers
// TODO shift to parent class of kineticWidget and plotWidget
void specKineticWidget::svgModification(bool mod)
{
	if(mod) connect(plot->svgPicker(), SIGNAL(pointMoved(specCanvasItem*, int, double, double)), items->model(), SLOT(svgMoved(specCanvasItem*, int, double, double))) ;
	else disconnect(plot->svgPicker(), SIGNAL(pointMoved(specCanvasItem*, int, double, double)), items->model(), SLOT(svgMoved(specCanvasItem*, int, double, double))) ;

	plot->svgPicker()->highlightSelectable(mod) ;
}

QList<QWidget*> specKineticWidget::mainWidgets() const
{
	return QList<QWidget*>() << items << plot ;
}
