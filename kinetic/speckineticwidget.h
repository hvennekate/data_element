#ifndef SPECKINETICWIDGET_H
#define SPECKINETICWIDGET_H

#include <QDockWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QToolBar>
#include "specplot.h"
#include "specmetamodel.h"
#include "specmetaview.h"

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
	QSplitter *splitter ;
	specMetaView *items ;
private slots:
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) ;
protected:
	void contextMenuEvent(QContextMenuEvent*) ;
public:
	specKineticWidget ( QString, QWidget *parent = 0 );
	~specKineticWidget();
	void addToolbar(specActionLibrary*) ;

	specPlot *internalPlot() { return plot ; }
	
	specMetaView *view() ;
	
	friend QDataStream& operator<<(QDataStream&, const specKineticWidget&);
	friend QDataStream& operator>>(QDataStream&, specKineticWidget&);
};

#endif
