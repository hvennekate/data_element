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
	// TODO write function for conversion of data for plot
// public:
	QWidget* content ;
	QVBoxLayout* layout ;
	specSpectrumPlot* plot ;
	specDataView* items ;
	QToolBar* toolbar ;
	QSplitter* splitter;
	QFile* onDisk ;
	QAction *saveAction,
		*kineticsAction,
		*saveAsAction,
		*logAction,
		*printAction ;
	specActionLibrary *actions ;
	specLogWidget *logWidget ;
		
	specKineticWidget *kineticWidget ;
	void createActions() ;
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
    specPlotWidget(const QString& fileName = "", QWidget *parent = 0);

    ~specPlotWidget();
	specView* mainView() { return items ; } // TODO dangerous
public slots :
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) ; // TODO consider making these private
	void setFont() ;
	bool saveFile() ;
// 	void zeroCorrection() ;
//	void addZeroRange() ;
signals :
	void addItem(int position, int rows, const QModelIndex &index) ;

};

#endif
