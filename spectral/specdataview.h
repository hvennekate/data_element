#ifndef SPECDATAVIEW_H
#define SPECDATAVIEW_H

#include <specview.h>
#include <QPersistentModelIndex>

class specDataView : public specView
{
	Q_OBJECT
private:
	QAction *importAction ; //, *getPersistent, *printPersistent;
	QPersistentModelIndex index ;
public:
	specDataView ( QWidget* parent=0 );
	~specDataView();

	QList<QAction*> actions() ;
public slots:
	void importFile() ;
// 	void aqPersistent() ;
// 	void outputPersistent() ;
};

#endif
