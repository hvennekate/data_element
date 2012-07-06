#include "speclogwidget.h"
#include <QVBoxLayout>
#include "speclogtodataconverter.h"

specLogWidget::specLogWidget(specModel* dataModel, QWidget *parent)
	: QDockWidget("Logs", parent),
	  view(new specLogView(this)),
	  layout(new QVBoxLayout(this))
{
	setFloating(true) ;
	QWidget* content = new QWidget(this) ;
	view->setModel(new specLogModel(dataModel,view));
	view->model()->mimeConverters[QString("application/spec.log.item")] = new specLogToDataConverter ;
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

void specLogWidget::write(specOutStream & out)
{
	out.startContainer(spec::logWidget) ;
	view->write(out) ;
	out.stopContainer();
}

bool specLogWidget::read(specInStream &in)
{
	if (!in.expect(spec::logWidget)) return false ;
	view->read(in) ;
	return !in.next() ;
}
