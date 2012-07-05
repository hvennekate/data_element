#include "specdataview.h"
#include <QTextStream>

specDataView::specDataView(specModel* mod, QWidget* parent)
		: specView(parent)
{
	setModel(mod) ;
}

specDataView::~specDataView()
{
}
