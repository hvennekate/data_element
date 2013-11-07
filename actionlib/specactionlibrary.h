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
#include "specdockwidget.h"

class specView ;
class specModel ;
class specUndoAction ;
class QUndoView ;
class QProgressDialog ;

class specActionLibrary : public QObject, public specStreamable
{
	Q_OBJECT
public:
	explicit specActionLibrary(QObject* parent = 0);
	~specActionLibrary() ;
	//	QMenuBar *menuBar(QObject*) ;
	QToolBar* toolBar(QWidget*) ;
	QMenu* contextMenu(QWidget*) ;  // actionLibrary bekommt Clients in fester Reihenfolge; speichert diese mit den Commands und restauriert so deren Referenz.
	QObject* parentId(int) ;
	void addDragDropPartner(specModel*) ;
	void setLastRequested(const QModelIndexList&) ;
	int moveInternally(const QModelIndex&, int row, specModel*) ;
	int deleteInternally(specModel*) ;
	void addPlot(specPlot*) ;
	QAction* undoAction(QObject*) ;
	QAction* redoAction(QObject*) ;
	specDockWidget* undoWidget() ;
	specCommandGenerator commandGenerator ;
	void setProgressDialog(QProgressDialog*) ;
public slots:
	void push(specUndoCommand*) ;
signals:
	void stackModified(bool) ;
	void stackIndexChanged() ;
private slots:
	void stackClean(const bool&) ;
	void purgeUndo() ;
private:
	void writeToStream(QDataStream& out) const;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::actionLibrary ; }
	QUndoStack* undoStack ;
	QVector<QObject*> parents ;
	QVector<specModel*> partners;
	QModelIndexList lastRequested ;
	void addParent(QObject*) ;
	void addNewAction(QToolBar*, specUndoAction*) ;
	specStreamable* factory(const type& t) const ;
	QProgressDialog* progress ;
	QAction* purgeUndoAction ;
	specDockWidget* dockWidget ;
};

class specHistoryWidget : public specDockWidget
{
	Q_OBJECT
	QList<QWidget*> mainWidgets() const ;
	QUndoView* undoView ;
public:
	explicit specHistoryWidget(QUndoStack* stack = 0, QWidget* parent = 0) ;
} ;

#endif // SPECACTIONLIBRARY_H
