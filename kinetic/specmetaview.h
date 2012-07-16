#ifndef SPECMETAVIEW_H
#define SPECMETAVIEW_H

#include "specview.h"

class specMetaModel ;

class specMetaView : public specView
{
	Q_OBJECT
	specView *dataView ;
	type id() const { return specStreamable::metaView ; }
public:
	explicit specMetaView(QWidget *parent = 0);
	~specMetaView() ;

	void setModel(specMetaModel*) ;
	specMetaModel *model() const ;
	void assignDataView(specView*) ;
	specView* getDataView() ;
};

#endif // SPECMETAVIEW_H
