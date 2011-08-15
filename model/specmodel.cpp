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
		if (descriptorProperties[descriptor] & spec::numeric)
		{
			double a = itemPointer(first)->descriptor(descriptors[descriptor]).toDouble(),
				b = itemPointer(second)->descriptor(descriptors[descriptor]).toDouble() ;
			equal &= b-tolerance <= a && a <= b+tolerance ;
		}
		else
			equal &= itemPointer(first)->descriptor(descriptors[descriptor]) ==
				itemPointer(second)->descriptor(descriptors[descriptor]) ;
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
	for(QStringList::size_type i = 0 ; i < descriptors.size() ; i++)
	{
		descriptorLayout->addWidget(new QCheckBox(descriptors[i]),i,0) ;
// 		((QCheckBox*) descriptorLayout->itemAt(descriptorLayout->count()-1)->widget())->setCheckState(Qt::Checked) ;
		if(descriptorProperties[i] & spec::numeric)
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
				descriptorProperties[row] & spec::numeric ?
						((QLineEdit*) descriptorLayout->itemAt(i+1)->widget())->text().toDouble() : 0.) ;
	}
	
	return retval ;
}

bool specModel::exportData(QModelIndexList& list)
{
	QFile *exportFile = new QFile(QFileDialog::getSaveFileName(0,"File name","","ASCII files (*.asc)")) ;
	exportDialog *exportFormat = new exportDialog(&descriptors) ;
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

QModelIndexList specModel::merge(QModelIndexList& list, const QList<QPair<QStringList::size_type, double> >& criteria)
{
	bool fitInterpolated = true ; // TODO: Get from criteria
// resolve folders and sort  TODO exclude system messages etc.
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
		qDebug("initial descriptor is editable: %d",int(itemPointer(mergeList.first())->descriptorProperties("") & spec::editable)) ;
		QByteArray *firstItemData = new QByteArray() ;
		QDataStream outStream(firstItemData,QIODevice::WriteOnly) ;
		itemPointer(mergeList.takeFirst())->writeOut(outStream) ;
		QDataStream inStream(firstItemData,QIODevice::ReadOnly) ;
		specModelItem *newItem ;
		inStream >> newItem ;
		newItems << newItem ;
		qDebug("initial descriptor of copy is editable: %d",int(newItem->descriptorProperties("") & spec::editable)) ;
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
			qDebug("initial descriptor of copy before merging is editable: %d",int(pointer->descriptorProperties("") & spec::editable)) ;
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
	qDebug("done merging") ;
	return returnList ;
}

bool specModel::buildTree(const QModelIndex& parent) // outsource to folderItem (pass list of descriptors)
{
	int col = parent.column() ;
	if(col >= descriptors.size()) return false ;
	if(itemPointer(parent)->isFolder()) return false ;
// 	itemPointer(parent)->setAutoReplot(false) ;
	((specFolderItem*) itemPointer(parent))->buildTree(descriptors) ;
// 	itemPointer(parent)->setAutoReplot(true) ;
	return true ;
}

bool specModel::isFolder(const QModelIndex& index) const
{ return itemPointer(index)->isFolder() ;}

void specModel::importFile(QModelIndex index)
{
	QTime timer ;
	index = isFolder(index) ? index : parent(index) ;
	QStringList fileNames = QFileDialog::getOpenFileNames();
	timer.start() ;
	if (fileNames.size())
	{
		foreach(QString fileName, fileNames)
		{
			QList<specModelItem*> (*importFunction)(QFile&) = fileFilter(fileName);
			QFile fileToImport(fileName) ;
			fileToImport.open(QFile::ReadOnly | QFile::Text) ;
			QList<specModelItem*> importedItems = importFunction(fileToImport) ;
			qDebug("%d msecs to import",timer.restart()) ;
			insertItems(importedItems,index) ;
			qDebug("%d msecs to get header info",timer.elapsed()) ;
		}
	}
}

QDataStream& operator<<(QDataStream& out, specModel& model)
{
	qDebug("---writing model") ;
	out << (qint32) model.descriptors.size() ;
	for( int i =0 ; i < model.descriptors.size() ; i++)
		out << model.descriptors[i] << (quint8) model.descriptorProperties[i] ;
	return model.root->writeOut(out) ;
} // TODO save column widths --> on plotwidget level!

QDataStream& operator>>(QDataStream& in, specModel& model)
{
	qDebug("Reading model") ;
	delete model.root ;
	model.descriptors.clear() ;
	model.descriptorProperties.clear() ;
// 	cout << "---reading  model" << endl ;
	quint32 descriptorsSize;
	quint8  prop ;
	in >> descriptorsSize ;
// 	cout << "---descriptors: " << descriptorsSize << endl ;
	for (int i = 0 ; i < descriptorsSize ; i++)
	{
		model.descriptors.append(QString()) ;
		in >> model.descriptors.last() >> prop ;
		model.descriptorProperties.append((spec::descriptorFlags) prop) ;
// 		cout << "---read descriptor " << i << ":  " << model.descriptors.last() << "  Properties: " << model.descriptorProperties.last() << endl ;
	}
	qDebug("reading root item") ;
	return in >> model.root ;
}

specModelItem* specModel::itemPointer(const QModelIndex& index) const
{ return (index.isValid() && index.internalPointer() != 0 ? (specModelItem*) index.internalPointer() : root) ; } // TODO evtl. als operator= definieren.

specModel::specModel(QObject *par)
	: QAbstractItemModel(par), mime("application/spec.model.item")
{
	root = new specFolderItem ;
	descriptors += "" ;
	descriptorProperties += spec::editable ;
}

specModel::~specModel()
{
	delete root ;
	descriptors.clear() ;
	descriptorProperties.clear() ;
}

bool specModel::setHeaderData (int section,Qt::Orientation orientation,const QVariant & value,int role)
{
	if (role == Qt::EditRole)
	{
		descriptors.replace(section,value.toString()) ;
		emit headerDataChanged(Qt::Horizontal,section,section) ;
		emit headerDataChanged(Qt::Vertical  ,section,section) ;
		return true ;
	}
	
	if (role == 34) // TODO introduce in names.h
	{
		descriptorProperties[section] = (spec::descriptorFlags) value.toInt() ;
		return true ;
	}
	return false;
}

bool specModel::insertColumns (int column,int count,const QModelIndex & parent)
{
	emit beginInsertColumns(parent,column,column+count-1) ;
	for (QStringList::size_type i = 0 ; i < count ; i++)
	{
		descriptors.insert(column+i,"") ;
		descriptorProperties.insert(column+i,spec::def) ;
	}
	emit endInsertColumns() ;
	return true ;
}
bool specModel::removeColumns (int column,int count,const QModelIndex & parent)
{
	emit beginRemoveColumns(parent,column,column+count-1) ;
	for (QStringList::size_type i = 0 ; i < count ; i++)
	{
		descriptors.removeAt(column) ;
		descriptorProperties.removeAt(column) ;
	}
	emit endRemoveColumns() ;
	return true ;
}

bool specModel::insertItems(QList<specModelItem*> list, QModelIndex parent, int row) // TODO
{
	// Check for possible new column headers
	QTime timer ;
	timer.start();
	qDebug("checking new col headers") ;
	for (QList<specModelItem*>::size_type i = 0 ; i < list.size() ; i++)
	{
		for (QStringList::size_type j = 0 ; j < list[i]->descriptorKeys().size() ; j++)
		{
			if (!descriptors.contains(list[i]->descriptorKeys()[j]))
			{
				insertColumns(columnCount(parent),1,parent) ;
				setHeaderData(columnCount(parent)-1,Qt::Horizontal,list[i]->descriptorKeys()[j]) ;
				setHeaderData(columnCount(parent)-1,Qt::Horizontal,(int) list[i]->descriptorProperties(list[i]->descriptorKeys()[j]), 34) ;
			}
		}
	}
	row = row < rowCount(parent) ? row : rowCount(parent) ;
	
	qDebug("inserting %d",timer.restart()) ;
	emit layoutAboutToBeChanged() ;
	beginInsertRows(parent, row, row+list.size()-1);
	qDebug("assigning to folder %d", timer.restart()) ;
	if (itemPointer(parent)->isFolder())
		((specFolderItem*) itemPointer(parent))->haltRefreshes(true) ;
	bool retVal = itemPointer(parent)->addChildren(list,row) ;
	qDebug("done %d",timer.restart()) ;
	endInsertRows();
	emit layoutChanged() ;
	
	if (itemPointer(parent)->isFolder())
		((specFolderItem*) itemPointer(parent))->haltRefreshes(false) ;
	return retVal ;
}

int specModel::columnCount(const QModelIndex &parent) const {return descriptors.size() ;}
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
	if (itemPointer(index)->isEditable(descriptors[index.column()]))
		return  defaultFlags | Qt::ItemIsEditable ;
	return QAbstractItemModel::flags(index) ;
}



QVariant specModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= rowCount(parent(index)))
		return QVariant();
	
	specModelItem* pointer = itemPointer(index) ;
	
	switch(role)
	{
		case Qt::DecorationRole :
			return index.column() ? QVariant() : pointer->decoration() ;
// 			return index.column() != 0 ? QVariant() : QIcon(pointer->isFolder() ? ":/folder.png" : (pointer->isSysEntry() ? ":/sys_message.png" : (pointer->isLogEntry() ? ":/log_message.png" : ":/data.png"))) ;
		case Qt::DisplayRole :
			if (index.column() < columnCount(index))
				return pointer->descriptor(descriptors[index.column()]) ;
			return "" ;
		case Qt::ForegroundRole :
			return itemPointer(index)->brush() ;
// 		case 32 : // TODO replace in namespace
// 			return pointer->plotData() ;
	}
	return QVariant() ;
}

bool specModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	bool changed = false ;
	if(index.isValid())
	{
		switch(role)
		{
			case Qt::EditRole :
		 		changed =  itemPointer(index)->changeDescriptor(descriptors[index.column()], value.toString()) ; break ;
			case Qt::ForegroundRole :
				itemPointer(index)->setPen(value.value<QPen>()) ;
				changed = true ;
				break ;
			case 33 :
				changed = true ; itemPointer(index)->mergePlotData = value.toBool() ; break ;
		}
	}
	if (changed) emit dataChanged(index,index) ;
	return changed ;
}

QVariant specModel::headerData(int section, Qt::Orientation orientation,
		    int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	return descriptors[section] ;
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
	beginRemoveRows(parent, position, position+rows-1);
// 	QList<specModelItem*> list = itemPointer(index)->takeChildren(position,rows) ;
	QList<specModelItem*> list ;
	specFolderItem *parentPointer = (specFolderItem*) itemPointer(parent) ;
	qDebug("removing %d rows at %d", rows, position) ;
	for (int i = 0 ; i < rows ; i++)
		list << itemPointer(index(position+i,0,parent)) ;
	parentPointer->haltRefreshes(true) ;
	while(!list.isEmpty())
		delete list.takeLast() ;
	parentPointer->haltRefreshes(false) ;
	endRemoveRows();
	return true ;
}

Qt::DropActions specModel::supportedDropActions() const
{ return Qt::CopyAction | Qt::MoveAction; }

QStringList specModel::mimeTypes() const
{ return mime ; }

void specModel::setMimeTypes(const QStringList& types)
{ mime = types ; }

void specModel::eliminateChildren(QModelIndexList& list) const
{
	qDebug("before removing children: %d",list.size()) ;
	for( QModelIndexList::size_type j = 0 ; j < list.size() ; j++)
		if(list[j].column())
			list.removeAt(j--) ;
	qDebug("after removing other columns: %d", list.size()) ;
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
	QByteArray encodedData ;
	QDataStream stream(&encodedData,QIODevice::WriteOnly) ;
	QModelIndexList list = indices ;
	qDebug("originally in list: %d",list.size()) ;
	eliminateChildren(list) ;
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "mimeData list size:  " << list.size() << endl ;
	
	qDebug("exporting %d items of mime",list.size()) ;
	foreach(QModelIndex index,list) // TODO eliminate children
	{
		qDebug("checking item") ;
		if(index.isValid() && index.column() == 0)
		{
			qDebug("writing item") ;
			itemPointer(index)->writeOut(stream) ;
		}
	}
	
// 	qDebug("setting mime data") ;
	mimeData->setData(mime.first(),encodedData) ;
// 	qDebug("returning mime data") ;
	return mimeData ;
}

bool specModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat(mime.first()))
		return false;
	
	QByteArray encodedData = data->data(mime.first()) ;
	QDataStream stream(&encodedData, QIODevice::ReadOnly) ;
	row = (row != -1 ? row : rowCount(parent) );
	qDebug("dropping mime from stream...") ;
	while(!stream.atEnd())
		insertFromStream(stream,parent,row++) ;
	return true ;
}

void specModel::insertFromStream(QDataStream& stream, const QModelIndex& parent, int row)
{
	QList<specModelItem*> list ;
	qDebug("starting insert from stream") ;
	while(!stream.atEnd())
	{
		qDebug("dropping an item") ;
		specModelItem* pointer ;
		stream >> pointer ;
		list << pointer ;
	}
	insertItems(list,parent,row) ;
}

specModel::specModel(QDataStream& in,QObject *par) :
		QAbstractItemModel(par), mime("application/spec.model.item") // TODO:  read mime type from stream
{
	root = new specFolderItem ;
	descriptors += "" ;
	descriptorProperties += spec::editable ;
	in >> *this ;
	qDebug("done reading model") ;
}

void specModel::fillSubMap(const QModelIndexList & indexList)
{
	subMap.clear();
	QMap<double,int> norm ;
	foreach(QModelIndex index, indexList)
	{
		specModelItem *pointer = itemPointer(index) ;
		for (int i = 0 ; i < pointer->dataSize() ; i++)
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
