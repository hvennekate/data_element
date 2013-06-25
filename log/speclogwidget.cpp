#include "speclogwidget.h"
#include <QVBoxLayout>
#include "speclogtodataconverter.h"
#include "specgenericmimeconverter.h"
#include "speclogtodataconverter.h"
#include "specmimetextexporter.h"
#include "specmimefileimporter.h"
#include "specactionlibrary.h"

specLogWidget::specLogWidget(QWidget *parent)
	: specDockWidget(tr("Logs"), parent),
	  logView(new specLogView(this)),
	  layout(new QVBoxLayout)
{
	setWhatsThis(tr("Log widget -- This widget contains log data.  Individual log items may be dragged to the data widget in order to import the corresponding data sets."));
	setFloating(true) ;
	QWidget* content = new QWidget(this) ;
	logView->setModel(new specLogModel(logView));
	new specGenericMimeConverter(logView->model()) ;
	new specLogToDataConverter(logView->model()) ;
    new specMimeFileImporter(logView->model()) ;
    new specMimeTextExporter(logView->model()) ;
	layout->addWidget(logView) ;
	content->setLayout(layout);
	setWidget(content);
	setWhatsThis(tr("This dock window contains log data."));
	setObjectName("logWidget");
}

void specLogWidget::addToolbar(specActionLibrary *actions)
{
	if (logView && logView->model())
		actions->addDragDropPartner(logView->model()) ;
	layout->insertWidget(0,actions->toolBar(this->logView)) ;
}

void specLogWidget::writeToStream(QDataStream &out) const
{
	out << *logView ;
}

void specLogWidget::readFromStream(QDataStream &in)
{
	in >> *logView ;
}
