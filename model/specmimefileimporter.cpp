#include "specmimefileimporter.h"
#include <QFile>
#include <QMimeData>
#include "utility-functions.h"
#include "specmodel.h"
#include <QMessageBox>
#include <QUrl>

specMimeFileImporter::specMimeFileImporter(QObject* parent) :
	specMimeConverter(parent)
{
}

void specMimeFileImporter::exportData(QList<specModelItem*>& l, QMimeData* d)
{
	Q_UNUSED(l)
	Q_UNUSED(d)
	return ;
}

QStringList specMimeFileImporter::importableTypes() const
{
	return QStringList() << "text/uri-list" ;
}

QList<specModelItem*> specMimeFileImporter::importData(const QMimeData* mime)
{
	specModel* model = qobject_cast<specModel*> (parent()) ;
	QStringList failed ;
	QList<specModelItem*> importedItems ;
	if(!model) return importedItems ;
	//    if (!canImport(mime)) return importedItems ;
	foreach(const QUrl & fileUrl, mime->urls())
	{
		QString filename = fileUrl.toLocalFile() ;
		QFile file(filename) ;
		specFileImportFunction fileImportFunction = fileFilter(filename) ;
		if(!model->acceptableImportFunctions().contains(fileImportFunction))
		{
			failed << filename ;
			break;
		}
		file.open(QFile::ReadOnly | QFile::Text) ;
		importedItems << fileImportFunction(file) ;
	}
	if(!failed.isEmpty())
		QMessageBox::warning(0, tr("Failed to import"),
				     tr("These files could not be imported here:\n")
				     + failed.join("\n")) ;
	return importedItems ;
}

