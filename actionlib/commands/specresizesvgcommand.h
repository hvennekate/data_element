#ifndef SPECRESIZESVGCOMMAND_H
#define SPECRESIZESVGCOMMAND_H

#include "specsingleitemcommand.h"
#include "specsvgitem.h"

class specGenealogy ;

class specResizeSVGcommand : public specSingleItemCommand<specSVGItem>
{
private:
	specSVGItem::bounds other ;
	specSVGItem::SVGCornerPoint anchor ;
	void doIt() ;
	void undoIt() ;
	void writeCommand(QDataStream &out) const;
	void readCommand(QDataStream &in) ;
	type typeId() const { return specStreamable::resizeSVGCommandId ; }
public:
	explicit specResizeSVGcommand(specUndoCommand *parent = 0) ;
	void setItem(specModelItem* item, const specSVGItem::bounds&,
			 specSVGItem::SVGCornerPoint anchor = specSVGItem::undefined) ;
	bool mergeable(const specUndoCommand *other) ;
	bool mergeWith(const QUndoCommand *other) ;
};

#endif // SPECRESIZESVGCOMMAND_H
