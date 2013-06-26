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
	specItemAction(parent)
{
	setIcon(QIcon::fromTheme("insert-image")) ;
	setToolTip(tr("Add SVG image")) ;
	setWhatsThis(tr("Insert an image to be displayed as an annotation to the plot canvas.  Currently, SVG images are supported.")) ;
	setText(tr("Add SVG picture item...")) ;
	setShortcut(tr("i"));
}

specUndoCommand* specAddSVGItemAction::generateUndoCommand()
{
	QFile input(QFileDialog::getOpenFileName(parentWidget(),
						 "Select SVG file",
						 "",
						 "SVG images (*.svg)")) ;
	if (!(input.exists() && input.open(QIODevice::ReadOnly)))
		return 0;
	QByteArray fileContent(input.readAll()) ;
	specSVGItem *newItem = new specSVGItem() ;
	newItem->setImage(fileContent) ; // TODO remove

	int row = 0 ;
	if (!currentItem->isFolder())
	{
		row = currentIndex.row()+1 ;
		currentIndex = currentIndex.parent() ;

	}
	if (! model->insertItems(QList<specModelItem*>() << newItem, currentIndex, row))
	{
		delete newItem ;
		return 0;
	}

	specAddFolderCommand *command = new specAddFolderCommand ;
	command->setItems(QModelIndexList() << model->index(row,0,currentIndex)) ;

	command->setParentObject(model) ;
	command->setText(tr("Add SVG item")) ;

	return command ;
}
