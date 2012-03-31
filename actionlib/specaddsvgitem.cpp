#include "specaddsvgitem.h"
#include "specdataview.h"
#include "specgenealogy.h"
#include "specaddfoldercommand.h"
#include "specsvgitem.h"
#include <QFile>
#include <QFileDialog>
#include <QByteArray>
#include <QtSvg>

specAddSVGItemAction::specAddSVGItemAction(QObject *parent) :
    specUndoAction(parent)
{
}

const std::type_info &specAddSVGItemAction::possibleParent()
{
	return typeid(specView) ;
}

void specAddSVGItemAction::execute()
{
	QFile input(QFileDialog::getOpenFileName(parentWidget(),"Open SVG file")) ;
	if (!(input.exists() && input.open(QIODevice::ReadOnly)))
		return ;
	QByteArray fileContent(input.readAll()) ;
	specSVGItem *newItem = new specSVGItem() ;
	newItem->setImage(QRectF(1700,1,100,3),fileContent) ;

	specDataView *currentView = (specDataView*) parent() ; // TODO consolidate with addfolderaction
	specModel *model = currentView->model() ;
	QModelIndex index = currentView->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	qDebug("checking if item is folder") ;
	if (!item->isFolder())
	{
		qDebug("item is not a folder") ;
		row = index.row()+1 ;
		index = index.parent() ;

	}
	qDebug("inserting item") ;
	if (! model->insertItems(QList<specModelItem*>() << newItem, index, row))
	{
		delete newItem ;
		return ;
	}

	qDebug("done inserting") ;
	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(row,0,index)) ;
	qDebug("set item to command") ;

	command->setParentWidget((QWidget*)parent()) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}