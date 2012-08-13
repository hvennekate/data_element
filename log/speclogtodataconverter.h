#ifndef SPECLOGTODATACONVERTER_H
#define SPECLOGTODATACONVERTER_H
#include "specmimeconverter.h"
#include <QFileDialog>
#include "specstreamable.h"
#include "specmodelitem.h"

class specLogToDataConverter : public specMimeConverter, private specStreamable
{
	Q_OBJECT
private:
	specModelItem* getData(specModelItem*) ;
	QDir currentDirectory ;
	specModelItem* factory(const type &t) const ;
	void writeToStream(QDataStream &out) const { Q_UNUSED(out) }
	void readFromStream(QDataStream &in) { Q_UNUSED(in) }
	specStreamable::type typeId() const { return specStreamable::none ;}
	void toStream(specModelItem *, QDataStream &) ;
public:
	explicit specLogToDataConverter(QObject *parent = 0);
	QList<specModelItem*> importData(const QMimeData *) ;
	void exportData(QList<specModelItem *> &, QMimeData *) ;
	bool canImport(const QStringList &) ;
};

#endif // SPECLOGTODATACONVERTER_H
