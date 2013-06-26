#include "speclogwidget.h"
#include <QVBoxLayout>
#include "speclogtodataconverter.h"
#include "specgenericmimeconverter.h"
#include "speclogtodataconverter.h"
#include "specmimetextexporter.h"
#include "specmimefileimporter.h"
#include "specactionlibrary.h"

specLogWidget::specLogWidget(QWidget *parent)
	: specDockWidget(tr("Log"), parent),
	  logView(new specLogView(this))
{
	setWhatsThis(tr("Log widget -- This widget contains log data.  Individual log items may be dragged to the data widget in order to import the corresponding data sets."));
	logView->setModel(new specLogModel(logView));
	new specGenericMimeConverter(logView->model()) ;
	new specLogToDataConverter(logView->model()) ;
	new specMimeFileImporter(logView->model()) ;
	new specMimeTextExporter(logView->model()) ;
	setWhatsThis(tr("This dock window contains log data."));
	setObjectName("logWidget");
}

void specLogWidget::writeToStream(QDataStream &out) const
{
	out << *logView ;
}

void specLogWidget::readFromStream(QDataStream &in)
{
	in >> *logView ;
}

QList<QWidget*> specLogWidget::mainWidgets() const
{
	return QList<QWidget*>() << logView ;
}
