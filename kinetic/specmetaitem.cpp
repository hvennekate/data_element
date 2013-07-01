#include "specmetaitem.h"
#include "specplot.h"
#include "metaitemproperties.h"
#include "specfitcurve.h"
#include "QApplication"

bool specMetaItem::fitCurveDescriptor(const QString &s) const
{
	return fitCurve && fitCurve->descriptorKeys().contains(s) ;
}

void specMetaItem::syncFitCurveName() const
{
	if (fitCurve)
		fitCurve->setTitle(QObject::tr("") + descriptor("") + QObject::tr(" (Fit)"));
}

specMetaItem::specMetaItem(specFolderItem *par, QString description)
	: specModelItem(par,description),
	  filter(0),
	  currentlyConnectingServer(0),
	  metaModel(0),
	  dataModel(0),
	  fitCurve(0),
	  styleFitCurve(false)
{
	filter = new specMetaParser("","","",this) ;
	invalidate() ;
	variables["x"] = specDescriptor("", spec::editable) ;
	variables["y"] = specDescriptor("", spec::editable) ;
	variables["variables"] = specDescriptor("", spec::editable) ;
	variables["errors"] = specDescriptor("No data") ;
}

specMetaItem::~specMetaItem()
{
	foreach(specModelItem* item, items)
		disconnectServer(item) ;
	delete filter ;
	delete fitCurve ;
}

void specMetaItem::connectedItems(QModelIndexList &dataItems, QModelIndexList &metaItems)
{
	if (!dataModel || !metaModel) return ;
	foreach(specModelItem* item, items)
	{
		QModelIndex index = dataModel->index(item) ;
		if (index.isValid())
			dataItems << index ;
		else
		{
			index = metaModel->index(item) ;
			if (index.isValid())
				metaItems << index ;
		}
	}
}

void specMetaItem::setModels(specModel *m, specModel *d)
{
	metaModel = m ;
	dataModel = d ;

	for (int i = 0 ; i < oldConnections.size() ; ++i)
	{
		oldConnections[i].first.setModel(oldConnections[i].second ? m : d) ;
		QVector<specModelItem*> oldItems = oldConnections[i].first.items() ;
		foreach(specModelItem* item, oldItems)
			connectServer(item) ;
	}
	oldConnections.clear();
	invalidate();
}

void specMetaItem::writeToStream(QDataStream &out) const
{
	bool stylingWasToFit = styleFitCurve ;
	const_cast<bool&>(styleFitCurve) = false ; // TODO this is ugly
	specModelItem::writeToStream(out) ;
	const_cast<bool&>(styleFitCurve) = stylingWasToFit ;
	out << ((quint8) styleFitCurve) << variables ;
	if (!metaModel || !dataModel) return ;
	QVector<QPair<specGenealogy,qint8> > currentConnections ;
	QModelIndexList indexes ;
	foreach(specModelItem* item, items)
		indexes << (metaModel->contains(item) ? metaModel->index(item) : dataModel->index(item)) ;

	while (!indexes.isEmpty())
		currentConnections << qMakePair<specGenealogy,qint8>(specGenealogy(indexes), indexes.first().model() == metaModel) ;
	out << currentConnections ;
	out << (quint8 ((bool) fitCurve)) ;
	if (fitCurve) out << *fitCurve ;
}

void specMetaItem::readFromStream(QDataStream &in)
{
	delete fitCurve ;
	fitCurve = 0 ;
	styleFitCurve = false ;
	specModelItem::readFromStream(in) ;
	quint8 stylingWasToFit ;
	in >> stylingWasToFit >> variables >> oldConnections;
	/*stylingWasToFit introduced on Oct 12 2012 (Commit:  Fixed meta item peculiarity)
    Merged into office branch (muParser) on Nov 13 2012 (Commit: Merge branch 'innovations' of /fs/hvennek/dataelement-src into muParser)

    Old files (before Nov 13 2012) do not contain StylingWasToFit!
    Fix:  remove ">> stylingWasToFit" in specmetaitem.cpp (line 96) and set stylingWasToFit = 0 (line 95)
	  read and save old files. */
	styleFitCurve = stylingWasToFit ;
	filter->setAssignments(variables["variables"].content(true), variables["x"].content(true), variables["y"].content(true)) ;
	quint8 hasFitCurve ;
	in >> hasFitCurve ; // redundant
	if (hasFitCurve)
	{
		fitCurve = new specFitCurve ;
		in >> *fitCurve ;
	}
	invalidate() ; // TODO maybe insert in data item or just model item.
	syncFitCurveName();
}

QList<specModelItem*> specMetaItem::purgeConnections()
{
	QList<specModelItem*> list = items ;
	foreach(specModelItem* item, items)
		item->disconnectClient(this) ;
	items.clear();
	invalidate() ;
	return list ;
}

bool specMetaItem::disconnectServer(specModelItem *server) // TODO template ? -> short circ?
{
	if (server == currentlyConnectingServer)
		return true ;
	if (!items.contains(server))
		return false ;
	currentlyConnectingServer = server ;
	if (!server->disconnectClient(this))
	{
		currentlyConnectingServer = 0 ;
		return false ;
	}
	currentlyConnectingServer = 0 ;
	items.removeOne(server) ;
	invalidate() ;
	return true ;
}

bool specMetaItem::connectServer(specModelItem *server)
{
	if (currentlyConnectingServer == server)
		return true ;
	if (items.contains(server))
		return false ;
	if (shortCircuit(server))
		return false ;
	currentlyConnectingServer = server ;
	if (!server->connectClient(this))
	{
		currentlyConnectingServer = 0 ;
		return false ;
	}
	currentlyConnectingServer = 0 ;
	items << server ;
	invalidate() ;
	return true ;
}

void specMetaItem::refreshOtherPlots()
{
	QSet<specPlot *> otherPlots ;
	foreach(specModelItem* item, items)
		otherPlots << ((specPlot*) item->plot()) ;
	otherPlots.remove(0) ;
	QColor rangeColor = pen().color() ;
	rangeColor.setAlpha(128);
	if (plot())
		filter->attachRanges(otherPlots, rangeColor)  ;
	else
		filter->detachRanges();
	foreach(QwtPlot *otherPlot, otherPlots)
		otherPlot->replot(); // TODO leads to crashes
}

void specMetaItem::attach(QwtPlot *plot)
{
	specModelItem::attach(plot) ;
	if (fitCurve) fitCurve->attach(plot) ;
	refreshOtherPlots() ;
}

void specMetaItem::detach()
{
	attach(0) ;
	specModelItem::detach() ; // TODO find out why this is necessary
}

void specMetaItem::refreshPlotData()
{
	// TODO do some more checks on valid items etc.
	foreach(specModelItem *item, items)
		item->revalidate();
	setData(processData(filter->evaluate(items.toVector()))) ; // TODO use vector
	variables["errors"] = filter->warnings() ;
	refreshOtherPlots() ;
}

QStringList specMetaItem::descriptorKeys() const
{
	QStringList keys = specModelItem::descriptorKeys() << "variables" << "x" << "y" << "errors" ; // TODO change to get from variables
	if (fitCurve) keys << fitCurve->descriptorKeys() ;
	return keys ;
}

QString specMetaItem::descriptor(const QString &key, bool full) const
{
	if (key == "") return specModelItem::descriptor(key,full) ;
	if (fitCurveDescriptor(key))
		return fitCurve->descriptor(key,full) ;
	return variables[key].content(full) ;
}

QString specMetaItem::editDescriptor(const QString &key) const
{
	if(fitCurveDescriptor(key)) return fitCurve->editDescriptor(key) ;
	return specModelItem::editDescriptor(key) ;
}

bool specMetaItem::changeDescriptor(QString key, QString value)
{
	if (key == "")
	{
		bool result = specModelItem::changeDescriptor(key,value) ;
		syncFitCurveName();
		return result ;
	}
	if (fitCurveDescriptor(key))
		return fitCurve->changeDescriptor(key,value) ;
	if (!variables.contains(key)) return false ;
	variables[key] = value ;
	filter->setAssignments(variables["variables"].content(true), variables["x"].content(true), variables["y"].content(true)) ; // TODO put in function (see above)
	invalidate();
	return true ;
}

spec::descriptorFlags specMetaItem::descriptorProperties(const QString &key) const
{
	if (key == "") return specModelItem::descriptorProperties(key) ;
	if (variables.contains(key))
		return variables[key].flags() ;
	if (fitCurveDescriptor(key)) return fitCurve->descriptorProperties(key) ; // TODO dangerous
	return spec::def ;
}

void specMetaItem::setDescriptorProperties(const QString &key, spec::descriptorFlags f)
{
	if (key == "") specModelItem::setDescriptorProperties(key, f) ;
	if (variables.contains(key)) variables[key].setFlags(f) ;
	if (fitCurveDescriptor(key) && key != "") fitCurve->setDescriptorProperties(key, f) ;
}

QIcon specMetaItem::decoration() const
{
	if (!variables["errors"].content(true).isEmpty())
		return QIcon::fromTheme("dialog-warning") ;

	return QIcon(":/kinetic.png") ;
}

void specMetaItem::getRangePoint(int variable, int range, int point, double &x, double &y) const
{
	filter->getRangePoint(variable, range,point,x,y) ;
}

void specMetaItem::setRange(int variableNo, int rangeNo, int pointNo, double newX, double newY)
{
	filter->setRange(variableNo, rangeNo, pointNo, newX, newY) ;
}

bool specMetaItem::setActiveLine(const QString &s, int i)
{
	if (fitCurveDescriptor(s))
		return fitCurve->setActiveLine(s,i) ;
	if (variables.contains(s))
	{
		variables[s].setActiveLine(i) ;
		return true ;
	}
	bool result = specModelItem::setActiveLine(s,i) ;
	syncFitCurveName();
	return result ;
}

int specMetaItem::activeLine(const QString &key) const
{
	if (fitCurveDescriptor(key)) return fitCurve->activeLine(key) ;
	if (variables.contains(key)) return variables[key].activeLine() ;
	return specModelItem::activeLine(key) ;
}

specUndoCommand* specMetaItem::itemPropertiesAction(QObject *parentObject)
{
	metaItemProperties propertiesDialog(this) ;
	propertiesDialog.exec() ;
	if (propertiesDialog.result() != QDialog::Accepted) return 0 ;
	return propertiesDialog.changedConnections(parentObject) ;
}

specFitCurve* specMetaItem::setFitCurve(specFitCurve *fc)
{
	specFitCurve *old = fitCurve ;
	fitCurve = fc ;
	if (fitCurve && plot()) fitCurve->attach(plot()) ;
	syncFitCurveName();
	return old ;
}

specFitCurve* specMetaItem::getFitCurve() const
{
	return fitCurve ;
}

void specMetaItem::conductFit()
{
	if (!fitCurve) return ;
	revalidate() ;
	fitCurve->refit(data()) ;
}

bool specMetaItem::getFitStyleState() const
{
	return styleFitCurve ;
}

void specMetaItem::toggleFitStyle()
{
	styleFitCurve = !styleFitCurve ;
}

#define STYLEROUTINGFUNCTION(TYPE,GETNAME,SETNAME) \
	void specMetaItem::SETNAME(const TYPE& arg) { \
	if (styleFitCurve && fitCurve) fitCurve->SETNAME(arg) ; \
	else specModelItem::SETNAME(arg) ; } \
	\
	TYPE specMetaItem::GETNAME() const { \
	if (styleFitCurve && fitCurve) return fitCurve->GETNAME() ; \
	else return specModelItem::GETNAME() ; }

STYLEROUTINGFUNCTION(double, lineWidth,   setLineWidth)
STYLEROUTINGFUNCTION(int,    symbolStyle, setSymbolStyle)
STYLEROUTINGFUNCTION(QColor, symbolPenColor, setSymbolPenColor)
STYLEROUTINGFUNCTION(QColor, symbolBrushColor, setSymbolBrushColor)
STYLEROUTINGFUNCTION(QSize, symbolSize, setSymbolSize)
STYLEROUTINGFUNCTION(qint8, penStyle, setPenStyle)

void specMetaItem::setPenColor(const QColor& arg)
{
	if (styleFitCurve && fitCurve) fitCurve->setPenColor(arg) ;
	else
	{
		specModelItem::setPenColor(arg) ;
		refreshOtherPlots();
	}
}

QColor specMetaItem::penColor() const {
	if (styleFitCurve && fitCurve) return fitCurve->penColor() ;
	else return specModelItem::penColor() ; }

void specMetaItem::setSymbolSize(int w, int h)
{
	if (styleFitCurve && fitCurve)
		fitCurve->setSymbolSize(w,h) ;
	else
		specCanvasItem::setSymbolSize(w,h) ;
}

void specMetaItem::exportData(const QList<QPair<bool,QString> >& headerFormat, const QList<QPair<spec::value,QString> >& dataFormat, QTextStream& out) // TODO split into two
{
	revalidate();

	for (int i = 0 ; i < headerFormat.size() ; i++)
		out << (headerFormat[i].first ? headerFormat[i].second : this->descriptor(headerFormat[i].second)) ;
	out << endl ;
	QVector<double> fitValues ;
	if (fitCurve) fitValues = fitCurve->getFitData(data()) ;
	for (size_t j = 0 ; j < dataSize() ; j++)
	{
		for (int i = 0 ; i < dataFormat.size() ; i++)
		{
			switch(dataFormat[i].first)
			{
			case spec::wavenumber: out << sample(j).x() ; break ;
			case spec::signal: out << sample(j).y() ; break ;
			case spec::maxInt:
				if (j < (size_t) fitValues.size())
					out <<  fitValues[j] ; break ;
			}
			out << dataFormat[i].second ;
		}
	}
	out << endl ;
}
