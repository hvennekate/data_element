#ifndef SPECMULTIPLOTMOVECOMMAND_H
#define SPECMULTIPLOTMOVECOMMAND_H

#include "specundocommand.h"
#include "qwt_plot_dict.h"
#include "names.h"

class specPlotMoveCommand ;

class specMultiPlotMoveCommand : public specUndoCommand
{
public:
	explicit specMultiPlotMoveCommand(specUndoCommand *parent = 0);
	void redo() ;
	void undo() ;
	QDataStream& write(QDataStream &out) const ;
	QDataStream& read(QDataStream &in) ;
	int id() const { return spec::multiMoveId ; }

	bool mergeWith(const QUndoCommand *other) ;
};

#endif // SPECMULTIPLOTMOVECOMMAND_H
