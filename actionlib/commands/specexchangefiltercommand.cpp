#include "specexchangefiltercommand.h"
#include "specmulticommand.h"
#include "specdataitem.h"

specExchangeFilterCommand::specExchangeFilterCommand(specUndoCommand* parent, bool rel)
	: specSingleItemCommand<specModelItem> (parent),
	  relative(rel)
{}

QVector<specDataPointFilter> specExchangeFilterCommand::collectFilters(const QVector<specDataItem*>& items) const
{
	QVector<specDataPointFilter> filters ;
	foreach(specDataItem * item, items)
	filters << item->dataFilter() ;
	return filters ;
}

void specExchangeFilterCommand::applyAbsoluteFilters(const QVector<specDataItem*>& items) const
{
	for(int i = 0 ; i < qMin(absoluteFilters.size(), items.size()) ; ++i)
		items[i]->setDataFilter(absoluteFilters[i]) ;
}

void specExchangeFilterCommand::doIt()
{
	specModelItem* item = itemPointer() ;
	if(!item) return ;
	QVector<specDataItem*> items = item->findDescendants<specDataItem*>() ;
	QVector<specDataPointFilter> oldFilters = collectFilters(items) ;
	if(relative)
		item->addDataFilter(relativeFilter) ;
	else
		applyAbsoluteFilters(items);
	absoluteFilters.swap(oldFilters) ;
	relative = false ;
	generateDescription();
}

void specExchangeFilterCommand::undoIt()
{
	if(relative)
		relativeFilter = -relativeFilter ;
	doIt() ;
}

void specExchangeFilterCommand::writeCommand(QDataStream& out) const
{
	if(relative)
		out << relativeFilter.getSlope()
		    << relativeFilter.getOffset()
		    << relativeFilter.getFactor()
		    << relativeFilter.getXShift() ;
	else
		out << absoluteFilters ;
	writeItem(out) ;
}

void specExchangeFilterCommand::readCommand(QDataStream& in)
{
	if(relative)
	{
		double slope = 0, offset = 0, scale = 1, shift = 0 ;
		in >> slope >> offset >> scale >> shift ;
		relativeFilter = specDataPointFilter(offset, slope, scale, shift) ;
	}
	else
		in >> absoluteFilters ;
	readItem(in) ;
	generateDescription();
}

QString specExchangeFilterCommand::description() const
{
	return QString() ;
}

bool specExchangeFilterCommand::mergeable(const specUndoCommand* other)
{
	return !relative
	       && !(((specExchangeFilterCommand*) other)->relative)
	       && (((specExchangeFilterCommand*) other)->itemPointer()) == itemPointer() ;
}

bool specExchangeFilterCommand::mergeWith(const QUndoCommand* ot)
{
	if(!parentObject()) return false ;
	const specExchangeFilterCommand* other = (const specExchangeFilterCommand*) ot ;
	if(!mergeable(other)) return false ;
	generateDescription();
	return true ;
}

void specExchangeFilterCommand::setRelativeFilter(const specDataPointFilter& f)
{
	relative = true ;
	relativeFilter = f ;
}

void specExchangeFilterCommand::setAbsoluteFilter(const specDataPointFilter& filter)
{
	relative = false ;
	specModelItem* item = itemPointer() ;
	absoluteFilters = (item ?
			   QVector<specDataPointFilter> (item->findDescendants<specDataItem*>().size(), filter)
			   : QVector<specDataPointFilter> (1, filter)) ;
}

void specExchangeFilterCommand::generateDescription()
{
	if(relative)
		setText(QObject::tr("Add filter.") + relativeFilter.description()) ;
	else
	{
		if(absoluteFilters.size() == 1)
			setText(QObject::tr("Exchange filter.") + absoluteFilters.first().description()) ;
		else
			setText(QObject::tr("Exchange ") + QString::number(absoluteFilters.size()) + QObject::tr(" filters.")) ;
	}
}

specStreamable::type specExchangeFilterCommand::typeId() const
{
	return relative ?
	       specStreamable::movePlotCommandId
	       : specStreamable::exchangeFilterCommandId ;
}
