#include "spectiltmatrixaction.h"
#include "specmulticommand.h"
#include "specdeleteaction.h"
#include <QSet>
#include "specdataitem.h"
#include <QMap>
#include "specaddfoldercommand.h"
#include "specdeletecommand.h"
#include "specexchangedescriptorxdialog.h"

specTiltMatrixAction::~specTiltMatrixAction()
{
	delete dialog ;
}

specTiltMatrixAction::specTiltMatrixAction(QObject *parent) :
	specRequiresDataItemAction(parent),
	dialog(new specExchangeDescriptorXDialog)
{
	setIcon(QIcon(":/exchangex.png")) ;
	setToolTip(tr("Exchange x data for descriptive field")) ;
	setWhatsThis(tr("Generates new items from the selected items' data, simultaneously turning the x-axis data into a description field and exchanging it for a different description field."));
	setText(tr("Exchange x...")) ;
	setShortcut(tr("Ctrl+j")) ;
}

class descriptorPreparationObject
{
	QSet<QString> values ;
	spec::descriptorFlags flagValue ;
public:
	void addDescriptor(const QString& value, const spec::descriptorFlags& Flags)
	{
		values << value ;
		flagValue &= Flags ; // TODO reconsider
	}
	QString descriptorValue() const
	{
		QStringList valueList(values.toList()) ;
		valueList.sort();
		return valueList.join("\n") ;
	}
	spec::descriptorFlags flags() const
	{ return values.size() > 1 ? (flagValue & ~spec::numeric) : flagValue ; }
	specDescriptor descriptor() const
	{ return specDescriptor(descriptorValue(), flags()) ; }
};

class dataItemPreparationObject
{
	QVector<specDataPoint> dataPoints ;
	QMap<QString, descriptorPreparationObject> description ;
public:
	void addDataPoint(double x, double y) { dataPoints << specDataPoint(x,y,0) ; }

	void addDescriptor(const QString& key, const QString& value, const spec::descriptorFlags& flags)
	{
		description[key].addDescriptor(value, flags) ;
	}

	void addItem(const specModelItem* item, double x, double y)
	{
		addDataPoint(x,y) ;
		foreach(const QString& key, item->descriptorKeys())
			addDescriptor(key, item->descriptor(key, true), item->descriptorProperties(key));
	}

	specDataItem *dataItem() const
	{
		QHash<QString, specDescriptor> descriptionHash ;
		foreach(const QString& key, description.keys())
			descriptionHash[key] = description[key].descriptor() ;
		return new specDataItem(dataPoints, descriptionHash) ;
	}
};



specUndoCommand* specTiltMatrixAction::generateUndoCommand()
{
	QList<specModelItem*> items, toBeDeletedFolders ;
	expandSelectedFolders(items, toBeDeletedFolders);

	// bail out if only empty folders were selected
	if (items.isEmpty()) return 0 ;

	// Get input from user
	dialog->setDescriptors(model->descriptors()) ;
	if(QDialog::Accepted != dialog->exec()) return 0 ;
	QString newDescriptor = dialog->newDescriptor(),
			conversionDescriptor = dialog->descriptorToConvert() ;

	// collect the data
	QMap<double, dataItemPreparationObject> newData ;
	// xValue, data, descriptor, descriptions, flags
	foreach(specModelItem* item, items)
	{
		item->revalidate();
		for (size_t i = 0 ; i < item->dataSize() ; ++i)
			newData[item->sample(i).x()].addItem(item, item->descriptorValue(conversionDescriptor),item->sample(i).y()) ;
	}

	// collect new items
	QList<specModelItem*> newItems ;
	foreach(const double& key, newData.keys())
	{
		// TODO avoid back and forth conversions double <-> string!!
		newData[key].addDescriptor(newDescriptor,
					   QString::number(key),
					   spec::numeric);
		newItems << newData[key].dataItem() ;
	}
	// remove descriptor exchanged for x value
	foreach(specModelItem* item, newItems)
		item->deleteDescriptor(conversionDescriptor) ;

	// insert new items
	// find safe parent index
	specModelItem* parent = currentItem ;
	while (parent->parent())
	{
		if (toBeDeletedFolders.contains(parent))
		{
			insertionRow = parent->parent()->childNo(parent) ;
			insertionIndex = model->index(parent->parent()) ;
		}
		parent = parent->parent() ;
	}


	if (!model->insertItems(newItems, insertionIndex, insertionRow)) return 0 ;

	// command generation
	specMultiCommand* parentCommand = new specMultiCommand ;
	parentCommand->setMergeable(false) ;
	specAddFolderCommand *insertionCommand = new specAddFolderCommand(parentCommand) ;
	QModelIndexList newIndexes = model->indexList(newItems) ;
	insertionCommand->setItems(newIndexes); // TODO integrate into command constructor

	// delete old items
	QModelIndexList newItemIndexes = model->indexList(items) ;
	newItemIndexes << model->indexList(toBeDeletedFolders) ;
	specDeleteAction::command(model, newItemIndexes, parentCommand) ;

	parentCommand->setParentObject(model) ;
	parentCommand->setText(tr("Exchange \"")+
				   conversionDescriptor+
				   tr("\" and \"")+
				   newDescriptor+
				   tr("\" in ")+
				   QString::number(newItems.size())+
				   tr(" items.")) ;
	return parentCommand ;
}
