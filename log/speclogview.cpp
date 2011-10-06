#include "speclogview.h"

specLogView::specLogView(QWidget *par)
	:specView(par)
{
}

void specLogView::setModel(specLogModel* mod)
{
	specView::setModel(mod) ;
}
