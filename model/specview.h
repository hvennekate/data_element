#ifndef SPECVIEW_H
#define SPECVIEW_H

#include <QTreeView>
#include "specstreamable.h"

class specViewState ;
class specModel ;
class specActionLibrary ;
class QContextMenuEvent ;
class specUndoCommand ;

class specView : public QTreeView, public specStreamable
{ // TODO turn into abstract class (add purely virtual function)
Q_OBJECT
private:
	specViewState *state ;

	void contextMenuEvent(QContextMenuEvent*) ;
	void triggerReselect() ;
	bool setAllSelected(const QVariant&, int role = Qt::EditRole) ;
	type typeId() const { return specStreamable::mainView ; }
	bool acceptData(const QMimeData*) ;
    void dragMoveEvent(QDragMoveEvent *event) ;
    void dragEnterEvent(QDragEnterEvent *event) ;
private slots:
	void columnsInserted(const QModelIndex& parent, int start, int end) ;
protected:
	void keyPressEvent(QKeyEvent*) ;
	void dropEvent(QDropEvent *event) ;
	void readFromStream(QDataStream &in) ;
	void writeToStream(QDataStream &out) const ;
	specActionLibrary *actionLibrary ;
protected slots:
	void columnMoved(int,int,int) ;
public:
	specView(QWidget* parent=0);
	virtual ~specView();
	
	specModel* model () const ;
	void setModel(specModel*) ; // TODO make virtual or find better solution!
//	QList<specModelItem*> currentlySelected() ;
	QModelIndexList getSelection() ;
	void setActionLibrary(specActionLibrary*) ;

signals:
	void changed() ;
	void newUndoCommand(specUndoCommand*) ;
public slots:
	void prepareReset() ;
	void resetDone() ;
};

#endif
