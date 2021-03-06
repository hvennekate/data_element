#ifndef SPECAPPWINDOW_H
#define SPECAPPWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QList>
#include "specplotwidget.h"
#include <QSettings>

class specShortcutDialog ;

class specAppWindow : public QMainWindow
{
	Q_OBJECT
public:
	specAppWindow();
	~specAppWindow();

private slots:
	void newFile();
	void openFile();
	void openFile(const QString&) ;
#ifdef QT_DEBUG
	void dumpInfo() ;
#endif

protected:
	void closeEvent(QCloseEvent*) ;

private:
	QSettings settings ;
	void createActions();
	void createMenus();
	void createToolBars() ;
	void addDock(specPlotWidget*) ;

	QMenu* fileMenu;
	QMenu* helpMenu;
	QToolBar* fileToolBar;
	QAction* newAction;
	QAction* openAction;
	QAction* whatsThisAction ;
	QAction* restoreSessionAction ;
	QAction* shortCutAction ;
	QAction* searchPathAction ;
#ifdef QT_DEBUG
	QAction* dumpAction ;
#endif
private slots:
	void about() ;
	void whatsThisMode() ;
	void editShortcuts() ;
};

#endif
