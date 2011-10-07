#ifndef SPECSPECTRUMPLOT_H
#define SPECSPECTRUMPLOT_H

#include "specplot.h"
#include <QHash>
#include <QList>
#include <QActionGroup>
#include "actionlib/specactionlibrary.h"
#include "specmodel.h"

class specSpectrumPlot : public specPlot
{
	Q_OBJECT
private:
	QAction *offsetAction, *offlineAction, *scaleAction, *shiftAction ;
	QActionGroup *correctionActions ;
	CanvasPicker *picker ;
	specActionLibrary *undoPartner ;
	QHash<specCanvasItem*, QList<int> > pointHash ;
	specModel *model ;
public:
	explicit specSpectrumPlot(QWidget *parent = 0);
	QActionGroup *actions() { return correctionActions ; }
	void setModel(specModel* mod) { model = mod ; }

signals:

private slots:
	void correctionsChanged() ;
	void pointMoved(specCanvasItem*,int point, double x, double y) ;

};

#endif // SPECSPECTRUMPLOT_H
