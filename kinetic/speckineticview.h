#ifndef SPECKINETICVIEW_H
#define SPECKINETICVIEW_H

#include <specview.h>
#include "speckineticmodel.h"

class specKineticView : public specView
{
	Q_OBJECT
	private:
		QAction *newItemAction, *newRangeAction ;
public:
	specKineticView(QWidget* parent=0);
	~specKineticView();
	
	void setModel(specKineticModel*) ;
	specKineticModel* model () const ;
	QList<QAction*> actions() ;
	void addToCurrent(const QModelIndexList&) ;
	
	specPlot* plot ;
	
	public slots:
		void newItem() ;
		void newRange() ;

};

#endif
