#include <specmodel.h>
#include <QIcon>
#include <QTextStream>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <utility-functions.h>

#include <QPersistentModelIndex>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QGridLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QLineEdit>
#include "specdataitem.h"
#include "exportdialog.h"
#include <QTime>
#include "specmovecommand.h"
#include "specaddfoldercommand.h"
#include "speceditdescriptorcommand.h"
#include "specresizesvgcommand.h"
// TODO replace isFolder() by addChildren(empty list,0)

bool specModel::itemsAreEqual(QModelIndex& first, QModelIndex& second, const QList<QPair<QStringList::size_type, double> >& criteria)
{
	if (!first.isValid() || !second.isValid())
		return false ;
	// TODO may need to be revised if descriptor contains actual numeric value, not QString
	bool equal = true ;
	for(QList<QPair<QStringList::size_type, double> >::size_type i = 0 ; i < criteria.size() ; i++)
	{
		QStringList::size_type descriptor = criteria[i].first ;
		double tolerance = criteria[i].second ;
		if (DescriptorProperties[descriptor] & spec::numeric)
		{
			double a = itemPointer(first)->descriptor(Descriptors[descriptor]).toDouble(),
				b = itemPointer(second)->descriptor(Descriptors[descriptor]).toDouble() ;
			equal &= b-tolerance <= a && a <= b+tolerance ;
		}
		else
			equal &= itemPointer(first)->descriptor(Descriptors[descriptor]) ==
				itemPointer(second)->descriptor(Descriptors[descriptor]) ;
	}
	return equal ;
}

bool specModel::getMergeCriteria(QList<QPair<QStringList::size_type, double> >& toCompare)
{ // TODO include smoother
	QDialog *descriptorMatch = new QDialog() ;
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel) ;
	connect(buttons,SIGNAL(accepted()), descriptorMatch, SLOT(accept()));
	connect(buttons,SIGNAL(rejected()), descriptorMatch, SLOT(reject()));
	QGridLayout *descriptorLayout = new QGridLayout ;
	QVBoxLayout *dialogLayout = new QVBoxLayout ;
	//TODO Label fuer tolerance ...
	for(QStringList::size_type i = 0 ; i < Descriptors.size() ; i++)
	{
		descriptorLayout->addWidget(new QCheckBox(Descriptors[i]),i,0) ;
// 		((QCheckBox*) descriptorLayout->itemAt(descriptorLayout->count()-1)->widget())->setCheckState(Qt::Checked) ;
		if(DescriptorProperties[i] & spec::numeric)
		{
			QLineEdit *validatorLine = new QLineEdit("0") ;
			validatorLine->setValidator(new QDoubleValidator(validatorLine)) ;
			((QDoubleValidator*) validatorLine->validator())->setBottom(0) ;
			descriptorLayout->addWidget(validatorLine,i,1) ;
		}
	}
	QScrollArea *scrollArea = new QScrollArea ;
	QWidget *areaWidget = new QWidget ;
	areaWidget->setLayout(descriptorLayout) ;
	scrollArea->setWidget(areaWidget) ;
	dialogLayout->addWidget(scrollArea) ;
	dialogLayout->addWidget(buttons) ;
	descriptorMatch->setLayout(dialogLayout) ;
	bool retval = descriptorMatch->exec() ;
	
// 	QList<QPair<QStringList::size_type, double> > toCompare ;
	for (int i = 0 ; i < descriptorLayout->count() ; i++)
	{
		int row, column, rowspan, columnspan ;
		descriptorLayout->getItemPosition(i, &row, &column, &rowspan, &columnspan) ;
		if (column == 0 && ((QCheckBox*) descriptorLayout->itemAt(i)->widget())->checkState() == Qt::Checked)
			toCompare << qMakePair(row,
				DescriptorProperties[row] & spec::numeric ?
						((QLineEdit*) descriptorLayout->itemAt(i+1)->widget())->text().toDouble() : 0.) ;
	}
	
	return retval ;
}

bool specModel::exportData(QModelIndexList& list)
{
	QFile *exportFile = new QFile(QFileDialog::getSaveFileName(0,"File name","","ASCII files (*.asc)")) ;
	exportDialog *exportFormat = new exportDialog(&Descriptors) ;
	if ( exportFile->fileName() == "" || ! exportFormat->exec() ) return false ;
	exportFile->open(QIODevice::WriteOnly | QIODevice::Text) ;
	QTextStream out(exportFile) ;
	QList<QPair<bool,QString> > headerFormat = exportFormat->headerFormat() ;
	QList<QPair<spec::value,QString> > dataFormat = exportFormat->dataFormat() ;
	if (list.isEmpty())
		for (int i = 0 ; i < root->children() ; i++)
			((specFolderItem*) root)->child(i)->exportData(headerFormat, dataFormat, out) ; // TODO make root specFolderItem
	else
		for (int i = 0 ; i < list.size() ; i++)
			itemPointer(list[i])->exportData(headerFormat, dataFormat, out) ;
	exportFile->close() ;
	return true ;
}

QModelIndexList ancestry(const QModelIndex& index)
{ return index.parent().isValid() ? (QModelIndexList() << ancestry(index.parent()) << index.parent() ) : QModelIndexList() ; }

bool lessThanIndex(const QModelIndex& one, const QModelIndex& two)
{
	if (one.parent() == two.parent())
		return one.row() < two.row() ;
	int levelOne = ancestry(one).size(), levelTwo = ancestry(two).size() ;
	if (levelOne == levelTwo)
		return lessThanIndex(one.parent(), two.parent()) ;
// old version:
	return levelOne < levelTwo ? lessThanIndex(one,two.parent()) :
			lessThanIndex(one.parent(),two) ;
// 	return levelOne < levelTwo ;
}

QModelIndexList specModel::mergeItems(QModelIndexList& list)
{
	QList<QPair<QStringList::size_type, double> > criteria ;
	if (getMergeCriteria(criteria))
	{
		eliminateChildren(list) ;
		return merge(list, criteria) ; // TODO more sophisiticated solution
	}
	return list ;
}

QModelIndexList specModel::allChildren(const QModelIndex& parent) const
{
	if (!parent.isValid() || !isFolder(parent))
		return QModelIndexList() ;
	QModelIndexList list ;
	for (int i = 0 ; i < rowCount(parent) ; i++)
		list << this->index(i,0,parent) ;
	return list ;
}

const QStringList& specModel::descriptors() const
{
	return Descriptors ;
}

const QList<spec::descriptorFlags>& specModel::descriptorProperties() const
{
	return DescriptorProperties ;
}

QModelIndexList specModel::merge(QModelIndexList& list, const QList<QPair<QStringList::size_type, double> >& criteria)
{
	for (QModelIndexList::size_type i = 0 ; i < list.size() ; i++)
		if (isFolder(list[i]))
			list << allChildren(list.takeAt(i--)) ;
	qSort(list.begin(),list.end(), lessThanIndex) ;
	
	QList<QList<QPersistentModelIndex> > mergeLists ;
// Setup lists of items to merge
	while (!list.isEmpty())
	{
		QList<QPersistentModelIndex> mergeList ;
		QModelIndex compareIndex = list.first() ;
		while(! dynamic_cast<specDataItem*>(itemPointer(list.first())))
			list.takeFirst() ; // Remove all non-data items
		mergeList << list.takeFirst() ; // current item is initial entry
		for (QModelIndexList::size_type i = 0 ; i < list.size() ; i++)
			if (itemsAreEqual(compareIndex, list[i], criteria) && dynamic_cast<specDataItem*>(itemPointer(list[i])))
				mergeList << list.takeAt(i--) ; // add more items to list if they match according to predefined criteria
		mergeLists << mergeList ; 
	}
// Create new, merged items
	QList<specModelItem*> newItems ;
	foreach(QList<QPersistentModelIndex> mergeList, mergeLists)
	{
		//QHash<QString,specDescriptor> temp ;
		//temp.insert(QString(""),specDescriptor(QString(""),spec::editable)) ;
		QByteArray *firstItemData = new QByteArray() ;
		QDataStream outStream(firstItemData,QIODevice::WriteOnly) ;
		outStream << *(itemPointer(mergeList.takeFirst())) ;
		QDataStream inStream(firstItemData,QIODevice::ReadOnly) ;
		type t ;
		inStream >> t ;
		specModelItem *newItem = specModelItem::itemFactory(t) ;
		inStream >> *newItem ;
		newItems << newItem ;
//		newItems << new specDataItem(* (specDataItem*) itemPointer(mergeList.takeFirst())) ;
		//newItems << new specDataItem(QList<specDataPoint>(),  temp) ;// TODO Default constructor?
		specDataItem *pointer = (specDataItem*) newItems.last() ;
		foreach(QModelIndex index, mergeList)
		{
			specDataItem *currentItem = (specDataItem*) itemPointer(index) ;
//			if (fitInterpolated // fitting is desired
//			    && !(currentItem->maxXValue() < pointer->minXValue()) // range is not left...
//			    && !(currentItem->minXValue() > pointer->maxXValue())) // ... or right of current  TODO: sort Items before merging
//			{
//				QVector<double> x, deviation ;
//				for (int i = 0 ; i < currentItem->dataSize() ; i++)
//				{
//// TODO BAUSTELLE
//				}
//			}
			*pointer += *(currentItem) ;
//			pointer->changeDescriptor("",pointer->descriptor("").append(currentItem->descriptor(""))) ;
		}
		pointer->flatten() ; // TODO: flatten with smoother
	}
// Determine insert position
// Criteria for determination: highest level, lowest row number ... TODO should be the first item in the list?
	QList<QPair<QPersistentModelIndex,int> > designatedInsertPositions ;
	for(QModelIndexList::size_type i = 0 ; i < mergeLists.size(); i++)
	{
		QPersistentModelIndex insertParent = mergeLists[i].first().parent() ;
		int insertRow = mergeLists[i].first().row() ;
		while(!mergeLists[i].isEmpty())
		{
			QModelIndex index = mergeLists[i].takeLast() ;
			
			if (index == insertParent) // precaution if designated parent is to be deleted (move up one level)  TODO maybe add condition that "index" is contained in "insertParent"'s ancestry
			{
				insertParent = index.parent() ;
				insertRow = index.row() ;
			}
// If containing folder has only to be removed entry left, append it to list so it will be removed in the next pass
			if (rowCount(parent(index)) == 1 && parent(index).isValid())
				mergeLists[i] << parent(index) ;
			if (parent(index) == insertParent && index.row() < insertRow)
				insertRow-- ;
			list << index ;
// 				removeRows(index.row(),1,parent(index)) ;
		}
		designatedInsertPositions << QPair<QPersistentModelIndex,int>(insertParent,insertRow) ;
// 		if(itemPointer(insertParent)->addChild(newItems[i],insertRow))
// 			returnList << index(insertRow,0,insertIndex) ;
	}
	
	// remove old items
	qSort(list.begin(),list.end(),lessThanIndex) ;
	QModelIndex currentParent ;
	int rowsToRemove = 0, lastToRemove = -1 ;
	while (!list.isEmpty())
	{
		QModelIndex index = list.takeLast() ;
		if (parent(index) != currentParent || index.row() != lastToRemove - rowsToRemove)
		{
			removeRows(lastToRemove-rowsToRemove+1, rowsToRemove, currentParent) ;
			currentParent = parent(index) ;
			rowsToRemove = 0 ;
			lastToRemove = index.row() ;
		}
		rowsToRemove ++ ;
	}
	removeRows(lastToRemove-rowsToRemove+1,rowsToRemove,currentParent) ;
	
	// look for items of same parent and insert them into model
	QModelIndexList returnList ;
	while(!designatedInsertPositions.isEmpty())
	{
		QList<specModelItem*> toBeInserted ;
		toBeInserted << newItems.takeFirst() ;
		QPair<QPersistentModelIndex,int> parentAndPos = designatedInsertPositions.takeFirst() ;
		int row=parentAndPos.second ;
		for (int i = 0 ; i < newItems.size() ; i++)
		{
			if (designatedInsertPositions[i].first == parentAndPos.first &&
						 (designatedInsertPositions[i].second == row-1 ||
						 designatedInsertPositions[i].second == row + toBeInserted.size()))
			{
				toBeInserted << newItems.takeAt(i) ;
				if (designatedInsertPositions.takeAt(i--).second == row-1)
					row-- ;
			}
		}
		if (insertItems(toBeInserted,parentAndPos.first,row))
			for(int i = 0 ; i < toBeInserted.size() ; i++)
				returnList << index(row+i,0,parentAndPos.first) ;
	}
	return returnList ;
}

bool specModel::buildTree(const QModelIndex& parent) // outsource to folderItem (pass list of descriptors)
{
	int col = parent.column() ;
	if(col >= Descriptors.size()) return false ;
	if(itemPointer(parent)->isFolder()) return false ;
// 	itemPointer(parent)->setAutoReplot(false) ;
	((specFolderItem*) itemPointer(parent))->buildTree(Descriptors) ;
// 	itemPointer(parent)->setAutoReplot(true) ;
	return true ;
}

bool specModel::isFolder(const QModelIndex& index) const
{ return itemPointer(index)->isFolder() ;}

void specModel::importFile(QModelIndex index)
{
	index = isFolder(index) ? index : parent(index) ;
	QStringList fileNames = QFileDialog::getOpenFileNames();
	QTime timer ;
	timer.start() ;
	if (!fileNames.isEmpty())
	{
		foreach(QString fileName, fileNames)
		{
			QList<specModelItem*> (*importFunction)(QFile&) = fileFilter(fileName);
			QFile fileToImport(fileName) ;
			fileToImport.open(QFile::ReadOnly | QFile::Text) ;
			QList<specModelItem*> importedItems = importFunction(fileToImport) ;
			insertItems(importedItems,index) ;
		}
	}
}

void specModel::writeToStream(QDataStream &out) const
{
	out << Descriptors ;
	foreach(spec::descriptorFlags flag, DescriptorProperties)
		out << quint8(flag) ;
	out << *root ;
}

void specModel::readFromStream(QDataStream &in)
{
	Descriptors.clear();
	DescriptorProperties.clear();
	delete root ;
	root = new specFolderItem ;
	in >> Descriptors ;
	quint8 flag ;
	for (int i = 0 ; i < Descriptors.size() ; ++i)
	{
		in >> flag ;
		DescriptorProperties << QFlag(flag) ;
	}
	in >> *root ;
}

specModelItem* specModel::itemPointer(const QModelIndex& index) const
{ return (index.isValid() && index.internalPointer() != 0 ? (specModelItem*) index.internalPointer() : root) ; } // TODO evtl. als operator= definieren.

specModel::specModel(QObject *par)
	: QAbstractItemModel(par),
	  internalDrop(false),
	  dropSource(0),
	  dropBuddy(0),
	  dontDelete(0),
	  metaModel(0)
{
	root = new specFolderItem ;
	Descriptors += "" ;
	DescriptorProperties += spec::editable ;
}

specModel::~specModel()
{
	delete root ;
	Descriptors.clear() ;
	DescriptorProperties.clear() ;
}

bool specModel::setHeaderData (int section,Qt::Orientation orientation,const QVariant & value,int role)
{
	Q_UNUSED(orientation)
	if (role == Qt::EditRole)
	{
		Descriptors.replace(section,value.toString()) ;
		emit headerDataChanged(Qt::Horizontal,section,section) ;
		emit headerDataChanged(Qt::Vertical  ,section,section) ;
		return true ;
	}
	
	if (role == 34) // TODO introduce in names.h
	{
		DescriptorProperties[section] = (spec::descriptorFlags) value.toInt() ;
		return true ;
	}
	return false;
}

bool specModel::insertColumns (int column,int count,const QModelIndex & parent)
{
	emit beginInsertColumns(parent,column,column+count-1) ;
	for (QStringList::size_type i = 0 ; i < count ; i++)
	{
		Descriptors.insert(column+i,"") ;
		DescriptorProperties.insert(column+i,spec::def) ;
	}
	emit endInsertColumns() ;
	return true ;
}
bool specModel::removeColumns (int column,int count,const QModelIndex & parent)
{
	emit beginRemoveColumns(parent,column,column+count-1) ;
	for (QStringList::size_type i = 0 ; i < count ; i++)
	{
		Descriptors.removeAt(column) ;
		DescriptorProperties.removeAt(column) ;
	}
	emit endRemoveColumns() ;
	return true ;
}

void specModel::checkForNewDescriptors(const QList<specModelItem*>& list, const QModelIndex& parent)
{
	// TODO: check if descriptors were removed... hm...
	// Check for possible new column headers
	foreach(specModelItem* pointer, list)
	{
		foreach (const QString& descriptor, pointer->descriptorKeys())
		{
			if (!Descriptors.contains(descriptor))
			{
				insertColumns(columnCount(parent),1,parent) ;
				setHeaderData(columnCount(parent)-1,Qt::Horizontal,descriptor) ;
				setHeaderData(columnCount(parent)-1,Qt::Horizontal,(int) pointer->descriptorProperties(descriptor), 34) ;
			}
		}
	}
}

bool specModel::insertItems(QList<specModelItem*> list, QModelIndex parent, int row) // TODO
{
	checkForNewDescriptors(list,parent) ;
	row = row < rowCount(parent) ? row : rowCount(parent) ;
	
	emit layoutAboutToBeChanged() ;
	beginInsertRows(parent, row, row+list.size()-1);
	if (itemPointer(parent)->isFolder())
		((specFolderItem*) itemPointer(parent))->haltRefreshes(true) ;
	bool retVal = itemPointer(parent)->addChildren(list,row) ;
	endInsertRows();
	emit layoutChanged() ;
	
	if (itemPointer(parent)->isFolder())
		((specFolderItem*) itemPointer(parent))->haltRefreshes(false) ;
	return retVal ;
}

int specModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return Descriptors.size() ;
}

int specModel::rowCount(const QModelIndex& parent) const
{ return itemPointer(parent)->children() ; }

QModelIndex specModel::parent(const QModelIndex & index) const
{
	specFolderItem* pointer = itemPointer(index)->parent() ;
	if (pointer && pointer != root)
		return createIndex(pointer->parent()->childNo(pointer),0,pointer);
	return QModelIndex() ;
}

QModelIndex specModel::index(int row, int column, const QModelIndex & parent) const
{
	specModelItem* pointer = itemPointer(parent)->isFolder() ?
			((specFolderItem*) itemPointer(parent))->child(row) : 0 ;
	return pointer ? createIndex(row,column,pointer) : QModelIndex() ;
}

Qt::ItemFlags specModel::flags(const QModelIndex &index) const
{
	if(!index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled ;
	
	Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) | ( itemPointer(index)->isFolder() ? Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled : Qt::ItemIsDragEnabled);
	if (itemPointer(index)->isEditable(Descriptors[index.column()]))
		return  defaultFlags | Qt::ItemIsEditable ;
	return defaultFlags ;
}



QVariant specModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= rowCount(parent(index)))
		return QVariant();
	
	specModelItem* pointer = itemPointer(index) ;
	
	switch(role)
	{
		case Qt::DecorationRole :
//			return index.column() ? QVariant() : pointer->decoration() ;
			return pointer->indicator(Descriptors[index.column()]) ;
// 			return index.column() != 0 ? QVariant() : QIcon(pointer->isFolder() ? ":/folder.png" : (pointer->isSysEntry() ? ":/sys_message.png" : (pointer->isLogEntry() ? ":/log_message.png" : ":/data.png"))) ;
		case Qt::DisplayRole :
			if (index.column() < columnCount(index))
				return pointer->descriptor(Descriptors[index.column()]) ;
			return "" ;
		case Qt::ForegroundRole :
			return pointer->brush() ;
		case spec::fullContentRole :
			return pointer->descriptor(Descriptors[index.column()],true) ;
		case Qt::ToolTipRole :
			return pointer->descriptor(Descriptors[index.column()],true) ;
// 		case 32 : // TODO replace in namespace
// 			return pointer->plotData() ;
	}
	return QVariant() ;
}

QList<specMimeConverter*> specModel::mimeConverters() const
{
	return findChildren<specMimeConverter*>() ;
}

bool specModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	bool changed = false ;
	if(index.isValid())
	{
		QString desc = Descriptors[index.column()] ;
		specModelItem *pointer = itemPointer(index) ;
		if (role == Qt::EditRole)
		{
			if (! (pointer->descriptorProperties(desc) & spec::editable) ) return false ;
			changed = true ;
			specEditDescriptorCommand *command = new specEditDescriptorCommand ;
			command->setParentObject(this) ;
			if (value.canConvert(QVariant::List))
			{
				QList<QVariant> list = value.toList() ;
				if (list.isEmpty())
				{
					delete command ;
					return false ;
				}
				command->setItem(index, desc, list[0].toString(), list[1].toInt()) ;
			}
			else
				command->setItem(index, desc, value.toString()) ;
			// TODO unite in value-variant
			command->setText(tr("Edit data")) ;
			dropBuddy->push(command) ;
		}
		else if (role == Qt::ForegroundRole)
		{
			itemPointer(index)->setPen(value.value<QPen>()) ;
			changed = true ;
		}
		else if (role == 33)
		{
			changed = true ;
			itemPointer(index)->mergePlotData = value.toBool() ;
		}
		else if (role == spec::activeLineRole)
		{
			changed = itemPointer(index)->setActiveLine(Descriptors[index.column()], value.toInt()) ;
		}
	}
	if (changed) emit dataChanged(index,index) ;
	return changed ;
}

QVariant specModel::headerData(int section, Qt::Orientation orientation,
		    int role) const
{
	Q_UNUSED(orientation)
	if (role != Qt::DisplayRole)
		return QVariant();
	return Descriptors[section] ;
}

// bool specModel::insertRows(int position, int rows, const QModelIndex &parent) // TODO remove this function entirely as it is not known beforehand, what type of item is to be added and parent class is purely virtual.
// {
// 	emit layoutAboutToBeChanged() ;
// 	beginInsertRows(parent, position, position+rows-1);
// 	bool retval = itemPointer(parent)->addChildren(position,rows) ;
// 	endInsertRows();
// 	emit layoutChanged() ; // TODO maybe look for a different signal for refresh after adding children
// 	return retval ;
// }

bool specModel::removeRows(int position, int rows, const QModelIndex &parent) 
{ // TODO check if index is valid?
// 	if(!(position+rows <= rowCount(index))) return false ; maybe necessary...
	if (position < 0 || rows < 1) return false ;
	if (dontDelete)
	{
		dontDelete -- ;
		return true ;
	}
	beginRemoveRows(parent, position, position+rows-1); // TODO this is actually not to be permitted!!
// 	QList<specModelItem*> list = itemPointer(index)->takeChildren(position,rows) ;
	QList<specModelItem*> list ;
	specFolderItem *parentPointer = (specFolderItem*) itemPointer(parent) ;
	for (int i = 0 ; i < rows ; i++)
		list << itemPointer(index(position+i,0,parent)) ;
	parentPointer->haltRefreshes(true) ;
	while(!list.isEmpty())
		delete list.takeLast() ;
	parentPointer->haltRefreshes(false) ;
	endRemoveRows();
	return true ;
}

void specModel::setInternalDrop(bool val)
{
	internalDrop = val ;
}

Qt::DropActions specModel::supportedDropActions() const
{ return Qt::CopyAction | Qt::MoveAction; }

void specModel::eliminateChildren(QModelIndexList& list) const
{
	for( QModelIndexList::size_type j = 0 ; j < list.size() ; j++)
		if(list[j].column())
			list.removeAt(j--) ;
	for( QModelIndexList::size_type j = 0 ; j < list.size() ; j++)
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

QMimeData *specModel::mimeData(const QModelIndexList &indices) const
{
	QMimeData *mimeData = new QMimeData() ;
	QModelIndexList list = indices ;
	eliminateChildren(list) ;
	if (dropBuddy)
		dropBuddy->setLastRequested(list) ;

	QList<specModelItem*> pointers = pointerList(list) ;
	foreach (specMimeConverter* converter, mimeConverters())
		converter->exportData(pointers,mimeData) ;

	return mimeData ;
}

bool specModel::mimeAcceptable(const QMimeData *data) const
{
	foreach(specMimeConverter* converter, mimeConverters())
		if (converter->canImport(data))
			return true ;
	return false ;
}

bool specModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column)
	if (action == Qt::IgnoreAction)
		return true;

	row = (row != -1 ? row : rowCount(parent) );
	if (internalDrop && dropBuddy && dropSource)
	{
		dontDelete = dropBuddy->moveInternally(parent,row,dropSource) ;
		internalDrop = false ;
		dropSource = 0 ;
		return true ;
	}


	QList<specModelItem*> newItems ;
	foreach(specMimeConverter* converter, mimeConverters())
	{
		if (converter->canImport(data))
		{
			newItems = converter->importData(data) ;
			break ;
		}
	}

	if (!newItems.isEmpty())
	{
		insertItems(newItems,parent,row) ;
		QModelIndexList newIndexes = indexList(newItems) ;
		if (dropBuddy)
		{
			specAddFolderCommand *command = new specAddFolderCommand ;
			command->setParentObject(this) ;
			command->setItems(newIndexes) ;
			command->setText(tr("Insert items")) ;
			dropBuddy->push(command);
		}
	}
	dropSource = 0 ;
	return true ;
}

void specModel::insertFromStream(QDataStream& stream, const QModelIndex& parent, int row)
{
	QList<specModelItem*> list ;
	while(!stream.atEnd())
	{
		type t ;
		stream >> t ;
		specModelItem* pointer = specModelItem::itemFactory(t);
		pointer->readDirectly(stream);
		list << pointer ;
	}
	insertItems(list,parent,row) ;
}

void specModel::fillSubMap(const QModelIndexList & indexList)
{
	subMap.clear();
	QMap<double,int> norm ;
	foreach(QModelIndex index, indexList)
	{
		specModelItem *pointer = itemPointer(index) ;
		for (size_t i = 0 ; i < pointer->dataSize() ; i++)
		{
			double xval = pointer->sample(i).x() ;
			subMap[xval] += pointer->sample(i).y() ; // Qt promises "default-construction" of double as zero.
			norm[xval] ++ ;
		}
	}
	for (int i = 0 ; i < norm.keys().size() ; i++) // TODO iterators
		subMap[norm.keys()[i]] /= (double) norm[norm.keys()[i]] ;
}

void specModel::applySubMap(const QModelIndexList & indexList)
{
	foreach(QModelIndex index, indexList)
		itemPointer(index)->subMap(subMap) ;
}

specModelItem* specModel::itemPointer(const QVector<int> &indexes) const
{
	specModelItem* pointer = root ;
	for (int i =  indexes.size() - 1 ; i >= 0 && pointer->isFolder() ; --i)
	{
		if (!pointer) return 0 ;
		pointer = ((specFolderItem*) pointer)->child(indexes[i]) ;
	}
	return pointer ;
}

QVector<int> specModel::hierarchy(specModelItem *item)
{
	QVector<int> retVal ;
	specFolderItem *parent ;
	while ((parent = item->parent()))
	{
		retVal << parent->childNo(item) ;
		item = parent ;
	}
	return retVal ;
}

QVector<int> specModel::hierarchy(const QModelIndex &index)
{
	QVector<int> retVal ;
	QModelIndex item = index ;
	while (item.isValid())
	{
		retVal << item.row();
		item = item.parent() ;
	}
	return retVal ;
}

void specModel::setDropBuddy(specActionLibrary *buddy)
{
	dropBuddy = buddy ;
}

QModelIndex specModel::index(const QVector<int> &ancestry,int column) const
{
	QModelIndex returnIndex = QModelIndex() ;
	for (int i = ancestry.size()-1 ; i >= 0 ; --i) // TODO consider precaution if invalid at any stage
		returnIndex = index(ancestry[i],column,returnIndex) ;
	return returnIndex ;
}

bool specModel::contains(specModelItem* parent) const
{
	while (parent->parent()) parent = parent->parent() ;
	return parent == root ;
}

QModelIndex specModel::index(specModelItem *pointer, int column) const
{
	if (!pointer) return QModelIndex() ;
	// Test if item is indeed part of THIS model
	if (!contains(pointer)) return QModelIndex() ;

	// Generate genealogy to find parent
	return index(hierarchy(pointer), column) ;
}

QList<specModelItem*> specModel::pointerList(const QModelIndexList &indexes) const
{
	QList<specModelItem*> returnList ;
	for(int i = 0 ; i < indexes.size() ; ++i)
		returnList << itemPointer(indexes[i]) ;
	return returnList ;
}

QModelIndexList specModel::indexList(const QList<specModelItem *>& pointers) const
{
	QModelIndexList returnList ;
	for (int i = 0 ; i < pointers.size() ; ++i)
		returnList << index(pointers[i]) ;
	return returnList ;
}

void specModel::setDropSource(specView *view)
{
	dropSource = view ;
}

void specModel::svgMoved(specCanvasItem *i, int point, double x, double y)
{
	specSVGItem *item = dynamic_cast<specSVGItem*>(i) ;
	if (!item || !dropBuddy) return ;

	specResizeSVGcommand *command = new specResizeSVGcommand ;
	command->setParentObject(this) ;
	item->pointMoved(point,x,y) ;
	command->setItem(index(item), item->getBounds()) ;
	command->undo();
	command->setText(tr("Resize/move SVG item"));
	dropBuddy->push(command) ;
}

void specModel::setMetaModel(specMetaModel *m)
{
	metaModel = m ;
}

specMetaModel* specModel::getMetaModel() const
{
	return metaModel ;
}

void specModel::signalChanged(const QModelIndex &i)
{

	QModelIndex begin = index(i.row(),0,i.parent()),
			end = index(i.row(),columnCount(i)-1,i.parent()) ;
	specModelItem *item = itemPointer(i) ;
	checkForNewDescriptors(QList<specModelItem*>() << item, i.parent());
	emit dataChanged(begin,end) ;
}
