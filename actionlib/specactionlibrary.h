#ifndef SPECACTIONLIBRARY_H
#define SPECACTIONLIBRARY_H

class specActionLibrary ;

#include <QObject>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QDataStream>
#include <QUndoStack>
#include "specundocommand.h"
#include "specundoaction.h"
#include "specview.h"
#include "specmodel.h"
#include <typeinfo>
#include "specplot.h"

class specView ;
class specModel ;
class specUndoAction ;

class specActionLibrary : public QObject
{
	Q_OBJECT
public:
	explicit specActionLibrary(QObject *parent = 0);
//	QMenuBar *menuBar(QObject*) ;
	QToolBar *toolBar(QWidget*) ;
//	QMenu *contextMenu(const QItemSelection&, specModel*) ; // actionLibrary bekommt Clients in fester Reihenfolge; speichert diese mit den Commands und restauriert so deren Referenz.
	void write(specOutStream&) ;
	bool read(specInStream&) ;
	void push(specUndoCommand*) ;
	QObject* parentId(int) ;
	void addDragDropPartner(specModel*) ;
	void setLastRequested(const QModelIndexList&) ;
	void moveInternally(const QModelIndex&, int row, specView*) ;
	void addPlot(specPlot*) ;

	static specUndoCommand* commandById(int id, specUndoCommand* parent = 0) ;
	QAction* undoAction(QObject*) ;
	QAction* redoAction(QObject*) ;

signals:
	void stackChanged() ;
private:
	QUndoStack *undoStack ;
	QVector<QObject*> parents ;
	QVector<specModel*> partners;
	QModelIndexList lastRequested ;
	void addParent(QObject*) ;
	void addNewAction(QToolBar*, specUndoAction*) ;
};

#endif // SPECACTIONLIBRARY_H
