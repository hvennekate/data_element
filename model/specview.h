#ifndef SPECVIEW_H
#define SPECVIEW_H

#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <specmodel.h>
#include <QMap>
#include "actionlib/specactionlibrary.h"

class specView : public QTreeView
{ // TODO turn into abstract class (add purely virtual function)
Q_OBJECT
private:
	QMenu *itemContextMenu, *folderContextMenu ;
	QAction *deleteAction, *newItemAction, *treeAction, *changePenAction, *mergeFolderAction, *mergeAction, *exportAction, *cutByIntensityAction, *averageAction, *movingAverageAction, *getSubtractionDataAction, *applySubtractionAction ;

	specActionLibrary* dropBuddy ;
	
	void createContextMenus() ;
	void createActions() ;
	void contextMenuEvent(QContextMenuEvent*) ;
	void triggerReselect() ;
	bool setAllSelected(const QVariant&, int role = Qt::EditRole) ;
private slots:
	void averageItems() ;
protected:
	void keyPressEvent(QKeyEvent*) ;
	void dropEvent(QDropEvent *event) ;
protected slots:
	void columnMoved(int,int,int) ;
public:
	specView(QWidget* parent=0);
	~specView();
	
	specModel* model () const ;
	void setModel(specModel*) ;
//	QList<specModelItem*> currentlySelected() ;
	virtual QList<QAction*> actions() ;
	QModelIndexList getSelection() ;
	void setDropBuddy(specActionLibrary*) ;

	signals:
		void changed() ;
public slots:
	void deleteItems() ;
	void newFolder() ;
	void buildTree() ;
	void changePen() ;
	void mergeFolder() ;
	void mergeItems() ;
	void cutByIntensity() ;
	void currentlySelectedToSubMap() ;
	void applySubMapToSelection() ;
	void exportItems() ;
};

#endif
