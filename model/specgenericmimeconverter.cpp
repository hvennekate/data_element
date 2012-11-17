#include "specgenericmimeconverter.h"
#include "specmetamodel.h"
#include "speclogmodel.h"
#include "specmodel.h"
#include <QMimeData>

specGenericMimeConverter::specGenericMimeConverter(QObject *parent)
	: specMimeConverter(parent)
{
}

specModelItem* specGenericMimeConverter::factory(const type &t) const
{
	return specModelItem::itemFactory(t) ;
}

QList<specModelItem*> specGenericMimeConverter::importData(const QMimeData *data)
{
	QList<specModelItem*> items ;
	QByteArray ba(data->data(ownType())) ;
	QDataStream in(&ba,QIODevice::ReadOnly) ;
	while (!in.atEnd())
		items << (specModelItem*) produceItem(in) ;
	items.removeAll(0) ;
	return items ;
}

void specGenericMimeConverter::exportData(QList<specModelItem *> &items, QMimeData* data)
{
	QByteArray ba ;
	QDataStream out(&ba, QIODevice::WriteOnly) ;
	foreach(specModelItem* item, items)
		out << *item ;
	data->setData(ownType(),ba) ;
}

QString specGenericMimeConverter::ownType() const
{
	specModel *model = 0 ;
	if ((model = qobject_cast<specLogModel*>(parent())))
		return "application/spec.log.items" ;
	if ((model = qobject_cast<specMetaModel*>(parent())))
		return "application/spec.meta.items" ;
	if ((model = qobject_cast<specModel*>(parent())))
		return "application/spec.spectral.items" ;
	return QString() ;
}

bool specGenericMimeConverter::canImport(const QStringList &types)
{
	QString type = ownType() ;
	if (type != "")
		return types.contains(type) ;
	return false ;
}

QStringList specGenericMimeConverter::importableTypes() const
{
    return specMimeConverter::importableTypes() << ownType() ;
}
