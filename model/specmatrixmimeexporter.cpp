#include "specmatrixmimeexporter.h"
#include <QMimeData>
#include "specmodelitem.h"

specMatrixMimeExporter::specMatrixMimeExporter(QObject *parent) :
	specMimeConverter(parent),
	XAcross(true),
	SortY(false)
{
}

void specMatrixMimeExporter::setXAcross(bool across)
{
	XAcross = across ;
}

void specMatrixMimeExporter::setSeparator(const QString &s)
{
	Separator = s ;
}

QList<specModelItem*> specMatrixMimeExporter::importData(const QMimeData* data) // TODO eventuell in parent-Klasse
{
	Q_UNUSED(data) ;
	return QList<specModelItem*>() ;
}

void specMatrixMimeExporter::setDescriptor(const QString &d)
{
	Descriptor = d ;
}

void specMatrixMimeExporter::setSortY(bool sortY)
{
	SortY = sortY ;
}

void specMatrixMimeExporter::exportData(QList<specModelItem *> &items, QMimeData *data)
{
	data->setText(matrix(items)) ;
}

typedef QPair<double, QMap<double, double> > itemDataType ;
bool smallerItemDataType(const itemDataType& a, const itemDataType& b)
{
	return a.first < b.first ;
}

QString specMatrixMimeExporter::matrix(QList<specModelItem *> & items)
{
	QList<itemDataType> itemData ;
	foreach(specModelItem* item, items)
		itemData << qMakePair(item->descriptorValue(Descriptor),
				      item->dataMap()) ;

	if (SortY) qSort(itemData.begin(), itemData.end(), smallerItemDataType) ;

	QSet<double> xValueSet ;
	foreach(const itemDataType& item, itemData)
		foreach(const double &x, item.second.keys())
			xValueSet << x ;
	QList<double> xValues = xValueSet.toList() ;
	qSort(xValues) ;

	double *matrix = new double[itemData.size() * xValues.size()] ;
	for (int j = 0 ; j < itemData.size() ; ++j)
		for (int i = 0 ; i < xValues.size() ; ++i)
			matrix[j*xValues.size()+i] =
					itemData[j].second.contains(xValues[i]) ?
						itemData[j].second[xValues[i]] : NAN ;

	QStringList result ;
	QStringList line ;
	if (XAcross)
		foreach(double x, xValues)
			line << QString::number(x) ;
	else
		foreach(const itemDataType& item, itemData)
			line << QString::number(item.first) ;
	result << Separator + line.join(Separator) ;

	int outerLimit = itemData.size(),
			innerLimit = xValues.size() ;
	int outerStride = xValues.size(),
			innerStride = 1 ;
	if (!XAcross)
	{
		qSwap(outerLimit, innerLimit) ;
		qSwap(outerStride, innerStride) ;
	}

	for (int i = 0 ; i < outerLimit ; ++i)
	{
		QStringList innerLine ;
		innerLine << QString::number(XAcross ? itemData[i].first : xValues[i]) ;
		for (int j = 0 ; j < innerLimit ; ++j)
			innerLine << QString::number(matrix[i*outerStride + j*innerStride]) ;
		result << innerLine.join(Separator) ;
	}
	return result.join("\n") ;
}
