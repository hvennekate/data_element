#ifndef SPECMETAVIEW_H
#define SPECMETAVIEW_H

#include "specview.h"
#include "actionlib/specactionlibrary.h"

class specMetaModel ;

class specMetaView : public specView
{
	Q_OBJECT
	specView *dataView ;
	type typeId() const { return specStreamable::metaView ; }
	specActionLibrary* undoPartner ;
public:
	explicit specMetaView(QWidget *parent = 0);
	~specMetaView() ;

	void setUndoPartner(specActionLibrary*) ;
	void setModel(specMetaModel*) ;
	specMetaModel *model() const ;
	void assignDataView(specView*) ;
	specView* getDataView() ;
public slots:
	void rangeModified(specCanvasItem*,int,double,double) ;
};

#endif // SPECMETAVIEW_H
