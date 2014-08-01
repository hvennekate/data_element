#ifndef SPECDATATOKINETICMIME_H
#define SPECDATATOKINETICMIME_H

#include "specmimeconverter.h"

class specMetaModel ;
class specModel ;

class specDataToKineticMime : public specMimeConverter
{
	Q_OBJECT
private:
	bool dataParent() const ;
	bool metaParent() const ;
	specModel* dataModel() const ;
	specMetaModel* metaModel() const ;
	QString ownType() const ;
public:
	explicit specDataToKineticMime(QObject *parent = 0);
	QList<specModelItem*> importData(const QMimeData*) ;
	void exportData(QList<specModelItem*>&, QMimeData*) ;
	QStringList importableTypes() const ;
};

#endif // SPECDATATOKINETICMIME_H
