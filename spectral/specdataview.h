#ifndef SPECDATAVIEW_H
#define SPECDATAVIEW_H

#include "specview.h"
#include <QPersistentModelIndex>

class specDataView : public specView
{
	Q_OBJECT
private:
	type typeId() const { return specStreamable::dataView ;}
public:
	specDataView (QWidget* parent=0 );
	~specDataView();

};

#endif
