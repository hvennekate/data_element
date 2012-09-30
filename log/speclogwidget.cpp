#include "speclogwidget.h"
#include <QVBoxLayout>
#include "speclogtodataconverter.h"
#include "specgenericmimeconverter.h"
#include "speclogtodataconverter.h"
#include "specmimetextexporter.h"

specLogWidget::specLogWidget(QWidget *parent)
	: QDockWidget("Logs", parent),
	  view(new specLogView(this)),
	  layout(new QVBoxLayout)
{
	setWhatsThis(tr("Log widget -- This widget contains log data.  Individual log items may be dragged to the data widget in order to import the corresponding data sets."));
	setFloating(true) ;
	QWidget* content = new QWidget(this) ;
	view->setModel(new specLogModel(view));
	new specGenericMimeConverter(view->model()) ;
	new specLogToDataConverter(view->model()) ;
	new specMimeTextExporter(view->model()) ;
	layout->addWidget(view) ;
	content->setLayout(layout);
	setWidget(content);
	setWhatsThis(tr("This dock window contains log data."));
}

void specLogWidget::addToolbar(specActionLibrary *actions)
{
	if (view && view->model())
		actions->addDragDropPartner(view->model()) ;
	layout->insertWidget(0,actions->toolBar(this->view)) ;
}

void specLogWidget::writeToStream(QDataStream &out) const
{
	out << *view ;
}

void specLogWidget::readFromStream(QDataStream &in)
{
	in >> *view ;
}
