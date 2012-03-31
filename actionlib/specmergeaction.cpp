#include "specmergeaction.h"
#include "specdeletecommand.h"
#include "specaddfoldercommand.h"
#include "specmulticommand.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include "names.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QScrollArea>

specMergeAction::specMergeAction(QObject *parent)
	: specUndoAction(parent)
{
}

void specMergeAction::execute()
{
	specView *view = (specView*) parentWidget() ;
	specModel *model = view->model() ;

	// let user define similarities
	QList<QPair<QStringList::size_type, double> > criteria ;
	if (!getMergeCriteria(criteria, model->descriptors(), model->descriptorProperties())) return ;

	QVector<specModelItem*> toBeDeleted ;
	QVector<specModelItem*> newlyInserted ;
	QModelIndexList indexes = view->getSelection() ;
//	for (QModelIndexList::size_type i = 0 ; i < indexes.size() ; ++i)
//		if(model->isFolder(indexes[i]))
//			indexes << allChildren(indexes.takeAt(i--)) ;
	qSort(indexes) ;
	QList<specModelItem*> items = model->pointerList(indexes) ;

	// Create and insert new merged items
	while (!items.isEmpty())
	{
		specDataItem *item = dynamic_cast<specDataItem*>(items.takeFirst()) ;
		if (! item)
			continue ;
		specDataItem *newItem = new specDataItem(*item) ;
		bool others ;
		// look for items to merge with
		for (int i = 0 ; i < items.size() ; ++i)
		{
			if (itemsAreEqual(newItem,items[i],criteria, model->descriptors(), model->descriptorProperties()))
			{
				specDataItem *other = dynamic_cast<specDataItem*>(items[i]) ;
				if (!other)
				{
					items.takeAt(i--) ;
					continue ;
				}
				others = true ;
				*newItem += *(other) ;
				toBeDeleted << items.takeAt(i--) ;
			}
		}
		if (others)
		{
			newItem->flatten();
			if (! model->insertItems(QList<specModelItem*>() << newItem, model->index(item).parent(), model->index(item).row()))
			{
				delete newItem ;
				continue ;
			}
			toBeDeleted << item ; // TODO check possibility of immediate deletion
			newlyInserted << newItem ;
		}
		else
			delete newItem ;
	}

	specMultiCommand *command = new specMultiCommand ;
	command->setParentWidget(view) ;
	command->setMergeable(false) ;

	// preparing insertion command
	specAddFolderCommand *insertionCommand = new specAddFolderCommand(command) ;
	QModelIndexList insertList = model->indexList(newlyInserted.toList()) ;
	qDebug() << "******* insert" << insertList ;
	insertionCommand->setItems(insertList) ;
	insertionCommand->setParentWidget(view) ;

	// prepare to delete the old item
	specDeleteCommand *deletionCommand = new specDeleteCommand(command) ;
	QModelIndexList deleteList = model->indexList(toBeDeleted.toList()) ;
	qDebug() << "******* delete" << deleteList ;
	deletionCommand->setItems(deleteList) ;
	deletionCommand->setParentWidget(view) ;

	library->push(command) ;
}


bool specMergeAction::getMergeCriteria(QList<QPair<QStringList::size_type, double> >& toCompare, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties)
{ // TODO separate class
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


bool specMergeAction::itemsAreEqual(specModelItem* first, specModelItem* second, const QList<QPair<QStringList::size_type, double> >& criteria, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties) // TODO insert actual strings into merge criteria
{
	if (!first || !second)
		return false ;
	// TODO may need to be revised if descriptor contains actual numeric value, not QString
	bool equal = true ;
	for(QList<QPair<QStringList::size_type, double> >::size_type i = 0 ; i < criteria.size() ; i++)
	{
		qDebug() << "comparing" << i << descriptors[criteria[i].first] << first->descriptor(descriptors[criteria[i].first]) << second->descriptor(descriptors[criteria[i].first]) ;
		QStringList::size_type descriptor = criteria[i].first ;
		double tolerance = criteria[i].second ;
		if (descriptorProperties[descriptor] & spec::numeric)
		{
			double a = first->descriptor(descriptors[descriptor]).toDouble(),
				b = second->descriptor(descriptors[descriptor]).toDouble() ;
			equal &= b-tolerance <= a && a <= b+tolerance ;
		}
		else
			equal &= first->descriptor(descriptors[descriptor]) ==
				second->descriptor(descriptors[descriptor]) ;
	}
	return equal ;
}

//QModelIndexList specMergeAction::allChildren(const QModelIndex& parent) const // TODO ueberarbeiten
//{
//	if (!parent.isValid() || !isFolder(parent))
//		return QModelIndexList() ;
//	QModelIndexList list ;
//	for (int i = 0 ; i < rowCount(parent) ; i++)
//		list << this->index(i,0,parent) ;
//	return list ;
//}
