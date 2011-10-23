#ifndef SPECLINECOLORCOMMAND_H
#define SPECLINECOLORCOMMAND_H

#include "specstylecommand.h"

class specLineColorCommand : public specStyleCommand
{
public:
	explicit specLineColorCommand(specUndoCommand* parent = 0);
	void setStyle(specCanvasItem *) ;

	QDataStream& write(QDataStream &out) const ;
	QDataStream& read(QDataStream &in) ;
protected:
	void applyStyle(const specGenealogy &, int) ;
	int styleNo(specCanvasItem*);
	void saveStyles(const QList<specGenealogy> &) ;
private:
	QColor newColor ;
	QVector<QColor> oldColors ;
};

#endif // SPECLINECOLORCOMMAND_H
