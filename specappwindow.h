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
	
protected:
// 	void closeEvent(QCloseEvent *event);
	
private slots:
	void newFile();
	void openFile();
// 	bool save();
// 	bool saveAs();
// 	void about();
// 	void documentWasModified();
// 	void merge();
	
protected:
	void closeEvent(QCloseEvent*) ;
	
private:
	QSettings settings ;
	void createActions();
	void createMenus();
	void createToolBars();
// 	void createStatusBar();
// 	void readSettings();
// 	void writeSettings();
// 	bool maybeSave();
// 	void loadFile(const QString &fileName);
// 	bool saveFile(const QString &fileName);
// 	void setCurrentFile(const QString &fileName);
// 	QString strippedName(const QString &fullFileName);
	
	QMenu *fileMenu;
// 	QMenu *editMenu;
// 	QMenu *helpMenu;
	QToolBar *fileToolBar;
// 	QToolBar *editToolBar;
	QAction *newAction;
	QAction *openAction;
// 	QAction *saveAct;
// 	QAction *saveAsAct;
// 	QAction *exitAct;
// 	QAction *cutAct;
// 	QAction *copyAct;
// 	QAction *pasteAct;
// 	QAction *aboutAct;
// 	QAction *aboutQtAct;
// 	QAction *mergeAct;
};

#endif
