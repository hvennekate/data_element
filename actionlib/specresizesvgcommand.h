#ifndef SPECRESIZESVGCOMMAND_H
#define SPECRESIZESVGCOMMAND_H

#include "specundocommand.h"
#include "model/specgenealogy.h"

class specResizeSVGcommand : public specUndoCommand
{
private:
	specGenealogy *item ;
	QRectF other ;
	void exchange() ;
public:
	explicit specResizeSVGcommand(specUndoCommand *parent = 0) ;
	void setItem(const QModelIndex&, const QRectF&) ;
	bool ok() ;
	void write(specOutStream &out) const ;
	bool read(specInStream &in) ;

	bool mergeable(const specUndoCommand *other) ;
	bool mergeWith(const QUndoCommand *other) ;

	void redo() ;
	void undo() ;
	int id() const { return spec::resizeSVGCommandId ; }
};

#endif // SPECRESIZESVGCOMMAND_H
