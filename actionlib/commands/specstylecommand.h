#ifndef SPECSTYLECOMMAND_H
#define SPECSTYLECOMMAND_H

#include "specundocommand.h"
#include "model/specgenealogy.h"

// This class exists primarily for providing a pointer to the templated class below.
class specStyleCommand : public specUndoCommand
{
public:
	specStyleCommand(specUndoCommand *parent = 0) : specUndoCommand(parent) {}
	virtual void setItems(QList<specModelItem*> list) = 0 ;
	virtual void obtainStyle(specCanvasItem*) = 0 ;
};

specStyleCommand *generateStyleCommand(specStreamable::type id, specUndoCommand *parent = 0) ;

#define specStyleCommandImplTemplate template<class property, \
	property (specCanvasItem::*getProperty)() const, \
	void     (specCanvasItem::*setProperty)(const property&), \
	int ID>
#define specStyleCommandImplFuncTemplate specStyleCommandImplementation<property, getProperty, setProperty, ID>

specStyleCommandImplTemplate
class specStyleCommandImplementation : public specStyleCommand
{
	public:
	explicit specStyleCommandImplementation(specUndoCommand *parent = 0);

	void setItems(QList<specModelItem*> list) ;
	void obtainStyle(specCanvasItem*) ;
	private:
	void applyStyle(specGenealogy&, int) ; // if int == -1: revert to old style
	int styleNo(specCanvasItem*) ;
	void saveStyles(QList<specGenealogy>&) ;
	type typeId() const { return ID ; }

	void parentAssigned();
	void doIt() ;
	void undoIt() ;
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in);

	property newProperty ;
	QVector<property> oldProperties ;
	QList<specGenealogy> Genealogies ;
};

#endif // SPECSTYLECOMMAND_H
