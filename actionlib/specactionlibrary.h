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

class specView ;
class specModel ;

class specActionLibrary : public QObject
{
	Q_OBJECT
public:
	explicit specActionLibrary(QObject *parent = 0);
//	QMenuBar *menuBar(QObject*) ;
	QToolBar *toolBar(QWidget*) ;
//	QMenu *contextMenu(const QItemSelection&, specModel*) ; // actionLibrary bekommt Clients in fester Reihenfolge; speichert diese mit den Commands und restauriert so deren Referenz.
	QDataStream& write(QDataStream&) ;
	QDataStream& read(QDataStream&) ;
	void push(specUndoCommand*) ;
	QWidget* parentId(int) ;
	void addDragDropPartner(specModel*) ;
	void setLastRequested(const QModelIndexList&) ;
	void moveInternally(const QModelIndex&, int row, specView*) ;

	static specUndoCommand* commandById(int id, specUndoCommand* parent = 0) ;
signals:
	void stackChanged() ;

public slots:

private:
	QUndoStack *undoStack ;
	QVector<QWidget*> parents ;
	QVector<specModel*> partners;
	QModelIndexList lastRequested ;
	void addParent(QWidget*) ;

};

#endif // SPECACTIONLIBRARY_H
