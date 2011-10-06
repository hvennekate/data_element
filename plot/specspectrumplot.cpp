#include "specspectrumplot.h"

specSpectrumPlot::specSpectrumPlot(QWidget *parent) :
    specPlot(parent)
{
	offsetAction= new QAction(QIcon(":/offset.png"), "Offset", this) ;
	offlineAction = new QAction(QIcon(":/offline.png"), "Slope",this) ;
	scaleAction = new QAction(QIcon(":/scale.png"),"Scale", this) ;

	correctionActions->addAction(offsetAction) ;
	correctionActions->addAction(offlineAction) ;
	correctionActions->addAction(scaleAction) ;

	connect(correctionActions,SIGNAL(triggered(QAction*)),this,SLOT(correctionsChanged())) ;
	foreach(QAction *action, correctionActions->actions())
		action->setCheckable(true) ;
}

void specSpectrumPlot::correctionsChanged()
{
	bool checked = false ;
	foreach(QAction *action, correctionActions->actions())
		checked = checked || action->isChecked() ;
	if (!checked)


}
