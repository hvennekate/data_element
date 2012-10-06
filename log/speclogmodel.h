#ifndef SPECLOGMODEL_H
#define SPECLOGMODEL_H
#include "specmodel.h"

class specLogModel : public specModel
{
private:
    Q_OBJECT
	type typeId() const { return specStreamable::logModel ; }
public:
	explicit specLogModel(QObject *parent = 0);

signals:

public slots:

};

#endif // SPECLOGMODEL_H
