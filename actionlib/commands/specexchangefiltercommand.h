#ifndef SPECEXCHANGEFILTERCOMMAND_H
#define SPECEXCHANGEFILTERCOMMAND_H

#include "specsingleitemcommand.h"
#include "specmodelitem.h"
#include "specdatapointfilter.h"

class specDataItem ;

class specExchangeFilterCommand : public specSingleItemCommand<specModelItem>
{
private:
	specDataPointFilter relativeFilter ;
	QVector<specDataPointFilter> absoluteFilters ;
	bool relative ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
	QVector<specDataPointFilter> collectFilters(const QVector<specDataItem*>& items) const ;
	void applyAbsoluteFilters(const QVector<specDataItem*>& items) const ;
	type typeId() const ;
	void generateDescription() ;
	QString description() const;
	void undoIt() ;
	void doIt() ;
public:
	explicit specExchangeFilterCommand(specUndoCommand* parent = 0, bool relative = false) ;
	bool mergeWith(const QUndoCommand *other) ;
	void setRelativeFilter(const specDataPointFilter& filter) ;
	void setAbsoluteFilter(const specDataPointFilter& filter) ;
	bool mergeable(const specUndoCommand *other) ;
};


#endif // SPECEXCHANGEFILTERCOMMAND_H
