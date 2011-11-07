#ifndef SPECMULTICOMMAND_H
#define SPECMULTICOMMAND_H

#include "specundocommand.h"
#include "names.h"


class specMultiCommand : public specUndoCommand
{
public:
	explicit specMultiCommand(specUndoCommand* parent = 0);
	void redo() ;
	void undo() ;
	QDataStream& write(QDataStream &out) const ;
	QDataStream& read(QDataStream &in) ;
	int id() const { return spec::multiCommandId ; }

	bool mergeWith(const QUndoCommand *other) ;
};

#endif // SPECMULTICOMMAND_H
