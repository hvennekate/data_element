#ifndef SPECRESIZESVGCOMMAND_H
#define SPECRESIZESVGCOMMAND_H

#include "specundocommand.h"
#include "model/specgenealogy.h"
#include "model/specsvgitem.h"

class specResizeSVGcommand : public specUndoCommand
{
private:
	specGenealogy *item ;
	specSVGItem::bounds other ;
	void doIt() ;
	void undoIt() ;
	void writeToStream(QDataStream &out) const;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::resizeSVGCommandId ; }
	void parentAssigned();
public:
	explicit specResizeSVGcommand(specUndoCommand *parent = 0) ;
	void setItem(const QModelIndex&) ;
	bool ok() ;
	bool mergeable(const specUndoCommand *other) ;
	bool mergeWith(const QUndoCommand *other) ;
};

#endif // SPECRESIZESVGCOMMAND_H
