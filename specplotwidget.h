#ifndef SPECPLOTWIDGET_H
#define SPECPLOTWIDGET_H

#include "specdockwidget.h"
#include <QString>


class specSpectrumPlot ;
class specDataView ;
class QAction ;
class specKineticWidget ;
class specActionLibrary ;
class specLogWidget ;
class QFile ;
class specView ;
class QItemSelection ;
class QToolBar ;

class specPlotWidget : public specDockWidget
{
	Q_OBJECT
private:
	specActionLibrary* actions ;
	specDataView* items ;
	specKineticWidget* kineticWidget ;
	specLogWidget* logWidget ;
	specSpectrumPlot* plot ;
	QAction* saveAction,
		 *kineticsAction,
		 *saveAsAction,
		 *logAction,
		 *undoViewAction,
		 *purgeUndoAction;
	specDockWidget* undoViewWidget ;
	void createWhatsThis() ;
	void setConnections() ;
	void changeFileName(const QString&) ;
	void changeEvent(QEvent* event) ;
	QList<specDockWidget*> subDocks ;
private slots:
	void svgModification(bool) ;
protected :
	void closeEvent(QCloseEvent*) ;
	QList<QWidget*> mainWidgets() const ;
public:
	specPlotWidget(QWidget* parent = 0);
	~specPlotWidget();
	void read(QString fileName) ;
	specView* mainView() ;
	QToolBar* createToolbar();
public slots :
	bool saveFile() ;
};


#endif
