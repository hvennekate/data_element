#include "specsvgunitbutton.h"
#include <qwt_plot.h>

specSvgUnitButton::specSvgUnitButton(QWidget *parent) :
	QPushButton(parent)
{
	setText(tr(""));
	setCheckable(true) ;
	connect(this,SIGNAL(toggled(bool)),this,SLOT(stateChanged())) ;
}

void specSvgUnitButton::stateChanged()
{
	if (isChecked()) setText(tr("Pixels"));
	else setText(tr("Axis Units")) ;
}
