#ifndef SPECKRCONTEXTMENU_H
#define SPECKRCONTEXTMENU_H

#include <QMenu>
#include "speccanvasitem.h"

class specKRContextMenu : public QMenu
{
	Q_OBJECT
private:
	specCanvasItem *pointer ;
	QAction *deleteAction, *lineWidthAction ;
private slots:
	void deleteItem() ;
	void changeLineWidth() ;
public:
	specKRContextMenu ( specCanvasItem *item = 0 );
	~specKRContextMenu();
};

#endif
