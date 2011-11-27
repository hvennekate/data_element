#ifndef SPECMETAVIEW_H
#define SPECMETAVIEW_H

#include "specview.h"

class specMetaModel ;

class specMetaView : public specView
{
    Q_OBJECT
public:
	explicit specMetaView(QWidget *parent = 0);
	~specMetaView() ;

	void setModel(specMetaModel*) ;
	specMetaModel *model() const ;
};

#endif // SPECMETAVIEW_H
