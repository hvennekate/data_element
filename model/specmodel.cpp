#include "specmodel.h"
#include <QString>
#include <QMimeData>

#include "exportdialog.h"
#include "specmovecommand.h"
#include "specaddfoldercommand.h"
#include "speceditdescriptorcommand.h"
#include "specresizesvgcommand.h"
#include "specfolderitem.h"
#include "specactionlibrary.h"
#include "specmimeconverter.h"
#include "specfolderitem.h"
#include "utility-functions.h"
#include <QtAlgorithms>


// TODO replace isFolder() by addChildren(empty list,0)
bool specModel::lessThanItemPointer(specModelItem* a, specModelItem* b)
{
	return lessThanHierarchies(hierarchy(a), hierarchy(b)) ;
}

bool specModel::lessThanHierarchies(const QVector<int> &aHierarchy, const QVector<int> &bHierarchy)
{
	int i = 0, n = qMin(aHierarchy.size(), bHierarchy.size()) ;
	int haSize = aHierarchy.size() - 1, hbSize = bHierarchy.size() - 1 ;
	while(i < n && aHierarchy[haSize] == bHierarchy[hbSize])
	{
		++i ;
		--haSize ;
		--hbSize ;
	}
	if(i == n) return aHierarchy.size() < bHierarchy.size() ;
	return aHierarchy[haSize] < bHierarchy[hbSize] ;
}

QStringList specModel::dataTypes() const
{
	return QStringList() << "wavenumber" << "signal" << "maximum intensity" ;
}

QModelIndexList ancestry(const QModelIndex& index)
{ return index.parent().isValid() ? (QModelIndexList() << ancestry(index.parent()) << index.parent()) : QModelIndexList() ; }

bool lessThanIndex(const QModelIndex& one, const QModelIndex& two)
{
	if(one.parent() == two.parent())
		return one.row() < two.row() ;
	int levelOne = ancestry(one).size(), levelTwo = ancestry(two).size() ;
	if(levelOne == levelTwo)
		return lessThanIndex(one.parent(), two.parent()) ;
	// old version:
	return levelOne < levelTwo ? lessThanIndex(one, two.parent()) :
	       lessThanIndex(one.parent(), two) ;
	// 	return levelOne < levelTwo ;
}

const QStringList& specModel::descriptors() const
{
	return Descriptors ;
}

bool specModel::isFolder(const QModelIndex& index) const
{ return itemPointer(index)->isFolder() ;}

void specModel::writeToStream(QDataStream& out) const
{
	out << Descriptors ;
	foreach(const specDescriptor& flag, Descriptors) // TODO replace
		out << quint8(0) ;
	out << *root ;
}

void specModel::readFromStream(QDataStream& in)
{
	Descriptors.clear();
	delete root ;
	root = new specFolderItem ;
	in >> Descriptors ;
	in.skipRawData(Descriptors.size()) ; // TODO remove
	in >> *root ;
}

specModelItem* specModel::itemPointer(const QModelIndex& index) const
{ return (index.isValid() && index.internalPointer() != 0 ? (specModelItem*) index.internalPointer() : root) ; } // TODO evtl. als operator= definieren.

specModel::specModel(QObject* par)
	: QAbstractItemModel(par),
	  internalDrop(false),
	  dropBuddy(0),
	  dontDelete(0),
	  metaModel(0),
	  resetPending(false)
{
	root = new specFolderItem ;
	Descriptors += "" ;
	setObjectName("mainModel");
}

specModel::~specModel()
{
	delete root ;
}

bool specModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	Q_UNUSED(orientation)
	if(role == Qt::EditRole)
	{
		Descriptors.replace(section, value.toString()) ;
		emit headerDataChanged(Qt::Horizontal, section, section) ;
		emit headerDataChanged(Qt::Vertical  , section, section) ;
		return true ;
	}
	return false;
}

bool specModel::insertColumns(int column, int count, const QModelIndex& parent)
{
	if (!count) return true ;
	emit beginInsertColumns(parent, column, column + count - 1) ;
	for(QStringList::size_type i = 0 ; i < count ; i++)
		Descriptors.insert(column + i, "") ;
	emit endInsertColumns() ;
	return true ;
}
bool specModel::removeColumns(int column, int count, const QModelIndex& parent)
{
	emit beginRemoveColumns(parent, column, column + count - 1) ;
	for(QStringList::size_type i = 0 ; i < count ; i++)
		Descriptors.removeAt(column) ;
	emit endRemoveColumns() ;
	return true ;
}

void specModel::checkForNewDescriptors()
{
	// get descriptor set
	QSet<QString> descriptorNames = root->descriptorKeys().toSet();

	// check for removed descriptors
	foreach(const QString& descriptorName, Descriptors)
		if (!descriptorNames.contains(descriptorName))
			Descriptors.removeAll(descriptorName) ;
	emit headerDataChanged(Qt::Horizontal, 0, Descriptors.size());

	// check for new descriptors
	QList<QString> newDescriptors = (descriptorNames - Descriptors.toSet()).toList() ;
	qSort(newDescriptors) ;
	int col = columnCount(QModelIndex()) ;
	insertColumns(col, newDescriptors.size(), QModelIndex()) ;
	foreach (const QString& newDescriptor, newDescriptors)
		setHeaderData(col++, Qt::Horizontal, newDescriptor) ;
}

bool specModel::insertItems(QList<specModelItem*> list, QModelIndex parent, int row)  // TODO
{
	row = row < rowCount(parent) ? row : rowCount(parent) ;

	emit layoutAboutToBeChanged() ;

	checkForNewDescriptors() ;
	beginInsertRows(parent, row, row + list.size() - 1);
	bool retVal = itemPointer(parent)->addChildren(list, row) ;
	endInsertRows();
	emit layoutChanged() ;

	return retVal ;
}

int specModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return Descriptors.size() ;
}

int specModel::rowCount(const QModelIndex& parent) const
{
	return itemPointer(parent)->children() ;
}

QModelIndex specModel::parent(const QModelIndex& index) const
{
	specFolderItem* pointer = itemPointer(index)->parent() ;
	if(pointer && pointer != root)
		return createIndex(pointer->parent()->childNo(pointer), 0, pointer);
	return QModelIndex() ;
}

QModelIndex specModel::index(int row, int column, const QModelIndex& parent) const
{
	specModelItem* pointer = itemPointer(parent)->isFolder() ?
				 ((specFolderItem*) itemPointer(parent))->child(row) : 0 ;
	return pointer ? createIndex(row, column, pointer) : QModelIndex() ;
}

Qt::ItemFlags specModel::flags(const QModelIndex& index) const
{
	if(!index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled ;

	Qt::ItemFlags defaultFlags =
			QAbstractItemModel::flags(index)
			| (itemPointer(index)->isFolder()
			   ? Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled
			   : Qt::ItemIsDragEnabled)
			| Qt::ItemIsEditable ;
	return defaultFlags ;
}



QVariant specModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid() || index.row() >= rowCount(parent(index)))
		return QVariant();

	specModelItem* pointer = itemPointer(index) ;
	if (!pointer) return QVariant() ;

	const QString& descriptor = Descriptors[index.column()] ;
	switch(role)
	{
		case Qt::DecorationRole :
			return pointer->indicator(descriptor) ;
		case Qt::DisplayRole :
			if(index.column() < columnCount(index))
				return pointer->descriptor(descriptor) ;
			return "" ;
		case Qt::ForegroundRole :
			return pointer->pen().color() ;
		case Qt::EditRole :
			return pointer->editDescriptor(descriptor) ;
		case Qt::ToolTipRole :
			return pointer->toolTip(descriptor) ;
		case spec::MultiLineRole:
			return pointer->isMultiline(descriptor) ;
		case spec::ActiveLineRole:
			return pointer->activeLine(descriptor) ;
	}
	return QVariant() ;
}

QList<specMimeConverter*> specModel::mimeConverters() const
{
	return findChildren<specMimeConverter*>() ;
}

bool specModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	bool changed = false ;
	if(index.isValid())
	{
		QString desc = Descriptors[index.column()] ;
		specModelItem* pointer = itemPointer(index) ;
		if(role == Qt::EditRole)
		{
			changed = true ;
			specEditDescriptorCommand* command = new specEditDescriptorCommand ;
			command->setParentObject(this) ;
			if(value.canConvert(QVariant::List))
			{
				QList<QVariant> list = value.toList() ;
				if(list.isEmpty())
				{
					delete command ;
					return false ;
				}
				command->setItem(pointer, desc, list[0].toString(), list[1].toInt()) ;
			}
			else
				command->setItem(pointer, desc, value.toString()) ;
			// TODO unite in value-variant
			dropBuddy->push(command) ;
		}
		else if(role == Qt::ForegroundRole)
		{
			itemPointer(index)->setPen(value.value<QPen>()) ;
			changed = true ;
		}
		else if(role == 33)
		{
			changed = true ;
			itemPointer(index)->mergePlotData = value.toBool() ;
		}
	}
	if(changed) emit dataChanged(index, index) ;
	return changed ;
}

QVariant specModel::headerData(int section, Qt::Orientation orientation,
			       int role) const
{
	Q_UNUSED(orientation)
	if(role == Qt::DisplayRole)
		return Descriptors[section] ;
	return QVariant();
}

bool specModel::removeRows(int position, int rows, const QModelIndex& parent)
{
	// TODO check if index is valid?
	// 	if(!(position+rows <= rowCount(index))) return false ; maybe necessary...
	Q_UNUSED(position)
	Q_UNUSED(rows)
	Q_UNUSED(parent)
	if(position < 0 || rows < 1) return false ;
	if(dontDelete)
	{
		dontDelete -- ;
		return true ;
	}

	if(dropBuddy) dropBuddy->deleteInternally(this) ;
	return true ;
}

void specModel::setInternalDrop(bool val)
{
	internalDrop = val ;
}

Qt::DropActions specModel::supportedDropActions() const
{ return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction ; }

void specModel::eliminateChildren(QModelIndexList& list)
{
	for(QModelIndexList::size_type j = 0 ; j < list.size() ; j++)
		if(list[j].column())
			list.removeAt(j--) ;
	for(QModelIndexList::size_type j = 0 ; j < list.size() ; j++)
	{
		QModelIndexList parents = ancestry(list[j]) ;
		for(QModelIndexList::size_type i = 0 ; i < list.size() ; i++)
		{
			if(parents.contains(list[i]))
			{
				list.removeAt(j--) ;
				break ;
			}
		}
	}
}

void specModel::eliminateChildren(QList<specModelItem*>& l)
{
	QSet<specModelItem*> allChildren ;
	for(QList<specModelItem*>::iterator it = l.begin() ; it != l.end() ; ++it)
	{
		specFolderItem* folder = dynamic_cast<specFolderItem*>(*it) ;
		if(folder)
			foreach(specModelItem * pointer, folder->childrenList())
			allChildren << pointer ;
	}

	QList<specModelItem*> filtered ;
	for(QList<specModelItem*>::iterator it = l.begin() ; it != l.end() ; ++it)
		if(!allChildren.contains(*it))
			filtered << *it ;

	l.swap(filtered);
}

QMimeData* specModel::mimeData(const QModelIndexList& indices) const
{
	QMimeData* mimeData = new QMimeData() ;
	QModelIndexList list = indices ;
	eliminateChildren(list) ;
	if(dropBuddy)
		dropBuddy->setLastRequested(list) ;

	QList<specModelItem*> pointers = pointerList(list) ;
	foreach(specMimeConverter * converter, mimeConverters())
	converter->exportData(pointers, mimeData) ;

	return mimeData ;
}

bool specModel::mimeAcceptable(const QMimeData* data) const
{
	foreach(specMimeConverter * converter, mimeConverters())
	if(converter->canImport(data))
		return true ;
	return false ;
}

bool specModel::dropMimeData(const QMimeData* data,
			     Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	Q_UNUSED(column)
	if(action == Qt::IgnoreAction)
		return true;

	row = (row != -1 ? row : rowCount(parent));
	if(internalDrop && dropBuddy)
	{
		dontDelete = dropBuddy->moveInternally(parent, row, this) ;
		internalDrop = false ;
		return true ;
	}


	QList<specModelItem*> newItems ;
	foreach(specMimeConverter * converter, mimeConverters())
	{
		if(converter->canImport(data))
		{
			newItems = converter->importData(data) ;
			break ;
		}
	}

	if(!newItems.isEmpty())
	{
		insertItems(newItems, parent, row) ;
		if(dropBuddy)
		{
			specAddFolderCommand* command = new specAddFolderCommand ;
			command->setParentObject(this) ;
			command->setItems(newItems) ;
			command->setText(tr("Insert items")) ;
			dropBuddy->push(command);
		}
	}
	return true ;
}

#ifdef QT_DEBUG
void printModelContentSummary(specFolderItem* folder, QString indent)
{
	int n = 0 ;
	if(folder->parent()) n = folder->parent()->childNo(folder) ;
	qDebug() << indent + QString::number(n) + " contains " + QString::number(folder->children()) ;
	indent += "  " ;
	for(int i = 0 ; i < folder->children() ; ++i)
	{
		if(folder->child(i)->isFolder())
			printModelContentSummary((specFolderItem*)(folder->child(i)), indent);
	}
}

#endif

specModelItem* specModel::itemPointer(const QVector<int>& indexes) const
{
	specModelItem* pointer = root ;
#ifdef MODEL_DEBUG
	qDebug() << "seeking pointer for item" << indexes ;
	qDebug() << "============ Model summary: ============" ;
	printModelContentSummary((specFolderItem*) root, "");
	qDebug() << "========================================" ;
#endif
	for(int i = indexes.size() - 1 ; i >= 0 && pointer && pointer->isFolder() ; --i)
		pointer = ((specFolderItem*) pointer)->child(indexes[i]) ;
	return pointer ;
}

specFolderItem *specModel::parentPointer(const QVector<int> & indexes) const
{
	specModelItem* pointer = root ;
	for(int i = indexes.size() - 1 ; i >= 1 && pointer && pointer->isFolder() ; --i)
		pointer = ((specFolderItem*) pointer)->child(indexes[i]) ;
	return dynamic_cast<specFolderItem*>(pointer) ;
}

QVector<int> specModel::hierarchy(specModelItem* item)
{
	QVector<int> retVal ;
	specFolderItem* parent ;
	while((parent = item->parent()))
	{
		retVal << parent->childNo(item) ;
		item = parent ;
	}
	return retVal ;
}

QVector<int> specModel::hierarchy(const QModelIndex& index)
{
	QVector<int> retVal ;
	QModelIndex item = index ;
	while(item.isValid())
	{
		retVal << item.row();
		item = item.parent() ;
	}
	return retVal ;
}

void specModel::setDropBuddy(specActionLibrary* buddy)
{
	dropBuddy = buddy ;
	connect(dropBuddy, SIGNAL(stackIndexChanged()), this, SLOT(signalEndReset())) ;
}

QModelIndex specModel::index(const QVector<int>& ancestry, int column) const
{
	QModelIndex returnIndex = QModelIndex() ;
	for(int i = ancestry.size() - 1 ; i >= 0 ; --i)  // TODO consider precaution if invalid at any stage
		returnIndex = index(ancestry[i], column, returnIndex) ;
	return returnIndex ;
}

bool specModel::contains(specModelItem* parent) const
{
	while(parent->parent()) parent = parent->parent() ;
	return parent == root ;
}

QModelIndex specModel::index(specModelItem* pointer, int column) const
{
	if(!pointer) return QModelIndex() ;
	// Test if item is indeed part of THIS model
	if(!contains(pointer)) return QModelIndex() ;

	// Generate genealogy to find parent
	return index(hierarchy(pointer), column) ;
}

QList<specModelItem*> specModel::pointerList(const QModelIndexList& indexes) const
{
	QList<specModelItem*> returnList ;
	for(int i = 0 ; i < indexes.size() ; ++i)
		returnList << itemPointer(indexes[i]) ;
	return returnList ;
}

QModelIndexList specModel::indexList(const QList<specModelItem*>& pointers) const
{
	QModelIndexList returnList ;
	for(int i = 0 ; i < pointers.size() ; ++i)
		returnList << index(pointers[i]) ;
	return returnList ;
}

void specModel::svgMoved(specCanvasItem* i, int point, double x, double y)
{
	specSVGItem* item = dynamic_cast<specSVGItem*>(i) ;
	if(!item || !dropBuddy) return ;

	specResizeSVGcommand* command = new specResizeSVGcommand ;
	command->setParentObject(this) ;
	item->pointMoved(point, x, y) ;
	command->setItem(item, item->getBounds()) ;
	command->undo();
	command->setText(tr("Resize/move SVG item"));
	dropBuddy->push(command) ;
}

void specModel::setMetaModel(specMetaModel* m)
{
	metaModel = m ;
}

specMetaModel* specModel::getMetaModel() const
{
	return metaModel ;
}

void specModel::signalChanged(const QModelIndex& i)
{

	QModelIndex begin = index(i.row(), 0, i.parent()),
		    end = index(i.row(), columnCount(i) - 1, i.parent()) ;
	signalChanged(begin, end) ;
	checkForNewDescriptors();
	emit dataChanged(begin, end) ;
}

void specModel::signalChanged(QModelIndex begin, QModelIndex end)
{
	while(begin.parent() != end.parent())
	{
		if(begin.isValid())
			begin = begin.parent() ;
		else
			end = QModelIndex() ;
	}

	QModelIndex i(begin) ;
	QList<specModelItem*> items ;

	do
	{
		specModelItem* pointer = itemPointer(i) ;
		items << pointer;
		if(pointer->children())
		{
			QModelIndex firstChild = index(0, 0, i),
				    lastChild = index(pointer->children() - 1, 0, i) ;
			signalChanged(firstChild, lastChild) ;
		}
		i = i.sibling(i.row() + 1, 0) ;

	}
	while(i != end && i.isValid()) ;

	checkForNewDescriptors() ;
	emit dataChanged(begin, index(end.row(), Descriptors.count(), end.parent())) ;
}

QList<specModelItem*> specModel::expandFolders(const QList<specModelItem*>& list, bool retain)
{
	QList<specModelItem*> result ;
	foreach(specModelItem * item, list)
	{
		if(item->isFolder())
		{
			if(retain) result << item ;
			result << expandFolders(((specFolderItem*) item)->childrenList(), retain) ;
		}
		else
			result << item ;
	}
	return result ;
}

QStringList specModel::mimeTypes() const
{
	QStringList types ;
	foreach(specMimeConverter * converter, mimeConverters())
	types << converter->importableTypes() ;
	return types ;
}

void specModel::deleteDescriptor(const QString& key)
{
	int i = Descriptors.indexOf(key) ;
	if(i < Descriptors.size() && i >= 0)
		Descriptors.takeAt(i) ;
	root->deleteDescriptor(key) ;
}

void specModel::renameDescriptors(const QMap<QString, QString>& map)
{
	for(QStringList::iterator i = Descriptors.begin() ; i != Descriptors.end() ; ++i)
		if(map.contains(*i))
			*i = map[*i] ;
	root->renameDescriptors(map) ;
}

void specModel::dumpDescriptor(QList<specDescriptor>& destination, const QString& key) const
{
	root->dumpDescriptor(destination, key) ;
}

void specModel::restoreDescriptor(qint16 position, QListIterator<specDescriptor>& origin, const QString& key)
{
	if(!Descriptors.contains(key))
		Descriptors.insert(position, key) ;
	root->restoreDescriptor(origin, key) ;
}

QList<specFileImportFunction> specModel::acceptableImportFunctions() const
{
	return QList<specFileImportFunction>()
	       << readHVFile
	       << readPEFile
	       << readPEBinary
	       << readJCAMPFile
	       << readSKHIFile
	       << readXYFILE ;
}

QValidator* specModel::createValidator(const QModelIndex& i) const // TODO obsolete
{
	Q_UNUSED(i)
	return 0 ;
}

void specModel::signalBeginReset()
{
	if(resetPending) return ;
	resetPending = true ;
	beginResetModel();
}

void specModel::signalEndReset()
{
	if(resetPending)
	{
		endResetModel();
		checkForNewDescriptors();
	}
	resetPending = false ;
}
