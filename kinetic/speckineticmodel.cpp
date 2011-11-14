#include "speckineticmodel.h"
#include "speckinetic.h"

specKineticModel::specKineticModel ( QObject *parent )
		: specModel ( parent ), connectedModel(NULL)
{
}


specKineticModel::~specKineticModel()
{
}

bool specKineticModel::insertItems ( QList< specModelItem * > list, QModelIndex parent, int row )
{
	specKinetic* kinetic ;
	foreach(specModelItem* pointer, list)
	{
		if (kinetic = dynamic_cast<specKinetic*>(pointer))
		{
			kinetic->setModel(connectedModel) ;
			kinetic->setPlot(connectedPlot) ;
		}
	}
	return specModel::insertItems ( list, parent, row );
}

#include <QTextStream>
void printAncestry(QModelIndex index)
{
	QTextStream cout(stdout,QIODevice::WriteOnly) ;
	while(index.isValid())
	{
		cout << index.row() << "  " ;
		index = index.parent() ;
	}
	cout << endl ;
}

void specKineticModel::connectToModel(specModel* model)
{
	qDebug("connecting to kinetic model to main model") ;
	connectedModel = model ;
	qDebug() << "checking row count" << itemPointer(QModelIndex()) ;
	qDebug() << "number of children:" << itemPointer(QModelIndex())->children() ;
	if (!rowCount(QModelIndex())) return ;
	qDebug("doing connection to kinetic model") ;
	QModelIndex index = specModel::index(0,0,QModelIndex()) ;
	while (index != QModelIndex())
	{
// 		printAncestry(index) ;
		if (rowCount(index)) // TODO create function for both these connections
		{
// 			cout << "index is folder, switching to first child" << endl ;
			index = specModel::index(0,0,index) ;
		}
		else
		{
			specKinetic* pointer ;
			if (pointer = dynamic_cast<specKinetic*>((specModelItem*) index.internalPointer()))
			{
// 				cout << "setting model for index" << endl ;
				pointer->setModel(connectedModel) ;
			}
			while(index.isValid() && rowCount(parent(index)) -1 == index.row()) // is last of parent's children?
			{
// 				cout << "moving up" << endl ;
				index = parent(index) ; // move up
			}
// 			cout << "switching to next index " << index.row() << endl ;
			if (index.isValid())
				index = specModel::index(index.row()+1,0,parent(index)) ;
		}
	}
}

void specKineticModel::connectToPlot(specPlot* plot)
{
	connectedPlot = plot ;
	if (!rowCount(QModelIndex())) return ;
	QModelIndex index = specModel::index(0,0,QModelIndex()) ;
	while (index.isValid())
	{
		if (rowCount(index))
			index = specModel::index(0,0,index) ;
		else
		{
			specKinetic* pointer ;
			if (pointer = dynamic_cast<specKinetic*>((specModelItem*) index.internalPointer()))
				pointer->setPlot(connectedPlot) ;
			while(index.isValid() && rowCount(parent(index)) -1 == index.row()) // is last of parent's children?
				index = parent(index) ; // move up
			if (index.isValid())
				index = specModel::index(index.row()+1,0,parent(index)) ;
		}
	}
}

void specKineticModel::conditionalUpdate(specCanvasItem* itemPointer)
{
	if (!rowCount(QModelIndex())) return ;
	QModelIndex index = specModel::index(0,0,QModelIndex()) ;
	while (index != QModelIndex())
	{
// 		printAncestry(index) ;
		if (rowCount(index)) // TODO create function for both these connections
		{
// 			cout << "index is folder, switching to first child" << endl ;
			index = specModel::index(0,0,index) ;
		}
		else
		{
			specKinetic* pointer ;
			if (pointer = dynamic_cast<specKinetic*>((specModelItem*) index.internalPointer()))
			{
// 				cout << "setting model for index" << endl ;
				pointer->conditionalUpdate(itemPointer) ;
			}
			while(index.isValid() && rowCount(parent(index)) -1 == index.row()) // is last of parent's children?
			{
// 				cout << "moving up" << endl ;
				index = parent(index) ; // move up
			}
// 			cout << "switching to next index " << index.row() << endl ;
			if (index.isValid())
				index = specModel::index(index.row()+1,0,parent(index)) ;
		}
	}
}
