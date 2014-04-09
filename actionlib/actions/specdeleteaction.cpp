#include "specdeleteaction.h"
#include "specdeletecommand.h"
#include "specgenealogy.h"
#include "specmetamodel.h"
#include "specmulticommand.h"
#include "specdeleteconnectionscommand.h"
#include <QMap>
#include "speclogmodel.h"
#include "specmetaitem.h"
#include "specdataview.h"

specDeleteAction::specDeleteAction(QObject* parent) :
	specRequiresItemAction(parent)
{
	this->setIcon(QIcon::fromTheme("edit-delete"));
	setToolTip(tr("Delete")) ;
	setWhatsThis(tr("Deletes selected items.")) ;
	setText(tr("Delete")) ;
	setShortcut(Qt::Key_Delete);
}

specUndoCommand* specDeleteAction::command(specModel* model, QList<specModelItem*>& selection, specUndoCommand* parentsParent)
{
	specDeleteCommand* command = new specDeleteCommand(parentsParent) ;
	command->setParentObject(model);
	command->setItems(selection) ;
	command->setText(tr("Delete ") + QString::number(selection.size()) + tr(" items.")) ;
	return command ;
}

specUndoCommand* specDeleteAction::generateUndoCommand()
{
	return command(model, pointers) ;
}
