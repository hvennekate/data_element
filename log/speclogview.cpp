#include "speclogview.h"

specLogView::specLogView(QWidget *par)
	:specView(par)
{
	setWhatsThis(tr("This list contains log entries and system warnings obtained from log files.  Use the import button to add the contents of log files to this list.  Manage items by dragging and dropping.\nItems can be dragged from this list to the data list to insert the corresponding data sets there.\nSome log data may be changed by double clicking onto the corresponding entry.  The first column, e.g. is reserved for your comments/descriptions.  If a field contains multiple lines, an indicator will be displayed."));
}

void specLogView::setModel(specLogModel* mod)
{
	specView::setModel(mod) ;
}
