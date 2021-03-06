#ifndef SPECLOGMODEL_H
#define SPECLOGMODEL_H
#include "specmodel.h"

class specLogModel : public specModel
{
private:
	Q_OBJECT
	type typeId() const { return specStreamable::logModel ; }
	QStringList dataTypes() const ;
public:
	explicit specLogModel(QObject* parent = 0);
	QList<specFileImportFunction> acceptableImportFunctions() const ;
};

#endif // SPECLOGMODEL_H
