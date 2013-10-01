#ifndef SPECPLOT_H
#define SPECPLOT_H

#include <qwt_plot.h>
#include "specstreamable.h"
#include <QLineEdit>

class specCanvasItem  ;
class specZoomer ;
class QAction ;
class CanvasPicker ;
class specActionLibrary ;
class specView ;
class QwtPlotMarker ;

class plotAxisEdit : public QLineEdit
{
	Q_OBJECT
public:
	plotAxisEdit(QWidget* parent = 0) : QLineEdit(parent) { }
public slots:
	void hideAfterEditing(QWidget* old, QWidget* newW)
	{
		Q_UNUSED(old)
		if (newW != (QWidget*) this)
		{
			hide() ;
			disconnect(0,0,this, SLOT(hideAfterEditing(QWidget*, QWidget*))) ;
		}
	}
};

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
	*printAction,
	*legendAction ;
	CanvasPicker *MetaPicker, *SVGpicker ; // TODO make pickers more prominent: accessible through
	// function, attach metaRanges direktly to picker etc.
	QwtPlotMarker *zeroYLine, *zeroXLine ;
	bool autoScaling ;
	void readFromStream(QDataStream &in);
	void writeToStream(QDataStream &out) const ;
	type typeId() const {return specStreamable::mainPlot ;}
	specActionLibrary *undoP ;
	void resizeEvent(QResizeEvent *e) ;
	void autoScale(const QwtPlotItemList& allItems) ;
	void mouseDoubleClickEvent(QMouseEvent *e) ;
	plotAxisEdit xminEdit, xmaxEdit, yminEdit, ymaxEdit ;
	QwtPlotMarker* initializeZeroLine() ;
private slots:
	void setPlotAxis() ;
	void showLegend(bool) ;
	void toggleItem(QwtPlotItem*, bool) ;
	void toggleItem(const QVariant&, bool) ;
protected:
	specView *view ;
	specActionLibrary* undoPartner() const ;
public:
	explicit specPlot ( QWidget *parent=NULL );
	~specPlot();
	QAction* svgAction() const ;
	CanvasPicker *metaPicker() ;
	CanvasPicker *svgPicker() ;
	virtual QList<QAction*> actions() ;
	void setView(specView* mod) ;
	void setUndoPartner(specActionLibrary* lib) ;
	virtual void attachToPicker(specCanvasItem*) ;
	virtual void detachFromPicker(specCanvasItem*) ;
	void setAutoScaling(bool) ;
signals:
	void startingReplot() ;
	void replotted() ;
	void metaRangeModified(specCanvasItem*,int,double,double) ;
public slots :
	void replot() ;
};

#endif
