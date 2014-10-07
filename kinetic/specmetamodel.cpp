#include "specmetamodel.h"
#include "specfolderitem.h"
#include "specmetaitem.h"

specMetaModel::specMetaModel(QObject* parent) :
	specModel(parent),
	dataModel(0)
{
	setMetaModel(this) ;
	setObjectName("metaModel");
}

specMetaModel::~specMetaModel()
{
}

void specMetaModel::setDataModel(specModel* m)
{
	dataModel = m ;
	if(m) m->setMetaModel(this) ;
}

specModel* specMetaModel::getDataModel() const
{
	return dataModel ;
}

bool specMetaModel::insertItems(QList<specModelItem*> list, QModelIndex parent, int row)
{
	foreach(specModelItem * item, list)
	setModels(item) ;
	return specModel::insertItems(list, parent, row) ;
}

void specMetaModel::setModels(specModelItem* item)
{
	for(int i = 0 ; i < item->children() ; ++i)
		setModels(((specFolderItem*) item)->child(i)) ;
	specMetaItem* mitem = dynamic_cast<specMetaItem*>(item) ;
	if(mitem)
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

QVariant specMetaModel::data(const QModelIndex& index, int role) const
{
	specModelItem* pointer = itemPointer(index) ;
	if (pointer
			&& Qt::BackgroundRole == role
			&& index.column() >= 0
			&& index.column() < descriptors().size()
			&& !pointer-> descriptorKeys().contains(descriptors().at(index.column())))
	{
		return QBrush(Qt::gray) ;
	}
	if (spec::variableNamesRole == role
			|| spec::variablesInFormulaeRole == role
			|| spec::fitVariablesInFormulaRole == role
			|| spec::fitVariablesRole == role)
	{
		specMetaItem* item = dynamic_cast<specMetaItem*>(itemPointer(index)) ;
		if (item)
		{
			switch(role)
			{
			case spec::variableNamesRole:
				return QStringList(item->variableNames().toList()) ;
			case spec::variablesInFormulaeRole:
				return QStringList(item->variablesInFormulae().toList()) ;
			case spec::fitVariablesRole:
				return item->fitVariableNames() ;
			case spec::fitVariablesInFormulaRole:
				return item->fitFormulaVariableNames() ;
			}

		}
	}
	return specModel::data(index, role) ;
}
