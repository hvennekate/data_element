#ifndef SPECVIEWSTATE_H
#define SPECVIEWSTATE_H

#include <QVector>
#include "specmodel.h"
#include <QByteArray>
#include "specview.h"
class specView ;
class specModel ;

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
	inline specModel* model() { return parent ? parent->model() : 0 ; }
	specModelItem* hierarchyPointer(const QVector<int>&) ;
public:
	explicit specViewState(specView* Parent) ;
	void setParent(specView* Parent) ;
	void getState() ;
	void restoreState() ;
	QDataStream& write(QDataStream&) ;
	QDataStream& read(QDataStream&) ;
};

#endif // SPECVIEWSTATE_H
