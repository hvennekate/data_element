#ifndef SPECKINETICWIDGET_H
#define SPECKINETICWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <qwt_plot.h>
#include <QToolBar>
#include "speckinetic.h"
#include "specmodelitem.h"
#include "specplot.h"
#include "speckineticview.h"
#include "speckineticmodel.h"

class specKineticWidget;
QDataStream& operator<<(QDataStream&, const specKineticWidget&);
QDataStream& operator>>(QDataStream&, specKineticWidget&);

class specKineticWidget : public QDockWidget
{
	Q_OBJECT
private:
	QVBoxLayout *layout ;
	specPlot *plot ;
	QWidget *content ;
	QWidget *directParent ;
	QSplitter *splitter ;
	specKineticView *items ;
	QToolBar* toolbar ;
	QList<specKinetic*> kinetics ;
	QList<specKinetic*>::size_type currentKinetic ;
	QAction *syncAction ;
	void setupActions() ;
private slots:
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) ;
// 	void currentChanged(const QItemSelection & selected, const QItemSelection & deselected) ;
protected:
	void contextMenuEvent(QContextMenuEvent*) ;
public:
	specKineticWidget ( QString, QWidget *parent = 0 );
	~specKineticWidget();
	
	specKineticView *view() ;
	
	friend QDataStream& operator<<(QDataStream&, const specKineticWidget&);
	friend QDataStream& operator>>(QDataStream&, specKineticWidget&);

public slots:
	void addKinetic() ;
// 	void syncCurrent() ;

};

#endif
