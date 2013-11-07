#ifndef SPECMETAVIEW_H
#define SPECMETAVIEW_H

#include "specview.h"

class specCanvasItem ;

class specMetaModel ;

class specMetaView : public specView
{
	Q_OBJECT
	specView* dataView ;
	type typeId() const { return specStreamable::metaView ; }
protected:
	void readFromStream(QDataStream& in) ;
public:
	explicit specMetaView(QWidget* parent = 0);
	~specMetaView() ;

	void setModel(specMetaModel*) ;
	specMetaModel* model() const ;
	void assignDataView(specView*) ;
	specView* getDataView() ;
public slots:
	void rangeModified(specCanvasItem*, int, double, double) ;
};

#endif // SPECMETAVIEW_H
