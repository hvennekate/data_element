#ifndef SPECDATAVIEW_H
#define SPECDATAVIEW_H

#include "specview.h"
#include <QPersistentModelIndex>

class specDataView : public specView
{
	Q_OBJECT
public:
	specDataView ( QWidget* parent=0 );
	~specDataView();

};

#endif
