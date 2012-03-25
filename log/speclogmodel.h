#ifndef SPECLOGMODEL_H
#define SPECLOGMODEL_H
#include "specmodel.h"

QList<specModelItem*> specLogToData(const QMimeData*) ;

class specLogModel : public specModel
{
private:
    Q_OBJECT
	specModel *partner ;
public:
	specLogModel(specModel *dataModel, QObject *parent = 0);

signals:

public slots:

};

#endif // SPECLOGMODEL_H
