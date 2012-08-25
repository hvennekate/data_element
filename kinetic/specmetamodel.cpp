#include "specmetamodel.h"

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
