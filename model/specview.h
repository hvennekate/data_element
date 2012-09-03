#ifndef SPECVIEW_H
#define SPECVIEW_H

#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include "specmodel.h"
#include <QMap>
#include "actionlib/specactionlibrary.h"

class specViewState ;
class specModel ;

class specView : public QTreeView, public specStreamable
{ // TODO turn into abstract class (add purely virtual function)
Q_OBJECT
private:
	QMenu *itemContextMenu, *folderContextMenu ;
	QAction *deleteAction, *newItemAction, *treeAction, *changePenAction, *mergeFolderAction, *mergeAction, *exportAction, *cutByIntensityAction, *averageAction, *movingAverageAction, *getSubtractionDataAction, *applySubtractionAction, *propertiesAction ;
	specViewState *state ;

	void createContextMenus() ;
	void createActions() ;
	void contextMenuEvent(QContextMenuEvent*) ;
	void triggerReselect() ;
	bool setAllSelected(const QVariant&, int role = Qt::EditRole) ;
	type typeId() const { return specStreamable::mainView ; }
	bool acceptData(const QMimeData*) ;
	void dragMoveEvent(QDragMoveEvent *event) ;
	void dragEnterEvent(QDragEnterEvent *event) ;
private slots:
	void averageItems() ;
	void itemProperties() ;
	void columnsInserted(const QModelIndex& parent, int start, int end) ;
protected:
	void keyPressEvent(QKeyEvent*) ;
	void dropEvent(QDropEvent *event) ;
	void readFromStream(QDataStream &in) ;
	void writeToStream(QDataStream &out) const ;
protected slots:
	void columnMoved(int,int,int) ;
public:
	specView(QWidget* parent=0);
	virtual ~specView();
	
	specModel* model () const ;
	void setModel(specModel*) ; // TODO make virtual or find better solution!
//	QList<specModelItem*> currentlySelected() ;
	virtual QList<QAction*> actions() ;
	QModelIndexList getSelection() ;

signals:
	void changed() ;
	void newUndoCommand(specUndoCommand*) ;
public slots:
	void deleteItems() ;
	void newFolder() ;
	void buildTree() ;
	void changePen() ;
	void mergeFolder() ;
	void mergeItems() ;
	void currentlySelectedToSubMap() ;
	void applySubMapToSelection() ;
	void exportItems() ;
	void prepareReset() ;
	void resetDone() ;
};

#endif
