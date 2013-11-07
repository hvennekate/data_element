#ifndef SPECKINETICWIDGET_H
#define SPECKINETICWIDGET_H

#include "specdockwidget.h"
#include "specplot.h"
#include "specmetamodel.h"
#include "specmetaview.h"

class specKineticWidget : public specDockWidget, public specStreamable
{
	Q_OBJECT
private:
	specPlot* plot ;
	specMetaView* items ;
	void writeToStream(QDataStream& out) const ;
	void readFromStream(QDataStream& in) ;
	type typeId() const { return specStreamable::metaWidget ;}
private slots:
	void svgModification(bool mod) ;
protected:
	QList<QWidget*> mainWidgets() const ;
public:
	specKineticWidget(QWidget* parent = 0);
	specPlot* internalPlot() { return plot ; }
	specMetaView* view() ;
};

#endif
