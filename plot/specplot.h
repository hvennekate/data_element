#ifndef SPECPLOT_H
#define SPECPLOT_H

#include <qwt_plot.h>
#include "specstreamable.h"

class specCanvasItem  ;
class specZoomer ;
class QAction ;
class CanvasPicker ;
class specActionLibrary ;
class specView ;


class specPlot : public QwtPlot, public specStreamable
{
	Q_OBJECT
private:
	bool replotting ;
	specZoomer *zoom ;
	bool scaleX, scaleY ;
	QAction *fixXAxisAction,
		*fixYAxisAction,
		*modifySVGs,
		*printAction ;
	CanvasPicker *MetaPicker, *SVGpicker ; // TODO make pickers more prominent: accessible through
											// function, attach metaRanges direktly to picker etc.
	void readFromStream(QDataStream &in);
	void writeToStream(QDataStream &out) const ;
	type typeId() const {return specStreamable::mainPlot ;}
	specActionLibrary *undoP ;
	void resizeEvent(QResizeEvent *e) ;
protected:
	specView *view ;
	specActionLibrary* undoPartner() const ;
public:
	explicit specPlot ( QWidget *parent=NULL );
	~specPlot();
	CanvasPicker *metaPicker() ;
	CanvasPicker *svgPicker() ;
	virtual QList<QAction*> actions() ;
	void setView(specView* mod) ;
	void setUndoPartner(specActionLibrary* lib) ;
	virtual void attachToPicker(specCanvasItem*) ;
	virtual void detachFromPicker(specCanvasItem*) ;
signals:
	void startingReplot() ;
	void replotted() ;
	void metaRangeModified(specCanvasItem*,int,double,double) ;
public slots :
	void replot() ;
};

#endif
