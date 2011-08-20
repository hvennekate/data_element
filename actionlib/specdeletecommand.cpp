#include "specdeletecommand.h"

specDeleteCommand::specDeleteCommand(specUndoCommand *parent) :
	specUndoCommand(parent),
	sorted(false)
{
}

void specDeleteCommand::addItem(specModelItem *item)
{
	specFolderItem *parent = item->parent() ;
	data[item->parent()] << QPair<specModelItem*,int>(item,parent->childNo(item)) ;
	sorted = false ;
}

bool specDeleteCommand::indexComp(const indexPair &a, const indexPair &b)
{
	return a.second < b.second ;
}

void specDeleteCommand::doIt(bool undo)
{
	qDebug("--- signaling reset") ;
	model->signalBeginReset();

	// turn plot refresh of parents off
	qDebug("--- halting refreshes") ;
	QList<specFolderItem*> parents = data.keys() ;
	for (int i = 0 ; i < parents.size() ; ++i)
		parents[i]->haltRefreshes(true) ;
	// take children
	qDebug("--- removing items") ;
	for (dataHash::iterator i = data.begin() ; i != data.end() ; ++i)
	{
		if (!undo)
			for (int j = 0 ; j < i.value().size() ; j++)
				(i.value().at(j).first)->setParent(0) ;
		else
		{
			specFolderItem* newParent = i.key() ;
			for (int j = 0 ; j < i.value().size() ; j++)
				newParent->addChild(i.value().at(j).first, i.value().at(j).second) ;
		}
	}
	qDebug("--- restoring refreshes") ;
	// restore refreshes
	for (int i = 0 ; i < parents.size() ; ++i)
		parents[i]->haltRefreshes(false) ;
	qDebug("--- signaling reset done") ;
	model->signalEndReset();
	qDebug("--- done") ;

}

void specDeleteCommand::undo()
{
	doIt(true) ;
}

void specDeleteCommand::redo()
{
	qDebug("called redo") ;
	doIt(false) ;
}


bool specDeleteCommand::ok()
{
	if (model && !data.isEmpty())
		return true ;

	return false ;
}

void specDeleteCommand::setModel(specModel *mod)
{
	model = mod ;
}
