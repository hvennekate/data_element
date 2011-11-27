#ifndef SPECMETAMODEL_H
#define SPECMETAMODEL_H

#include "specmodel.h"

class specMetaModel : public specModel
{
	Q_OBJECT
private:
	QHash<specModelItem*,specModelItem*> generate
public:
	explicit specMetaModel(QObject *parent = 0);
	~specMetaModel() ;
	// TODO reimplement read/insert mime.
};

#endif // SPECMETAMODEL_H
