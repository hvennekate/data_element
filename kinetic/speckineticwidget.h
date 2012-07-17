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
class specKineticWidget : public QDockWidget, public specStreamable
{
	Q_OBJECT
private:
	QVBoxLayout *layout ;
	specPlot *plot ;
	QWidget *content ;
	QSplitter *splitter ;
	specMetaView *items ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::metaWidget ;}
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
	
};

#endif
