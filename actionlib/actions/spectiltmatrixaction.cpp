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

specTiltMatrixAction::specTiltMatrixAction(QObject* parent) :
	specRequiresDataItemAction(parent),
	dialog(new specExchangeDescriptorXDialog)
{
	setIcon(QIcon(":/exchangex.png")) ;
	setToolTip(tr("Exchange x data for descriptive field")) ;
	setWhatsThis(tr("Generates new items from the selected items' data, simultaneously turning the x-axis data into a description field and exchanging it for a different description field."));
	setText(tr("Exchange x...")) ;
	setShortcut(tr("Ctrl+j")) ;
}

class dataItemPreparationObject
{
	QVector<specDataPoint> dataPoints ;
	QMap<QString, QSet<specDescriptor> > description ;
public:
	void addDataPoint(double x, double y) { dataPoints << specDataPoint(x, y, 0) ; }

	void addDescriptor(const QString& key, const specDescriptor& desc)
	{
		description[key] << desc ;
	}

	void addItem(const specModelItem* item, double x, double y)
	{
		addDataPoint(x, y) ;
		foreach(const QString & key, item->descriptorKeys())
			addDescriptor(key, item->getDescriptor(key)) ;
	}

	specDataItem* dataItem() const
	{
		QHash<QString, specDescriptor> descriptionHash ;
		foreach(const QString & key, description.keys())
			descriptionHash[key] = specDescriptor::merge(description[key].toList()) ;
		return new specDataItem(dataPoints, descriptionHash) ;
	}
};



specUndoCommand* specTiltMatrixAction::generateUndoCommand()
{
	QList<specModelItem*> items, toBeDeletedFolders ;
	expandSelectedFolders(items, toBeDeletedFolders);

	// bail out if only empty folders were selected
	if(items.isEmpty()) return 0 ;

	// Get input from user
	dialog->setDescriptors(model->descriptors()) ;
	if(QDialog::Accepted != dialog->exec()) return 0 ;
	QString newDescriptor = dialog->newDescriptor(),
		conversionDescriptor = dialog->descriptorToConvert() ;

	// collect the data
	QMap<double, dataItemPreparationObject> newData ;
	// xValue, data, descriptor, descriptions, flags
	foreach(specModelItem * item, items)
	{
		item->revalidate();
		for(size_t i = 0 ; i < item->dataSize() ; ++i)
			newData[item->sample(i).x()].addItem(item, item->descriptorValue(conversionDescriptor), item->sample(i).y()) ;
	}

	// collect new items
	QList<specModelItem*> newItems ;
	foreach(const double & key, newData.keys())
	{
		newData[key].addDescriptor(newDescriptor,key);
		newItems << newData[key].dataItem() ;
	}
	// remove descriptor exchanged for x value
	foreach(specModelItem * item, newItems)
	item->deleteDescriptor(conversionDescriptor) ;

	// insert new items
	// find safe parent index
	specModelItem* parent = currentItem ;
	while(parent->parent())
	{
		if(toBeDeletedFolders.contains(parent))
		{
			insertionRow = parent->parent()->childNo(parent) ;
			insertionIndex = model->index(parent->parent()) ;
		}
		parent = parent->parent() ;
	}


	if(!model->insertItems(newItems, insertionIndex, insertionRow)) return 0 ;

	// command generation
	specMultiCommand* parentCommand = new specMultiCommand ;
	parentCommand->setParentObject(model) ;
	parentCommand->setMergeable(false) ;
	specAddFolderCommand* insertionCommand = new specAddFolderCommand(parentCommand) ;
	int newItemCount = newItems.size() ;
	insertionCommand->setItems(newItems);  // TODO integrate into command constructor

	// delete old items
	items << toBeDeletedFolders ;
	int oldItemCount = items.size() ;
	specDeleteAction::command(model, items, parentCommand) ;

	parentCommand->setText(tr("Exchange \"") +
			       conversionDescriptor +
			       tr("\" and \"") +
			       newDescriptor +
			       tr("\" creating ") +
			       QString::number(newItemCount) +
			       tr(" item(s) from ") +
			       QString::number(oldItemCount) +
			       tr(" item(s).")
			       ) ;
	return parentCommand ;
}
