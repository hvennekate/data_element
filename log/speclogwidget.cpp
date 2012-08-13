#include "speclogwidget.h"
#include <QVBoxLayout>
#include "speclogtodataconverter.h"

specLogWidget::specLogWidget(QWidget *parent)
	: QDockWidget("Logs", parent),
	  view(new specLogView(this)),
	  layout(new QVBoxLayout)
{
	setFloating(true) ;
	QWidget* content = new QWidget(this) ;
	view->setModel(new specLogModel(view));
	view->model()->addMimeConverter(new specLogToDataConverter);
	layout->addWidget(view) ;
	content->setLayout(layout);
	setWidget(content);
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
