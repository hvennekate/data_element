#ifndef SPECSTYLECOMMAND_H
#define SPECSTYLECOMMAND_H

#include "specundocommand.h"
#include "model/specgenealogy.h"

class specStyleCommand : public specUndoCommand
{
public:
	explicit specStyleCommand(specUndoCommand *parent = 0);
	QDataStream& write(QDataStream& out) const ;
	QDataStream& read(QDataStream& in) ;

	void redo() ;
	void undo() ;

	void setItems(QModelIndexList) ;
	virtual void setStyle(specCanvasItem*) = 0;
protected:
	virtual void applyStyle(const specGenealogy&, int) = 0 ; // if int == -1: revert to old style
	virtual int styleNo(specCanvasItem*) = 0;
	virtual void saveStyles(const QList<specGenealogy>&) = 0;
private:
	QList<specGenealogy> Genealogies ;
};

#endif // SPECSTYLECOMMAND_H
