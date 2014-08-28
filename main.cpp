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
#include <QVector>
#include "asciiexporter.h"
#include <QFile>
#include "bzipiodevice.h"
#include <QPair>
#include "specshortcutdialog.h"
#include <QString>

int main(int argc, char* argv[])
{
#ifdef WIN32BUILD
	QIcon::setThemeName("oxygen") ;
	QString appPath ;
	if (argc) appPath = QDir::fromNativeSeparators(argv[0]) ;
	appPath = appPath.left(appPath.lastIndexOf('/')) ;
	std::cout << "String: \"" << appPath.toStdString() << "\" " << appPath.size() << std::endl ;
	if (!appPath.isEmpty())
		appPath += "/" ;
	(appPath += "platforms").prepend("QT_QPA_PLATFORM_PLUGIN_PATH=") ;
	std::cout << "Platform path: " << appPath.toStdString() << std::endl ;
	_putenv(appPath.toStdString().c_str()) ;
#endif

	Q_INIT_RESOURCE(application);
	QCoreApplication::setOrganizationName("MPIbpC") ;
	QCoreApplication::setOrganizationDomain("mpibpc.mpg.de") ;
	QCoreApplication::setApplicationName("spec-PumpProbe") ;

	QApplication app(argc, argv) ;

	QString parameter(argc > 1 ? QString(argv[1]) : QString("")) ;
	if(parameter.left(2).toLower() == "-e")
	{
		bool withFiles(parameter.left(2) == "-E") ;
		QVector<QPair<QVector<int>, QString> > items ;
		for(int i = 3 ; i < argc ; ++i)
		{
			QStringList l =  QString(argv[i]).split("/") ;
			QVector<int> item ;
			foreach(QString n, l)
			item << n.toInt() ;
			QString filename ;
			if(withFiles && i < argc) filename = argv[++i] ;
			items << qMakePair(item, filename) ;
		}
		QFile* file = new QFile(argv[2]) ;
		if(!file->open(QFile::ReadOnly))
		{
			qDebug() << "Could not open file " << file->fileName() << " for export.";
			delete file ;
			return 1 ;
		}
		QTextStream cout(stdout, QIODevice::WriteOnly)  ;
		QDataStream in(file) ;
		asciiExporter exporter(parameter.mid(2) == "d" ? asciiExporter::data :
				       (parameter.mid(2) == "l" ? asciiExporter::log : asciiExporter::meta)) ;

		// TODO make static public in specplotwidget
		// Basic layout of the file:
		quint64 check ;
		in >> check ;
		if(check != FILECHECKRANDOMNUMBER && check != FILECHECKCOMPRESSNUMBER)
		{
			qDebug() << "File " << file->fileName() << " is not valid." ;
			delete file ;
			return 2 ;
		}
		QByteArray fileContent = file->readAll() ;
		file->close();

		QDataStream inStream(fileContent) ;
		bzipIODevice* zipDevice = 0 ;
		QBuffer buffer(&fileContent) ;  // does not take ownership of byteArray
		if(FILECHECKCOMPRESSNUMBER == check)
		{
			qDebug() << "File " << file->fileName() << " is compressed." ;
			zipDevice = new bzipIODevice(&buffer) ;  // takes ownership of buffer
			qDebug() << "opening buffer:" << zipDevice->open(bzipIODevice::ReadOnly) ;
			inStream.setDevice(zipDevice);
		}
		exporter.readFromStream(inStream) ;
		zipDevice->releaseDevice() ;
		delete zipDevice ;
		typedef QPair<QVector<int>, QString> itemPair ;
		qDebug() << "Exporting from file " << file->fileName() ;
		delete file ;
		foreach(const itemPair & item, items)
		{
			QFile outfile(item.second);
			qDebug() << "Exporting item " << item.first << " to " << item.second ;
			if(!item.second.isEmpty() && outfile.open(QFile::WriteOnly))
			{
				QTextStream out(&outfile) ;
				out << exporter.content(item.first) ;
				out.setDevice(&outfile) ;
			}
			else
				cout << exporter.content(item.first) ;
		}
	}
	else
	{
		specAppWindow* mainWindow = new specAppWindow();
		new specShortcutDialog(mainWindow) ;
		mainWindow->show();
		return app.exec();
	}
	return 0 ;
}

