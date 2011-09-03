#ifndef SPECAPPWINDOW_H
#define SPECAPPWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QObject>
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
	void createToolBars() ;
	
protected:
	void closeEvent(QCloseEvent*) ;
	
private:
	QSettings settings ;
	void createActions();
	void createMenus();
//	void createToolBars();
// 	void createStatusBar();
// 	void readSettings();
// 	void writeSettings();
	
	QMenu *fileMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;
	QAction *newAction;
	QAction *openAction;
};

#endif
