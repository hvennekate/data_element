#include "specmetamodel.h"
#include "specfolderitem.h"
#include "specmetaitem.h"

specMetaModel::specMetaModel(QObject *parent) :
	specModel(parent),
	dataModel(0)
{
	setMetaModel(this) ;
}

specMetaModel::~specMetaModel()
{
}

void specMetaModel::setDataModel(specModel *m)
{
	dataModel = m ;
	if (m) m->setMetaModel(this) ;
}

specModel* specMetaModel::getDataModel() const
{
	return dataModel ;
}

bool specMetaModel::insertItems(QList<specModelItem *> list, QModelIndex parent, int row)
{
	foreach(specModelItem* item, list)
		setModels(item) ;
	return specModel::insertItems(list, parent, row) ;
}

void specMetaModel::setModels(specModelItem *item)
{
	for (int i = 0 ; i < item->children() ; ++i)
		setModels(((specFolderItem*) item)->child(i)) ;
	specMetaItem *mitem = dynamic_cast<specMetaItem*>(item) ;
	if (mitem)
		mitem->setModels(this, dataModel) ;
}

QStringList specMetaModel::dataTypes() const
{
    return QStringList() << "x" << "y" << "fit" ;
}

QList<specFileImportFunction> specMetaModel::acceptableImportFunctions() const
{
    return QList<specFileImportFunction>() ;
}
