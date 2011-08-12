#ifndef SPECKINETIC_H
#define SPECKINETIC_H

#include "specmodelitem.h"
#include <QMultiMap>
#include <QMapIterator>
#include <QPair>
//#include <ginac/ginac.h>
#include "specmodel.h"
#include "specplot.h"
#include <QQueue>
#include <QStack>

class specKineticRange ;

class specKinetic : public specModelItem
{
private:
//	GiNaC::ex fitFunction ;
	QwtPlotCurve *fitCurve ;
	QList<QPersistentModelIndex> dataSource ;
	QList<specCanvasItem*> dataPointers ;
	QQueue<QStack<int> > indexQueue ;
	specModel *model ;
	specPlot *dataPlot ;
	double tolerance ;
	QList<specKineticRange*> intervals ;
	
	void updateData() ;
	QMultiMap<double,QPair<double,double> > data ;
	QwtArrayData averageData() ;
	QPair<double,double> mergeData(QMap<double,QPair<double,double> >::iterator&) ;
	bool similar(double,double) ;
	void fetchIndices() ;
	QQueue<QStack<int> > fetchIntegerIndices() const ;
	void refreshDataPointers() ;
protected:
	QDataStream& readFromStream(QDataStream&) ;// TODO
	QDataStream& writeToStream(QDataStream&) const ; // TODO
public:
	specKinetic();
	~specKinetic();	
	
	void addItems(const QModelIndexList&) ;
	void setModel(specModel*) ;
	void setPlot(specPlot*) ;
	void showRanges(specPlot*) ;
	void hideRanges() ;
	void refreshPlotData() ;
	void conditionalUpdate(specCanvasItem*) ;
	void removeRange(specKineticRange*) ;
	void addRange(specKineticRange*) ;
	QIcon decoration() const ;
	void newRange(double min=INFINITY, double max=INFINITY) ;
	void exportData(const QList<QPair<bool,QString> >&, const QList<QPair<spec::value,QString> >&, QTextStream&) ;
	QModelIndexList connectedData() ;
};

#endif
