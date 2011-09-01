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
#include "specdataview.h"
#include <typeinfo>

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
	void addDragDropPartner(specView*) ;
	void dragDrop(QDropEvent* event, specView* destination) ;
signals:

public slots:

private:
	QUndoStack *undoStack ;
	QVector<QWidget*> parents ;
	QVector<specView*> partners;
	void addParent(QWidget*) ;

};

#endif // SPECACTIONLIBRARY_H
