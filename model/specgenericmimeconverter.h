#ifndef SPECGENERICMIMECONVERTER_H
#define SPECGENERICMIMECONVERTER_H
#include "specmimeconverter.h"
#include "specstreamable.h"
#include "specmodelitem.h"

class specGenericMimeConverter : public specMimeConverter, private specStreamable
{
	Q_OBJECT
private:
	specModelItem* factory(const type &t) const ;
	void writeToStream(QDataStream &out) const { Q_UNUSED(out) }
	void readFromStream(QDataStream &in) { Q_UNUSED(in) }
	specStreamable::type typeId() const { return specStreamable::none ;}
	QString ownType() const ;
public:
	explicit specGenericMimeConverter(QObject *parent = 0);
	QList<specModelItem*> importData(const QMimeData *);
	void exportData(QList<specModelItem *> &, QMimeData*) ;
	bool canImport(const QStringList&) ;
};

#endif // SPECGENERICMIMECONVERTER_H
