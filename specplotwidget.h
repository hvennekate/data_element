#ifndef SPECPLOTWIDGET_H
#define SPECPLOTWIDGET_H

#include <QDockWidget>
#include <QString>

class specSpectrumPlot ;
class QVBoxLayout ;
class specDataView ;
class QToolBar ;
class QAction ;
class QSplitter ;
class specKineticWidget ;
class specActionLibrary ;
class specLogWidget ;
class QFile ;
class specView ;
class QItemSelection ;




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
	*undoAction,
	*redoAction;
	specActionLibrary *actions ;
	void createToolbars();
	void setConnections() ;
	void changeFileName(const QString&) ;
private slots:
	void modified() ;
	void svgModification(bool) ;
protected :
	void contextMenuEvent(QContextMenuEvent*) ;
	void closeEvent(QCloseEvent*) ;
public:
	specPlotWidget(QWidget *parent = 0);
	~specPlotWidget();

	void read(QString fileName) ;
	specView* mainView() ;
public slots :
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) ; // TODO consider making these private
	bool saveFile() ;
};

#endif
