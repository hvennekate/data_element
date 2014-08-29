#include "toggle3dplotaction.h"
#include "specplotwidget.h"
#include "spec3dquickplot.h"
#include <QComboBox>
#include <QVBoxLayout>
#include "specsplitter.h"


toggle3DPlotAction::toggle3DPlotAction(QObject *parent) :
	specUndoAction(parent),
	plotWidget(new QWidget),
	descriptors(new QComboBox(plotWidget)),
	quickplot(new spec3dQuickPlot(plotWidget))
{
	setIcon(QIcon(":/fast3dplot.png"));
	setWhatsThis(tr("Enable or disable a preview of "));
	setText(tr("Quick 3D plot")) ;
	setCheckable(true) ;
	new QVBoxLayout(plotWidget) ;
	plotWidget->layout()->addWidget(descriptors) ;
	plotWidget->layout()->addWidget(quickplot) ;
	connect(descriptors, SIGNAL(currentTextChanged(QString)), quickplot, SLOT(setDescriptor(QString))) ;
	connect(plotWidget, SIGNAL(destroyed()), this, SLOT(plotWidgetDeleted())) ;
}

void toggle3DPlotAction::plotWidgetDeleted()
{
	plotWidget = 0 ;
}

toggle3DPlotAction::~toggle3DPlotAction()
{
	delete plotWidget ;
}

const std::type_info& toggle3DPlotAction::possibleParent()
{
	return typeid(specPlotWidget) ;
}

void toggle3DPlotAction::execute()
{
	if (!parent()) return ;
	if (!parent()->parent()) return ;
	if (!parent()->parent()->parent()) return ; // TODO this is ugly!
	if (!plotWidget) return ;
	if (isChecked())
	{
		qDebug() << parent()->objectName() ;
		qDebug() << parent()->parent()->objectName() ;
		qDebug() << parent()->parent()->parent()->objectName() ;
		specSplitter* splitter = parent()->parent()->parent()->findChild<specSplitter*>() ;
		if (!splitter) return ;
		if (0 > splitter->indexOf(plotWidget))
			splitter->addWidget(plotWidget) ;
		plotWidget->setVisible(true) ;
		plotWidget->show();
		specModel* model = parent()->findChild<specModel*>() ;
		QString descriptor = descriptors->currentText() ;
		descriptors->clear();
		if (!model) return ;
		descriptors->addItems(model->descriptorsWithFlags(spec::numeric)) ;
		descriptors->setCurrentText(descriptor) ;
		quickplot->setPlot(parent()->parent()->parent()->findChild<specPlot*>()) ;
	}
	else
	{
		quickplot->setPlot(0);
		plotWidget->setVisible(false) ;
	}
}
