#ifndef SPECMULTICOMMAND_H
#define SPECMULTICOMMAND_H

#include "specundocommand.h"
#include "names.h"


class specMultiCommand : public specUndoCommand
{
private:
	bool mayMerge ; // TODO consider subclassing
public:
	explicit specMultiCommand(specUndoCommand* parent = 0);
	void setMergeable(bool mergeable = true) ;
	void redo() ;
	void undo() ;
	void write(specOutStream &out) const ;
	bool read(specInStream &in) ;
	int id() const { return spec::multiCommandId ; }
//	void setParentWidget(QWidget *) ; // TODO consider making virtual in undocommand
	bool mergeWith(const QUndoCommand *other) ;
};

#endif // SPECMULTICOMMAND_H
