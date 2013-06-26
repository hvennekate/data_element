#include "specdataview.h"
#include <QTextStream>

specDataView::specDataView(QWidget* parent)
	: specView(parent)
{
	setWhatsThis(tr("This list contains data entries.  Use the import button to import data from files.  Manage items by dragging and dropping."));
	setObjectName("mainView");
}

specDataView::~specDataView()
{
}
