#include "specmetaview.h"
#include "specmetadelegate.h"
#include "specmetarange.h"
#include "specmetarangecommand.h"
#include "specmetamodel.h"
#include "specmetaitem.h"
#include "specactionlibrary.h"
#include <QQueue>
#include "speccanvasitem.h"

specMetaView::specMetaView(QWidget *parent) :
	specView(parent),
	dataView(0)
{
	QAbstractItemDelegate *olddel = itemDelegate() ;
	setItemDelegate(new specMetaDelegate) ;
	delete olddel ;
	setWhatsThis(tr("This list contains meta entries which obtain their data from data items.  Use the connect button to connect meta items to data items.  Manage items by dragging and dropping.  Add a new item by using the appropriate button.  Connection to data items may also be established using the appropriate button and changed by right clicking on the respective meta item.\nThe first column is for your comments/descriptions; the \"variables\" column lets you define how to obtain data from data items (see below); the \"x\" and \"y\" columns may contain a formula for calculating the x and y values, respectively, of an item from the variables defined in the \"variables\" column.  Any errors in evaluating these will be displayed in the \"errors\" column.\nIf a field contains multiple lines, an indicator will appear.  The full contents is available as tool tip text (just rest the mous cursor on the field).\n\nSyntax for defining variables:\n name = [from:to:step]\"column\"\nor:\nname = [from:to:step]ABeginning:End\n where \"A\" may be \"x\" (meaning x values), \"y\" (y values),\"i\" (integral), \"u\" (max y value), \"l\" (min y value), \"p\" (position of min y value), \"P\" (position of min y value);\n \"column\" may be any column name; \"from\", \"to\", and \"step\" define the range of connected data items to use amd \"Beginning\" and \"End\" define the x range of values to use.\n\n When using a fit, the field \"Fit variables\" is to contain all variables used in the fitting formula, one line each with starting value, as in \n\ta=1\n\tb=3\nThe field\"Fit parameters\" must conatain a list of comma-separated names of variables which the fit algorithm is allowed to use for fitting.  \"Fit expression\" is what it says."));
	setObjectName("metaView");
}

specMetaView::~specMetaView()
{
}

void specMetaView::setModel(specMetaModel *mod)
{
	specView::setModel((specModel*) mod) ;
}

specMetaModel *specMetaView::model() const
{
	return (specMetaModel*) specView::model() ;
}

void specMetaView::assignDataView(specView *view)
{
	dataView = view ;
	if (model())
		((specMetaModel*) model())->setDataModel(dataView ? dataView->model() : 0) ;
}

specView *specMetaView::getDataView()
{
	return dataView ;
}

void specMetaView::rangeModified(specCanvasItem *r, int p, double x, double y)
{
	specMetaRange::addressObject address = ((specMetaRange*) r)->address() ;
	specMetaRangeCommand* command = new specMetaRangeCommand ;
	command->setItem(model()->index(address.item), address.variable,address.range, p, x, y) ;
	command->setParentObject(model()) ;
	command->setText(tr("Modify range")) ;
	if (!actionLibrary)
	{
		command->redo();
		delete command ;
	}
	else
		actionLibrary->push(command) ;
}

void specMetaView::readFromStream(QDataStream &in)
{
	specView::readFromStream(in) ;

	// Restoring the connections...
	QQueue<specModelItem*> itemQueue ;
	itemQueue << model()->itemPointer(QModelIndex()) ;
	while (!itemQueue.isEmpty())
	{
		specModelItem* item = itemQueue.dequeue();
		if (item->isFolder())
		{
			specFolderItem *folder = (specFolderItem*) item ;
			for (int i = 0 ; i < item->children() ; ++i)
				itemQueue.enqueue(folder->child(i)) ;
		}
		specMetaItem* mitem = dynamic_cast<specMetaItem*>(item) ;
		if (mitem)
			mitem->setModels(model(),dataView->model()) ;
	}
}
