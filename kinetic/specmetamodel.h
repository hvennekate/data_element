#ifndef SPECMETAMODEL_H
#define SPECMETAMODEL_H

#include "specmodel.h"

class specMetaModel : public specModel
{
	Q_OBJECT
private:
	type typeId() const { return specStreamable::metaModel ; }
	specModel* dataModel ;
//	QHash<specModelItem*,specModelItem*> generate // TODO
public:
	explicit specMetaModel(QObject *parent = 0);
	void setDataModel(specModel*) ;
	specModel* getDataModel() const ;
	~specMetaModel() ;
	// TODO reimplement read/insert mime.
};

#endif // SPECMETAMODEL_H
