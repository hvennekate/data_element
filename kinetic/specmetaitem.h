#ifndef SPECMETAITEM_H
#define SPECMETAITEM_H

#include "specmodelitem.h"
#include "specmetaparser.h"
#include "model/specdescriptor.h"
#include "specgenealogy.h"

class specPlot ;
class specFitCurve ;

class specMetaItem : public specModelItem
{
	friend class metaItemProperties ;
private:
	QList<specModelItem*> items ;
	specMetaParser* filter ;
	QHash<QString, specDescriptor> variables ;
	specModelItem* currentlyConnectingServer ;
	type typeId() const { return specStreamable::metaItem ; }
	void readFromStream(QDataStream& in) ;
	void writeToStream(QDataStream& out) const ;
	specModel* metaModel, *dataModel ;
	QVector<QPair<specGenealogy, qint8> > oldConnections ;
	specFitCurve* fitCurve ;
	bool styleFitCurve ;
	bool fitCurveDescriptor(const QString&) const ;
	void syncFitCurveName() const ;
	QString exportZ(int index) const ;
public:
	void refreshOtherPlots() ;
	void setModels(specModel* meta, specModel* data) ;
	bool disconnectServer(specModelItem*) ;
	bool connectServer(specModelItem*) ;
	QList<specModelItem*> serverList() const { return items ; }
	bool isServedBy(specModelItem* item) const { return items.contains(item) ;}
	explicit specMetaItem(specFolderItem* par = 0, QString description = "");
	~specMetaItem() ;
	QList<specModelItem*> purgeConnections() ;
	void attach(QwtPlot* plot) ;
	void detach();
	void refreshPlotData();
	QStringList descriptorKeys() const ;
	static QStringList genericDescriptorKeys() ;
	specDescriptor getDescriptor(const QString &key) const ;
	QString editDescriptor(const QString& key) const ;
	void changeDescriptor(const QString &key, QString value) ;  // TODO add changeDescriptor(key,specDescriptor)
	QIcon decoration() const ;
	void getRangePoint(int variable, int range, int point, double& x, double& y) const ;
	void setRange(int variableNo, int rangeNo, int pointNo, double newX, double newY) ;
	void setActiveLine(const QString&, quint32) ;
	void setMultiline(const QString& key, bool on = true) ;
	int rtti() { return spec::metaItem ; }
	specUndoCommand* itemPropertiesAction(QObject* parentObject) ;

	// for fitting:
	specFitCurve* setFitCurve(specFitCurve*) ;
	specFitCurve* getFitCurve() const ;
	QStringList fitVariableNames() const ;
	QStringList fitFormulaVariableNames() const ;
	bool hasFitCurve() const ;
	void conductFit() ;
	void toggleFitStyle() ;
	bool getFitStyleState() const ;

	void setLineWidth(const double&) ;
	double lineWidth() const;
	QColor penColor() const;
	void setPenColor(const QColor&) ;
	int symbolStyle() const ;
	void setSymbolStyle(const int&) ;
	QColor symbolPenColor() const ;
	void setSymbolPenColor(const QColor&) ;
	void setSymbolBrushColor(const QColor&) ;
	QColor symbolBrushColor() const ;
	QSize symbolSize() const;
	void setSymbolSize(int w, int h = -1) ;
	void setSymbolSize(const QSize&) ;
	void setPenStyle(const qint8&);
	qint8 penStyle() const ;
	void connectedItems(QModelIndexList& dataItems, QModelIndexList& metaItems) ;
	QSet<QString> variableNames() const ;
	QSet<QString> variablesInFormulae() const ;
};

/* TODO in other classes

  specModelItem:
	disconnect from server if detachChildren() called
	.install invalidate functionality
	.install client functionality
	attach needs to revalidate data before connecting to plot
	make use of invalidate in refreshPlotData etc.
	make use of client in destructor etc.

*/
#endif // SPECMETAITEM_H
