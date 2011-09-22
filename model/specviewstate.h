#ifndef SPECVIEWSTATE_H
#define SPECVIEWSTATE_H

#include <QVector>
#include "specmodel.h"
#include <QByteArray>

class specView ;

class specViewState
{
private:
	specView *parent ;
	QVector<specFolderItem*> openFolders ;
	QVector<specModelItem*> selectedItems ;
	specModelItem* currentTopItem ;
	specModelItem* currentItem ;
	QVector<int> hierarchyOfTopItem ;
	QVector<int> hierarchyOfCurrentItem ;
	QVector<qint32> widths ;
	QByteArray geometry ;
	void purgeLists() ;
	inline specModel* model() { return parent ? parent->model() : 0 }

public:
	explicit specViewState(specView* Parent) ;
	void setParent(specView* Parent) ;
	void getState() ;
	void restoreState() ;
	QDataStream& write(QDataStream&) ;
	QDataStream& read(QDataStream&) ;
};

#endif // SPECVIEWSTATE_H
