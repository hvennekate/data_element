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
#include "specmodel.h"
#include "specview.h"
#include <typeinfo>
#include "specplot.h"
#include "speccommandgenerator.h"

class specView ;
class specModel ;
class specUndoAction ;
class QUndoView ;
class QProgressDialog ;

class specActionLibrary : public QObject, public specStreamable
{
	Q_OBJECT
public:
	explicit specActionLibrary(QObject *parent = 0);
    ~specActionLibrary() ;
//	QMenuBar *menuBar(QObject*) ;
	QToolBar *toolBar(QWidget*) ;
	QMenu *contextMenu(QWidget*) ; // actionLibrary bekommt Clients in fester Reihenfolge; speichert diese mit den Commands und restauriert so deren Referenz.
	QObject* parentId(int) ;
	void addDragDropPartner(specModel*) ;
	void setLastRequested(const QModelIndexList&) ;
	int moveInternally(const QModelIndex&, int row, specView*) ;
    int deleteInternally(specModel*) ;
	void addPlot(specPlot*) ;
	QAction* undoAction(QObject*) ;
	QAction* redoAction(QObject*) ;
	void purgeUndo() ;
	QUndoView* undoView() ;
	specCommandGenerator commandGenerator ;
    void setProgressDialog(QProgressDialog*) ;
public slots:
	void push(specUndoCommand*) ;
signals:
    void stackClean(bool) ;
private:
	void writeToStream(QDataStream &out) const;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::actionLibrary ; }
	QUndoStack *undoStack ;
	QVector<QObject*> parents ;
	QVector<specModel*> partners;
	QModelIndexList lastRequested ;
	void addParent(QObject*) ;
	template<class toolMenu>
	void addNewAction(toolMenu*, specUndoAction*) ;
    specStreamable* factory(const type &t) const ;
    QProgressDialog *progress ;
};

#endif // SPECACTIONLIBRARY_H
