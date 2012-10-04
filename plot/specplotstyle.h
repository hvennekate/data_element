#ifndef SPECPLOTSTYLE_H
#define SPECPLOTSTYLE_H
#include <QPen>
#include <qwt_symbol.h>
#include "speccanvasitem.h"
#include "specstreamable.h"

class specPlotStyle : public specStreamable
{
	qreal lineWidth ;
	QColor penColor, symbolPenColor, symbolBrushColor ;
	qint16 symbolStyle ;
	QSize symbolSize ;
	void initialize(const specCanvasItem*) ;
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::plotStyle ; }
public:
	enum ownSymbolTypes
	{ noSymbol = -2 };

	explicit specPlotStyle(QDataStream&);
	explicit specPlotStyle(const specCanvasItem*);
	specPlotStyle() ;
	void apply(specCanvasItem*) const;
	void retrieve(specCanvasItem*) ;
};

#endif // SPECPLOTSTYLE_H
