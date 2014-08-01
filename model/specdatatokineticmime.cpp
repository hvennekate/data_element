#include "specdatatokineticmime.h"
#include <QMimeData>
#include "specmetamodel.h"
#include "speclogmodel.h"
#include "specmetaitem.h"
#include "specgenericmimeconverter.h"
#include <QApplication>

typedef QPair<quint64, QVector<int> > pointerHierarchy ;

QString specDataToKineticMime::ownType() const
{
	return "application/spec.datalink.items" ;
}

specDataToKineticMime::specDataToKineticMime(QObject *parent) :
	specMimeConverter(parent)
{
}

bool specDataToKineticMime::metaParent() const
{
	qDebug() << "Testing meta parent:" << qobject_cast<specMetaModel*>(parent()) ;
	if (qobject_cast<specMetaModel*>(parent()))
		    qDebug() << qobject_cast<specMetaModel*>(parent())->getDataModel() ;
	qDebug() << (qobject_cast<specMetaModel*>(parent())
		    && qobject_cast<specMetaModel*>(parent())->getDataModel()) ;
	return qobject_cast<specMetaModel*>(parent())
			&& qobject_cast<specMetaModel*>(parent())->getDataModel() ;
}

bool specDataToKineticMime::dataParent() const
{
	specModel* model = qobject_cast<specModel*>(parent()) ;
	if (model && model->getMetaModel() != model) return true ;
	return false ;
}

specMetaModel* specDataToKineticMime::metaModel() const
{
	if (metaParent()) return qobject_cast<specMetaModel*>(parent()) ;
	if (dataParent()) return qobject_cast<specModel*>(parent())->getMetaModel() ;
	return 0 ;
}

specModel* specDataToKineticMime::dataModel() const
{
	if (dataParent()) return qobject_cast<specModel*>(parent()) ;
	if (metaParent()) return qobject_cast<specMetaModel*>(parent())->getDataModel() ;
	return 0 ;
}

QStringList specDataToKineticMime::importableTypes() const
{
	if (metaParent()) return QStringList() << ownType() ;
	return QStringList() ;
}

void specDataToKineticMime::exportData(QList<specModelItem *> &items, QMimeData *data)
{
	if (!dataParent()) return ;
	QByteArray ba ;
	QDataStream out(&ba, QIODevice::WriteOnly) ;
	// sessionId
	out << qApp->sessionId() ;
	// dataModel pointer
	specModel* model = dataModel() ;
	out << (quint64) model ; //TODO also compare file name
	// for each item:  pointer and hierarchy
	QVector<pointerHierarchy> itemList ;
	foreach(specModelItem* item, items)
		itemList << qMakePair((quint64) item, model->hierarchy(item)) ;
	out << itemList ;
	data->setData(ownType(), ba) ;
	qDebug() << "exported data items:" << items ;
	qDebug() << "list:" << itemList ;
	qDebug() << "model pointer:" << (quint64) model << (quint64) metaModel();
}

QList<specModelItem*> specDataToKineticMime::importData(const QMimeData *data)
{
	// Session checken.
	QByteArray ba = data->data(ownType()) ;
	QDataStream in(&ba, QIODevice::ReadOnly) ;
	QString sessionId ;
	in >> sessionId ;
	if (qApp->sessionId() != sessionId) return QList<specModelItem*>() ;

	quint64 oldModel = 0 ;
	in >> oldModel ;
	qDebug() << "read model pointer:" << oldModel << (quint64) dataModel() << (quint64) metaModel() ;
	QVector<pointerHierarchy> itemList ;
	in >> itemList ;

	// Check old Model and presence of items
	specModel* model = dataModel() ;
	if (!model) return  QList<specModelItem*>() ;
	bool itemsExistAlready = (oldModel == (quint64) model) ;
	qDebug() << "read list:" << itemList ;
	if (itemsExistAlready)
	{
		foreach(pointerHierarchy pH, itemList)
		{
			if((quint64)(model->itemPointer(pH.second)) != pH.first)
			{
				qDebug() << "mismatch:"
					 << (quint64)(model->itemPointer(pH.second))
					 << pH.first ;
				itemsExistAlready = false ;
				break ;
			}
		}
	}

	// Generate item pointer list
	QList<specModelItem*> dataItems ;
	if (itemsExistAlready)
	{
		foreach(pointerHierarchy pH, itemList)
			dataItems << model->itemPointer(pH.second) ;
		qDebug() << "Items existed already:" << dataItems ;
	}
	else
	{
		// insert items from other meta data
		QString dataFolderName = QObject::tr("Inserted by meta window") ;

		QModelIndex firstItem = model->index(0,0) ;
		if(
				firstItem.isValid()
				&& model->isFolder(firstItem)
				&& model->data(firstItem, Qt::EditRole) == dataFolderName)
		{
			// Insert mime data into existing folder
			if (
					!model->dropMimeData(data,
							     Qt::CopyAction,
							     model->rowCount(firstItem),
							     0,
							     firstItem)
					)
				return QList<specModelItem*>()  ;
		}
		else // Create insertion folder, place mime data there, then
			// repack as mime and insert into model
		{
			specModel tempModel ;
			specGenericMimeConverter *genericConverter = new specGenericMimeConverter(&tempModel) ;
			specFolderItem* newFolder = new specFolderItem(0, dataFolderName) ;
			newFolder->addChildren(genericConverter->importData(data)) ;
			QMimeData *newData = new QMimeData ;
			genericConverter->exportData(QList<specModelItem*>() << newFolder,
						     newData) ;
			if (
					!model->dropMimeData(newData,
							     Qt::CopyAction,
							     0,
							     0,
							     QModelIndex())
					)
			{
				delete newData ;
				return QList<specModelItem*>() ;
			}
			delete newData ;
		}
		// At this point, the first folder should contain
		// the data items as its last n items.
		specFolderItem* firstFolder = dynamic_cast<specFolderItem*>(model->itemPointer(model->index(0,0))) ;
		if (!firstFolder) return QList<specModelItem*>() ;

		// so we get the last n items:
		dataItems = firstFolder->childrenList().mid(firstFolder->children() - itemList.size()) ;
		if (dataItems.size() != itemList.size())
			return QList<specModelItem*>() ;
		qDebug() << "Items newly inserted" << dataItems ;
	}

	specMetaItem* newItem = new specMetaItem ;
	foreach(specModelItem* item, dataItems)
		newItem->connectServer(item) ;

	// Set some descriptors for convenience
	double min = INFINITY, max = -INFINITY ;
	foreach(specModelItem* item, dataItems)
	{
		min = qMin(min, item->minXValue()) ;
		max = qMax(max, item->maxXValue()) ;
	}

	newItem->changeDescriptor("x", "x") ;
	newItem->changeDescriptor("y", "y") ;
	newItem->changeDescriptor("variables", "x = \"Zeit\"\ny = i"
				  + QString::number(min) + ":"
				  + QString::number(max)) ;
	newItem->revalidate();
	return QList<specModelItem*>() << newItem ;
}
