#ifndef SPECMULTICOMMAND_H
#define SPECMULTICOMMAND_H

#include "specundocommand.h"
#include "names.h"


class specMultiCommand : public specUndoCommand
{
private:
	bool mayMerge ; // TODO consider subclassing
	void doIt() ;
	void undoIt() ;
	void writeToStream(QDataStream &out) const;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::multiCommandId ; }
//	void parentAssigned();
public:
	explicit specMultiCommand(specUndoCommand* parent = 0);
	void setMergeable(bool mergeable = true) ;
	bool mergeWith(const QUndoCommand *other) ;
};

#endif // SPECMULTICOMMAND_H
