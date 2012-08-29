#ifndef SPECAPPWINDOW_H
#define SPECAPPWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QList>
#include "specplotwidget.h"
#include <QSettings>

class specAppWindow : public QMainWindow
{
	Q_OBJECT
public:
	specAppWindow();
	~specAppWindow();
	
private slots:
	void newFile();
	void openFile();
	void removeDock() ;
	
protected:
	void closeEvent(QCloseEvent*) ;
	
private:
	QSettings settings ;
	void createActions();
	void createMenus();
	void createToolBars() ;
	void addDock(specPlotWidget*) ;
	
	QMenu *fileMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;
	QAction *newAction;
	QAction *openAction;
	QList<specPlotWidget*> docks ;
private slots:
	void about() ;
};

#endif
