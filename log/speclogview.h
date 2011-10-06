#ifndef SPECLOGVIEW_H
#define SPECLOGVIEW_H

#include "specview.h"
#include "speclogmodel.h"

class specLogView : public specView
{
private:
	Q_OBJECT
public:
	explicit specLogView(QWidget *parent =0);
	void setModel(specLogModel*) ;
};

#endif // SPECLOGVIEW_H
