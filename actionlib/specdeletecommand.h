#ifndef SPECDELETECOMMAND_H
#define SPECDELETECOMMAND_H

#include "specmodel.h"
#include "specundocommand.h"
#include <QVector>
#include <QPair>
#include <QHash>

class specDeleteCommand : public specUndoCommand
{
public:
	explicit specDeleteCommand(specUndoCommand *parent = 0);

	void setModel(specModel*) ;
	void addItem(specModelItem* item) ;
	bool ok() ;
	void redo() ;
	void undo() ;
	typedef QPair<specModelItem*,int> indexPair ;
	static bool indexComp(const indexPair&, const indexPair&) ;
private:
	void doIt(bool undo = false) ;
	specModel* model ;
	typedef QHash<specFolderItem*,QVector<specDeleteCommand::indexPair> > dataHash ;
	dataHash data ;
};

#endif // SPECDELETECOMMAND_H
