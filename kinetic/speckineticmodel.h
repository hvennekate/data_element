#ifndef SPECKINETICMODEL_H
#define SPECKINETICMODEL_H

#include <specmodel.h>

class specPlot ;

class specKineticModel : public specModel
{
	Q_OBJECT
private:
	specModel* connectedModel ;
	specPlot* connectedPlot;
public:
	specKineticModel ( QObject *parent = 0 );
	~specKineticModel();

	bool insertItems ( QList< specModelItem * > list, QModelIndex parent, int row );
	void connectToModel(specModel*) ;
	void connectToPlot(specPlot*) ; // TODO maybe move to view/widget
public slots:
	void conditionalUpdate(specCanvasItem*) ;

};

#endif
