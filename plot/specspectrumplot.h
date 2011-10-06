#ifndef SPECSPECTRUMPLOT_H
#define SPECSPECTRUMPLOT_H

#include "specplot.h"
#include <QHash>
#include <QList>
#include <QActionGroup>

class specSpectrumPlot : public specPlot
{
	Q_OBJECT
private:
	QAction *offsetAction, *offlineAction, *scaleAction ;
	QActionGroup *correctionActions ;
	CanvasPicker *oldPicker ; // TODO remove
public:
	explicit specSpectrumPlot(QWidget *parent = 0);
	QActionGroup *actions() { return correctionActions ; }

signals:

private slots:
	void correctionsChanged() ;

};

#endif // SPECSPECTRUMPLOT_H
