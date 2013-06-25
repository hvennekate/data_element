#include "speclogmodel.h"
#include <QFile>
#include <QFileDialog>
#include "utility-functions.h"

specLogModel::specLogModel(QObject *parent)
	: specModel(parent)
{
	setObjectName("logModel");
}

QStringList specLogModel::dataTypes() const
{
    return QStringList() ;
}

QList<specFileImportFunction> specLogModel::acceptableImportFunctions() const
{
    return QList<specFileImportFunction>() << readLogFile ;
}
