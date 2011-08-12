// TODO : output (ASCII)
//		- currently plotted data
//		- currently selected
//		- all
//		-> formatting options!
// TODO : import old instruction sets
// TODO : automatic functionality
// TODO : accelerate drag and drop
// TODO : revise merge action
// TODO : replot only after all attachments/detachments are done
// TODO : order in :
//                   - tree-ing
//                   - merging (pos)
// DONE : remove folders with their children
// TODO : smoother or: time/wvn tolerance (on dataItem level)
// TODO : Default-Werte und sensible refreshes
// TODO : Zeitspuren
// TODO : SIGSEGV when closing unchanged dockwidget?
// TODO : dialogs:
//		- plot labels
//		- line types (tool bar?)
//		- brushes
// TODO : view context menu actions need to change "changed" status of file
// TODO : merge dialog: match which descriptors (if numeric -> tolerance)
// TODO : Bugfix folder refresh in tree building (Messung 1135)
// TODO : introduce propertyRole
// TODO : ranges point correction
#include <QApplication>
#include <specappwindow.h>

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(application);
	QCoreApplication::setOrganizationName("MPIbpC") ;
	QCoreApplication::setOrganizationDomain("mpibpc.gwdg.de") ;
	QCoreApplication::setApplicationName("spec-PumpProbe") ;
	QApplication app(argc, argv);
	specAppWindow* mainWindow = new specAppWindow();
	mainWindow->show();
	return app.exec();
}

