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
	setIcon(QIcon::fromTheme("insert-image")) ;
	setToolTip(tr("Add SVG image")) ;
	setWhatsThis(tr("Insert an image to be displayed as an annotation to the plot canvas.  Currently, SVG images are supported.")) ;
}

const std::type_info &specAddSVGItemAction::possibleParent()
{
	return typeid(specView) ;
}

void specAddSVGItemAction::execute()
{
	QFile input(QFileDialog::getOpenFileName(parentWidget(),
						 "Select SVG file",
						 "",
						 "SVG images (*.svg)")) ;
	if (!(input.exists() && input.open(QIODevice::ReadOnly)))
		return ;
	QByteArray fileContent(input.readAll()) ;
	specSVGItem *newItem = new specSVGItem() ;
	newItem->setImage(fileContent) ; // TODO remove

	specView *currentView = (specView*) parent() ; // TODO consolidate with addfolderaction
	specModel *model = currentView->model() ;
	QModelIndex index = currentView->currentIndex() ;
	specModelItem *item = model->itemPointer(index) ;
	int row = 0 ;
	if (!item->isFolder())
	{
		row = index.row()+1 ;
		index = index.parent() ;

	}
	if (! model->insertItems(QList<specModelItem*>() << newItem, index, row))
	{
		delete newItem ;
		return ;
	}

	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(row,0,index)) ;

	command->setParentObject(model) ;
	command->setText(tr("Add SVG item")) ;

	if (command->ok())
		library->push(command) ;
	else
		delete command ;
}
