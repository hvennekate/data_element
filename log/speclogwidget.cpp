#include "speclogwidget.h"
#include <QVBoxLayout>
#include "speclogtodataconverter.h"
#include "specgenericmimeconverter.h"
#include "speclogtodataconverter.h"

specLogWidget::specLogWidget(QWidget *parent)
	: QDockWidget("Logs", parent),
	  view(new specLogView(this)),
	  layout(new QVBoxLayout)
{
	setFloating(true) ;
	QWidget* content = new QWidget(this) ;
	view->setModel(new specLogModel(view));
	new specGenericMimeConverter(view->model()) ;
	new specLogToDataConverter(view->model()) ;
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
