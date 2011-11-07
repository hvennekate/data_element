#ifndef SPECSTYLECOMMAND_H
#define SPECSTYLECOMMAND_H

#include "specundocommand.h"
#include "model/specgenealogy.h"

class specStyleCommand : public specUndoCommand
{
public:
	specStyleCommand(specUndoCommand *parent = 0) : specUndoCommand(parent) {}
	virtual void setItems(QModelIndexList)    = 0 ;
	virtual void obtainStyle(specCanvasItem*) = 0 ;
};

specStyleCommand *generateStyleCommand(spec::undoActionIds id) ;

#define specStyleCommandImplTemplate template<class property, \
					      property (specCanvasItem::*getProperty)(), \
					      void     (specCanvasItem::*setProperty)(const property&), \
					      int ID>
#define specStyleCommandImplFuncTemplate specStyleCommandImplementation<property, getProperty, setProperty, ID>

specStyleCommandImplTemplate
class specStyleCommandImplementation : public specStyleCommand
{
public:
	explicit specStyleCommandImplementation(specUndoCommand *parent = 0);
	QDataStream& write(QDataStream& out) const ;
	QDataStream& read(QDataStream& in) ;
	int id() const { return ID ; }

	void redo() ;
	void undo() ;

	void setItems(QModelIndexList) ;
	void obtainStyle(specCanvasItem*) ;
private:
	void applyStyle(const specGenealogy&, int) ; // if int == -1: revert to old style
	int styleNo(specCanvasItem*) ;
	void saveStyles(const QList<specGenealogy>&) ;
private:
	property newProperty ;
	QVector<property> oldProperties ;
	QList<specGenealogy> Genealogies ;
};

#endif // SPECSTYLECOMMAND_H
