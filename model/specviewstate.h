#ifndef SPECVIEWSTATE_H
#define SPECVIEWSTATE_H

#include <QVector>
#include "specmodel.h"
#include <QByteArray>
#include "specview.h"
class specView ;
class specModel ;

class specViewState : public specStreamable
{
private:
	specView* parent ;
	QVector<specFolderItem*> openFolders ;
	QVector<specModelItem*> selectedItems ;
	specModelItem* currentTopItem ;
	specModelItem* currentItem ;
	QVector<int> hierarchyOfTopItem ;
	QVector<int> hierarchyOfCurrentItem ;
	QVector<qint32> widths ;
	void purgeLists() ;
	inline specModel* model() const { return parent ? parent->model() : 0 ; }
	specModelItem* hierarchyPointer(const QVector<int>&) ;
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::viewState ; }
public:
	explicit specViewState(specView* Parent) ;
	explicit specViewState(const specView* Parent) ;
	void setParent(specView* Parent) ;
	void getState(const specView* view) ;
	void getState() ;
	void restoreState() ;
	static QItemSelection indexesToSelection(const QModelIndexList& selectedItems, const specModel* model) ;
	static QItemSelection pointersToSelection(const QVector<specModelItem*>& selectedItems, const specModel* model) ;
};

#endif // SPECVIEWSTATE_H
