#ifndef SPECPLOTWIDGET_H
#define SPECPLOTWIDGET_H

#include <QDockWidget>
#include "specspectrumplot.h"
#include <QVBoxLayout>
#include "specdataview.h"
#include <QString>
#include <QToolBar>
#include "specmodel.h"
#include <QAction>
#include <QSplitter>
#include <QFile>
#include <QMenu>
#include <specdocktitle.h>
#include "speckineticwidget.h"
#include "actionlib/specactionlibrary.h"
#include "log/speclogview.h"
#include "log/speclogmodel.h"
#include "log/speclogwidget.h"




class specPlotWidget : public QDockWidget
{
	Q_OBJECT
private:
	specDataView* items ;
	specLogWidget *logWidget ;
	specKineticWidget *kineticWidget ;
	QWidget* content ;
	QVBoxLayout* layout ;
	specSpectrumPlot* plot ;
	QToolBar* toolbar ;
	QSplitter* splitter;
	QFile* file ;
	QAction *saveAction,
	*kineticsAction,
	*saveAsAction,
	*logAction,
	*printAction ;
	specActionLibrary *actions ;
	void createToolbars();
	void setConnections() ;
	void changeFileName(const QString&) ;
private slots:
	void modified() ;
	void currentChanged(const QModelIndex&, const QModelIndex&) ;
protected :
	void contextMenuEvent(QContextMenuEvent*) ;
	void closeEvent(QCloseEvent*) ;
public:
	specPlotWidget(QWidget *parent = 0);
	~specPlotWidget();

	void read(QString fileName) ;
	specView* mainView() { return items ; } // TODO dangerous
public slots :
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) ; // TODO consider making these private
	bool saveFile() ;
signals :
	void addItem(int position, int rows, const QModelIndex &index) ;

};

#endif
