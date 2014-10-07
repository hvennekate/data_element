#include "specmetaitem.h"
#include "specplot.h"
#include "metaitemproperties.h"
#include "specfitcurve.h"
#include "QApplication"

bool specMetaItem::fitCurveDescriptor(const QString& s) const
{
	return fitCurve && fitCurve->descriptorKeys().contains(s) ;
}

void specMetaItem::syncFitCurveName() const
{
	if(fitCurve)
		fitCurve->setTitle(QObject::tr("") + descriptor("") + QObject::tr(" (Fit)"));
}

specMetaItem::specMetaItem(specFolderItem* par, QString description)
	: specModelItem(par, description),
	  filter(0),
	  currentlyConnectingServer(0),
	  metaModel(0),
	  dataModel(0),
	  fitCurve(0),
	  styleFitCurve(false)
{
	filter = new specMetaParser("", "", "", this) ;
	invalidate() ;
	variables["x"] = specDescriptor("") ;
	variables["y"] = specDescriptor("") ;
	variables["variables"] = specDescriptor("") ;
	variables["errors"] = specDescriptor("No data") ;
}

specMetaItem::~specMetaItem()
{
	foreach(specModelItem * item, items)
	disconnectServer(item) ;
	delete filter ;
	delete fitCurve ;
}

void specMetaItem::connectedItems(QModelIndexList& dataItems, QModelIndexList& metaItems)
{
	if(!dataModel || !metaModel) return ;
	foreach(specModelItem * item, items)
	{
		QModelIndex index = dataModel->index(item) ;
		if(index.isValid())
			dataItems << index ;
		else
		{
			index = metaModel->index(item) ;
			if(index.isValid())
				metaItems << index ;
		}
	}
}

QSet<QString> specMetaItem::variableNames() const
{
	if (filter) return filter->variableNames() ;
	return QSet<QString>() ;
}

QSet<QString> specMetaItem::variablesInFormulae() const
{
	if (filter) return filter->variablesInFormulae() ;
	return QSet<QString>() ;
}

void specMetaItem::setModels(specModel* m, specModel* d)
{
	metaModel = m ;
	dataModel = d ;

	for(int i = 0 ; i < oldConnections.size() ; ++i)
	{
		oldConnections[i].first.setModel(oldConnections[i].second ? m : d) ;
		QVector<specModelItem*> oldItems = oldConnections[i].first.items() ;
		foreach(specModelItem * item, oldItems)
		connectServer(item) ;
	}
	oldConnections.clear();
	invalidate();
}

void specMetaItem::writeToStream(QDataStream& out) const
{
	bool stylingWasToFit = styleFitCurve ;
	const_cast<bool&>(styleFitCurve) = false ;  // TODO this is ugly
	specModelItem::writeToStream(out) ;
	const_cast<bool&>(styleFitCurve) = stylingWasToFit ;
	out << ((quint8) styleFitCurve) << variables ;
	if(!metaModel || !dataModel) return ;
	QVector<QPair<specGenealogy, qint8> > currentConnections ;

	// Make chunks of items in order to exploit the genealogy's compression mechanism
	int i = 0 ;
	while(i < items.size())
	{
		int j = 1 ;
		bool isMeta = metaModel->contains(items[i]) ;
		while(i + j < items.size() &&
			metaModel->contains(items[i + j]) == isMeta)
			++j ;
		QList<specModelItem*> section = items.mid(i, j) ;
		i += j ;
		while(!section.isEmpty())
			currentConnections << qMakePair<specGenealogy, qint8> (
					       specGenealogy(section, isMeta ? metaModel : dataModel),
					       isMeta) ;
	}


	////// alternative formulation (untested!)
	//	foreach(specModelItem* item, items)
	//	{
	//		bool isMeta = metaModel->contains(item) ;
	//		currentConnections << qMakePair<specGenealogy, qint8>(
	//					specGenealogy(item, isMeta ? metaModel : dataModel),
	//					isMeta) ;
	//	}
	out << currentConnections ;
	out << (quint8((bool) fitCurve)) ;
	if(fitCurve) out << *fitCurve ;
}

void specMetaItem::readFromStream(QDataStream& in)
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
	if(hasFitCurve)
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
	foreach(specModelItem * item, items)
	item->disconnectClient(this) ;
	items.clear();
	invalidate() ;
	return list ;
}

bool specMetaItem::disconnectServer(specModelItem* server)  // TODO template ? -> short circ?
{
	if(server == currentlyConnectingServer)
		return true ;
	if(!items.contains(server))
		return false ;
	currentlyConnectingServer = server ;
	if(!server->disconnectClient(this))
	{
		currentlyConnectingServer = 0 ;
		return false ;
	}
	currentlyConnectingServer = 0 ;
	items.removeOne(server) ;
	invalidate() ;
	return true ;
}

bool specMetaItem::connectServer(specModelItem* server)
{
	if(currentlyConnectingServer == server)
		return true ;
	if(items.contains(server))
		return false ;
	if(shortCircuit(server))
		return false ;
	currentlyConnectingServer = server ;
	if(!server->connectClient(this))
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
	QSet<specPlot*> otherPlots ;
	foreach(specModelItem * item, items)
		otherPlots << ((specPlot*) item->plot()) ;
	otherPlots.remove(0) ;
	QColor rangeColor = pen().color() ;
	rangeColor.setAlpha(128);
	if(plot())
		filter->attachRanges(otherPlots, rangeColor)  ;
	else
		filter->detachRanges();
	foreach(QwtPlot * otherPlot, otherPlots)
		otherPlot->replot(); // TODO leads to crashes
}

void specMetaItem::attach(QwtPlot* plot)
{
	specModelItem::attach(plot) ;
	if(fitCurve) fitCurve->attach(plot) ;
	refreshOtherPlots() ;
}

void specMetaItem::detach()
{
	if(fitCurve) fitCurve->detach();
	specModelItem::detach() ;
	refreshOtherPlots();
}

void specMetaItem::refreshPlotData()
{
	// TODO do some more checks on valid items etc.
	foreach(specModelItem * item, items)
	item->revalidate();
	setData(filter->evaluate(items.toVector())) ;   // TODO use vector
	variables["errors"] = filter->warnings() ;
	refreshOtherPlots() ;
}

QStringList specMetaItem::descriptorKeys() const
{
	QStringList keys = genericDescriptorKeys() ;
	if(fitCurve) keys << fitCurve->descriptorKeys() ;
	return keys ;
}

QStringList specMetaItem::genericDescriptorKeys()
{
	QStringList keys ;
	keys << "" << "variables" << "x" << "y" << "errors" ; // TODO change to get from variables
	return keys ;
}

specDescriptor specMetaItem::getDescriptor(const QString& key) const
{
	if(key == "") return specModelItem::getDescriptor(key) ;
	if(fitCurveDescriptor(key))
		return fitCurve->getDescriptor(key) ;
	return variables[key] ;
}

QString specMetaItem::editDescriptor(const QString& key) const
{
	if(fitCurveDescriptor(key)) return fitCurve->editDescriptor(key) ;
	return specModelItem::editDescriptor(key) ;
}

void specMetaItem::changeDescriptor(const QString& key, QString value)
{
	if(key == "")
	{
		specModelItem::changeDescriptor(key, value) ;
		syncFitCurveName();
		return ;
	}
	if(fitCurveDescriptor(key))
	{
		fitCurve->changeDescriptor(key, value) ;
		return ;
	}
	if(!variables.contains(key)) return ;
	variables[key] = value ;
	filter->setAssignments(variables["variables"].content(true), variables["x"].content(true), variables["y"].content(true)) ;     // TODO put in function (see above)
	invalidate();
	return ;
}

QIcon specMetaItem::decoration() const
{
	if(!variables["errors"].content(true).isEmpty())
		return QIcon::fromTheme("dialog-warning") ;

	return QIcon(":/kinetic.png") ;
}

void specMetaItem::getRangePoint(int variable, int range, int point, double& x, double& y) const
{
	filter->getRangePoint(variable, range, point, x, y) ;
}

void specMetaItem::setRange(int variableNo, int rangeNo, int pointNo, double newX, double newY)
{
	filter->setRange(variableNo, rangeNo, pointNo, newX, newY) ;
}

void specMetaItem::setActiveLine(const QString& s, quint32 i)
{
	if(fitCurveDescriptor(s)) return fitCurve->setActiveLine(s, i) ;
	else if(variables.contains(s)) variables[s].setActiveLine(i) ;
	else specModelItem::setActiveLine(s, i) ;
	syncFitCurveName();
}

void specMetaItem::setMultiline(const QString &key, bool on)
{
	if (fitCurveDescriptor(key)) fitCurve->setMultiline(key, on) ;
	else if (variables.contains(key)) variables[key].setMultiline(on) ;
	else specModelItem::setMultiline(key, on) ;
}

specUndoCommand* specMetaItem::itemPropertiesAction(QObject* parentObject)
{
	metaItemProperties propertiesDialog(this) ;
	propertiesDialog.exec() ;
	if(propertiesDialog.result() != QDialog::Accepted) return 0 ;
	return propertiesDialog.changedConnections(parentObject) ;
}

specFitCurve* specMetaItem::setFitCurve(specFitCurve* fc)
{
	specFitCurve* old = fitCurve ;
	fitCurve = fc ;
	if(fitCurve && plot()) fitCurve->attach(plot()) ;
	syncFitCurveName();
	return old ;
}

specFitCurve* specMetaItem::getFitCurve() const
{
	return fitCurve ;
}

QStringList specMetaItem::fitVariableNames() const
{
	if (!fitCurve) return QStringList() ;
	return fitCurve->variableNames() ;
}

QStringList specMetaItem::fitFormulaVariableNames() const
{
	if (!fitCurve) return QStringList() ;
	return fitCurve->formulaVariableNames() ;
}

bool specMetaItem::hasFitCurve() const
{
	return fitCurve ;
}

void specMetaItem::conductFit()
{
	if(!fitCurve) return ;
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
	if(styleFitCurve && fitCurve) fitCurve->setPenColor(arg) ;
	else
	{
		specModelItem::setPenColor(arg) ;
		refreshOtherPlots();
	}
}

QColor specMetaItem::penColor() const
{
	if(styleFitCurve && fitCurve) return fitCurve->penColor() ;
	else return specModelItem::penColor() ;
}

void specMetaItem::setSymbolSize(int w, int h)
{
	if(styleFitCurve && fitCurve)
		fitCurve->setSymbolSize(w, h) ;
	else
		specCanvasItem::setSymbolSize(w, h) ;
}

QString specMetaItem::exportZ(int index) const
{
	if (fitCurve)
		return QString::number(fitCurve->getFitPoint(sample(index).x())) ;
	return QString() ;
}
