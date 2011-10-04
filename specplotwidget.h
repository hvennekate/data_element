#ifndef SPECPLOTWIDGET_H
#define SPECPLOTWIDGET_H

#include <QDockWidget>
#include "specplot.h"
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

class specPlotWidget : public QDockWidget
{
Q_OBJECT
private:
	// TODO write function for conversion of data for plot
// public:
	QWidget* content ;
	QVBoxLayout* layout ;
	specPlot* plot ;
	specDataView* items ;
	specLogView* logs ;
	QToolBar* toolbar ;
	QSplitter* splitter;
	QFile* onDisk ;
	QAction *saveAction,
		*kineticsAction,
		*toKineticAction,
   		*fromKineticAction,
     		*saveAsAction ;
	specActionLibrary *actions ;
		
	specKineticWidget *kineticWidget ;
	void createActions() ;
	void createToolbars();
	
	void setConnections() ;
	void changeFileName(const QString&) ;
private slots:
	void modified() ;
	void scaleCurve() ;
	void offsetCurve() ;
	void offlineCurve() ;
	void currentChanged(const QModelIndex&, const QModelIndex&) ;
	void newKinetics() ;
	void toKinetic() ;
	void fromKinetic() ;
protected :
	void contextMenuEvent(QContextMenuEvent*) ;
	void closeEvent(QCloseEvent*) ;
public:
    specPlotWidget(const QString& fileName = "", QWidget *parent = 0);

    ~specPlotWidget();

    
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
