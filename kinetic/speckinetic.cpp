#include "speckinetic.h"
#include <QTextStream>
#include <QHash>
#include "specdescriptor.h"
#include <QFile>
#include <qwt_plot.h>
#include "speckineticrange.h"
#include <qwt_scale_div.h>
#include <qwt_series_data.h>

// TODO crash when merging data elements

specKinetic::specKinetic()
 : specModelItem(), tolerance(0)
{
}


specKinetic::~specKinetic()
{
}


void specKinetic::updateData()
{

}

void specKinetic::addItems(const QModelIndexList &list)
{
	foreach (QModelIndex index, list)
	{
		if(!index.isValid()) continue ;
		// TODO include case where no model was defined (move to queue ?)
		if(index.model() == model)
			dataSource << QPersistentModelIndex(index) ;
		// TODO revise
		else if(model->dropMimeData(index.model()->mimeData(QModelIndexList() << index),Qt::CopyAction,-1,0,QModelIndex()) )
			dataSource << QPersistentModelIndex(model->index(model->rowCount(QModelIndex())-1,0,QModelIndex())) ;
		qDebug("added item to kinetic") ;
	}
	refreshDataPointers() ;
	refreshPlotData() ;
	// TODO remove
// 	qDebug("adding range") ;
// 	addRange(new specKineticRange(1500.,1700.,this)) ;
// 	qDebug("showing range") ;
// 	showRanges() ;
}

void specKinetic::newRange(double min, double max)
{
	if (min == INFINITY || max == INFINITY)
	{
		min = dataPlot->axisScaleDiv(QwtPlot::xBottom)->lowerBound();
		max = dataPlot->axisScaleDiv(QwtPlot::xBottom)->upperBound();
		double delta = max-min ;
		min -= .1*delta ;
		max += .1*delta ;
	}
	intervals << new specKineticRange(min,max,this) ;
	if (plot()) showRanges((specPlot*) plot()) ;
	refreshPlotData() ;
}

QwtSeriesData<QPointF>* specKinetic::averageData()
{
	QVector<QPointF> kineticTrace  ;
	double minNu = INFINITY, maxNu = -INFINITY ;
	qDebug("size before comparison:  %d",data.size()) ;
	for(QMap<double,QPair<double,double> >::iterator i = data.begin() ; i != data.end() ; i++)
	{
		qDebug("comparing %f %f %f -- %f %f",minNu, maxNu, i.value().first,i.key(),i.value().second) ;
		minNu = qMin(minNu,i.value().first) ;
		maxNu = qMax(maxNu,i.value().first) ;
	}
	qDebug("Integrating from %f to %f.",minNu,maxNu) ;
	for(QMap<double,QPair<double,double> >::iterator i = data.begin() ; i != data.end() ; i++)
	{
		double t = i.key(), y = 0 ;
		qDebug("starting integration %f",t) ;
		int count = 0 ;
		QMap<double,QPair<double,double> >::iterator j ;
		for (j = i ; j != data.end() && similar(t, j.key()); j++)
			count ++ ;
		j-- ;
		if (count == 1)
			y = i.value().second*(maxNu-minNu) ;
		else if (count > 1)
		{
			double dx = i.value().first - minNu ;
			y += (i.value().second-((i+1).value().second - i.value().second)/((i+1).value().first-i.value().first)*dx/2.)*dx ;
			qDebug(" intermediate result:  %f",y);
			for (QMap<double,QPair<double,double> >::iterator k = i+1 ; k != j+1 ; k++)
			{
				y += (k.value().second+(k-1).value().second)*(k.value().first-(k-1).value().first)/2. ;
				qDebug(" intermediate result:  %f",y);
			}
			dx = maxNu - j.value().first ;
			y+= (j.value().second + (j.value().second-(j-1).value().second)/(j.value().first-(j-1).value().first)*dx/2.)*dx ;
		}
		i = j ;
		qDebug(" final result:  %f",y);
		kineticTrace << QPointF(t,y) ;
// 		QPair<double, double> newValue ;
// 		if( (i+1)!=data.end() && similar(i.key(), (i+1).key()) )
// 		{
// 			newValue = mergeData(i) ;
// 			i-- ;
// 		}
// 		else
// 			newValue = QPair<double,double>(i.key(),i.value().second) ;
// 		t << newValue.first ;
// 		y << newValue.second;
// // 		QTextStream cout(stdout,QIODevice::WriteOnly) ;
// // 		cout << "ty: " << newValue.first << "  " << newValue.second << endl ;
	}
	return new QwtPointSeriesData(kineticTrace) ;
}

QPair<double,double> specKinetic::mergeData(QMap<double,QPair<double, double> >::iterator& i)
{
	double t=0, y=0, comparison = i.key() ;
	int count=0 ;
	while(i != data.end() && similar(i.key(),comparison) )
	{
		t += i.key() ;
		y += i.value().second ;
		count++ ;
		i++ ;
	}
	return QPair<double,double>(t/count, y/count) ;
}

bool specKinetic::similar(double a,double b)
{ return b >= a-tolerance && b <= a+tolerance ;}

QDataStream& specKinetic::readFromStream(QDataStream& stream)
{
// TODO import ginac part
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "Reading kinetic item " << endl ;
	QHash<QString,specDescriptor>::size_type descriptionSize ;
	QList<specKineticRange*>::size_type intervalsSize ;
	stream >> tolerance >> descriptionSize >> intervalsSize;
	// Lese Filterdaten
	for (QHash<QString,specDescriptor>::size_type i=0 ; i < descriptionSize ; i++)
	{
// 		cout << "reading descriptor " << i << " of " << descriptionSize << endl ;
		QString key, value ;
		stream >> key ;
		stream >> value ;
		changeDescriptor(key,value) ;
	}
	for (QList<specKineticRange*>::size_type i = 0 ; i < intervalsSize ; i++)
	{
// 		cout << "Reagin range no. " << i << endl ;
		intervals << new specKineticRange(0,0,this) ;
		stream >> *(intervals.last()) ;
	}
	quint64 modelPointer ;
	stream >> modelPointer >> indexQueue ;
	model = (specModel*) modelPointer ;
	fetchIndices() ;
	refreshPlotData() ;
// 	cout << "done reading data item." << endl ;
	return stream;
}

QDataStream& specKinetic::writeToStream(QDataStream& stream) const
{// TODO export ginac part
// 	qDebug("writing kinetic item") ;
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "ID export:  " << (quint8) spec::kinetic << endl ;
	stream << (quint8) spec::kinetic << tolerance << descriptorKeys().size() << intervals.size() ;
	for (QHash<QString,specDescriptor>::size_type i = 0 ; i < descriptorKeys().size() ; i++)
		stream << (descriptorKeys()[i]) << (descriptor(descriptorKeys()[i])) ;
	for (QList<specKineticRange*>::size_type i = 0 ; i < intervals.size() ; i++)
		stream << *(intervals[i]) ;
	stream << (quint64)(qobject_cast<QFile*>(stream.device()) ? (specModel*) NULL : model) ;
	stream << fetchIntegerIndices() ;
	return stream ;
}

QQueue<QStack<int> > specKinetic::fetchIntegerIndices() const
{
	QQueue<QStack<int> > returnQueue ;
	foreach(QModelIndex index, dataSource)
	{
		QStack<int> integerIndices ;
		while (index.isValid())
		{
			integerIndices.push(index.row()) ;
			index = index.parent() ;
		}
		returnQueue.enqueue(integerIndices) ;
	}
	return returnQueue ;
}

void specKinetic::fetchIndices()
{
	if (!model) return ;
	while (!indexQueue.isEmpty())
	{
		QStack<int> current = indexQueue.dequeue() ;
		QModelIndex index = QModelIndex() ;
		while(!current.isEmpty())
			index = model->specModel::index(current.pop(),0,index) ;
		dataSource << QPersistentModelIndex(index) ;
	}
	refreshDataPointers() ;
	refreshPlotData() ;
}

void specKinetic::setPlot(specPlot* p)
{ dataPlot = p ; }

void specKinetic::setModel(specModel* m)
{
	if (m == model) return ;
	
	model = m ;
	fetchIndices() ;
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
// 	cout << "starting to fetch model indexes" << endl ;
	
	for (int i = 0 ; i < dataSource.size() ; i++)
		if (dataSource[i].model() != model &&
				  model->dropMimeData(dataSource[i].model()->mimeData(QModelIndexList() << dataSource[i]),Qt::CopyAction,-1,0,QModelIndex()) )
			dataSource.replace(i,QPersistentModelIndex(m->index(m->rowCount(QModelIndex())-1,0,QModelIndex()))) ;
	refreshDataPointers() ;
// 	cout << "done fetching" << endl ; 
}

void specKinetic::refreshPlotData()
{
// 	QTextStream cout(stdout,QIODevice::WriteOnly) ;
	data.clear() ;
	
	if(!dataSource.isEmpty() && !intervals.isEmpty())
	{
// 		qDebug("building data") ;
		for (int j = 0 ; j < dataSource.size() ; j++)
		{
			if (!dataSource[j].isValid())
			{
				dataSource.takeAt(j) ;
				continue ;
			} // TODO take pointer from dataPointers
			QMultiMap<double,QPair<double,double> > *tempData = ((specModelItem*) dataSource[j].internalPointer())->kinetics(intervals) ;
			qDebug("inserting into data array, size:  %d (+%d)",data.size(),tempData->size()) ;
// 			cout << "Data size:  " << tempData->size() << endl ;
			for(int i = 0 ; i < tempData->size(); i++)
			{
				data.insert(tempData->keys()[i],tempData->values()[i]) ;
				qDebug("inserting %f %f %f", tempData->keys()[i], tempData->values()[i].first, tempData->values()[i].second) ;
			}
			delete tempData ;
		}
// 		qDebug("setting data") ;
		setData(averageData()) ;
	}
	else
		setSamples(QVector<double>(),QVector<double>()) ;
// 	qDebug("replotting kinetics") ;
	if(plot()) plot()->replot() ;
// 	qDebug("done with kinetics") ;
}

void specKinetic::conditionalUpdate(specCanvasItem *pointer)
{
	if (dataPointers.contains(pointer)) refreshPlotData() ;
}

void specKinetic::refreshDataPointers()
{
	dataPointers.clear() ;
	foreach(QModelIndex index, dataSource)
		dataPointers << (specCanvasItem*) index.internalPointer() ;
}

void specKinetic::removeRange(specKineticRange* range)
{
	if(!intervals.contains(range)) return ;
	
	intervals.takeAt(intervals.indexOf(range)) ;
	refreshPlotData() ;
}

void specKinetic::addRange(specKineticRange* range)
{
	if (intervals.contains(range)) return ;
	
	intervals << range ;
	refreshPlotData() ;
}

void specKinetic::showRanges(specPlot* plot) // TODO maybe move this to view/model
{
	attach(plot) ;
	foreach(specKineticRange* pointer, intervals)
	{
		QPen tempPen = pointer->pen() ;
		QColor tempColor = pen().color() ;
		tempColor.setAlphaF(tempColor.alphaF()*.5);
		tempPen.setColor(tempColor) ;
		pointer->setPen(tempPen);
		pointer->attach(dataPlot) ;
	}
	dataPlot->replot() ;
}

void specKinetic::hideRanges()
{
	detach() ;
	foreach(specKineticRange* pointer, intervals)
		pointer->detach() ;
	dataPlot->replot() ;
}

QIcon specKinetic::decoration() const { return QIcon(":/kinetic.png") ; }

void specKinetic::exportData(const QList<QPair<bool,QString> >& headerFormat, const QList<QPair<spec::value,QString> >& dataFormat, QTextStream& out)
{
	for (int i = 0 ; i < headerFormat.size() ; i++)
		out << (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	out << endl ;
	for (int j = 0 ; j < dataSize() ; j++)
	{
		for (int i = 0 ; i < dataFormat.size() ; i++)
		{
			switch(dataFormat[i].first)
			{
				case spec::time: out << sample(j).x() ; break ;
				case spec::signal: out << sample(j).y() ; break ;
			}
			out << dataFormat[i].second ;
		}
	}
	out << endl ;
}

QModelIndexList specKinetic::connectedData()
{
	QModelIndexList connected ;
	foreach(QPersistentModelIndex index, dataSource)
		connected << QModelIndex(index) ;
	return connected ;
}
