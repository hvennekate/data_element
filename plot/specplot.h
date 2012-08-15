#ifndef SPECPLOT_H
#define SPECPLOT_H

#include <qwt_plot.h>
#include "canvaspicker.h"
#include "speczoomer.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QActionGroup>
#include "names.h"
#include "speccanvasitem.h"
#include "specstreamable.h"

class specMetaRange ;
class specSimpleTextEdit ;

class specPlot : public QwtPlot, public specStreamable
{
	Q_OBJECT
private:
	bool replotting ;
	QList<specCanvasItem*>* canBeSelected ;
	QList<specCanvasItem*>* ranges ;
	QList<specCanvasItem*>* ordinary ;
	QList<specCanvasItem*>* kineticRanges ;
	QList<specCanvasItem*>* selectRanges ;
	specZoomer   *zoom ;
	bool scaleX, scaleY ;
	void contextMenuEvent ( QContextMenuEvent* ) ;
	QMenu *contextMenu ;
	QAction *titleAction,
		*ylabelAction,
		*xlabelAction,
		*fixXAxisAction,
		*fixYAxisAction,
		*modifySVGs ;
	QAction *scaleAction,
	*offsetAction,
	*offlineAction,
	*shiftXAction,
	*zeroCorrectionAction,
	*addZeroRangeAction,
	*deleteZeroRangeAction,
	*individualZeroAction,
	*multipleCorrections ;
	QActionGroup *zero, *modifications ;
	spec::moveMode mm ;
	CanvasPicker *metaPicker ;
	specSimpleTextEdit *textEdit ;
	void setupActions() ;
	void highlightSelectable(bool highlight=true) ;
	void readFromStream(QDataStream &in);
	void writeToStream(QDataStream &out) const ;
	type typeId() const {return specStreamable::mainPlot ;}
private slots:
	void changeTitle(QString) ;
	void changeXLabel(QString) ;
	void changeYLabel(QString) ;
public:
	bool select ;
	QList<specCanvasItem*>* selectable() ;
	specPlot ( QWidget *parent=NULL );
	~specPlot();
	specZoomer   *zoomer() ;
	virtual QList<QAction*> actions() ;
	spec::moveMode moveMode() ;
	void refreshRanges() ;
signals:
	void startingReplot() ;
	void replotted() ;
	void metaRangeModified(specCanvasItem*,int,double,double) ;
public slots :
	void replot() ;
	void changeTextLabel() ;
	void refreshMoveMode() ;
	void addRange() ;
	void deleteRange() ;
};

#endif
