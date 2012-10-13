#ifndef SPECMULTICOMMAND_H
#define SPECMULTICOMMAND_H

#include "specundocommand.h"
#include "names.h"
#include "speccommandgenerator.h"


class specMultiCommand : public specUndoCommand
{
private:
	bool mayMerge ; // TODO consider subclassing
	void doIt() ;
	void undoIt() ;
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in) ;
	type typeId() const { return specStreamable::multiCommandId ; }
	void parentAssigned();
	specStreamable* factory(const type& t) const ;
	specCommandGenerator commandGenerator ;
public:
	explicit specMultiCommand(specUndoCommand* parent = 0);
	void setMergeable(bool mergeable = true) ;
	bool mergeWith(const QUndoCommand *other) ;
};

#endif // SPECMULTICOMMAND_H
