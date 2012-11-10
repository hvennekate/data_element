#ifndef SPECMERGEACTION_H
#define SPECMERGEACTION_H

#include "specitemaction.h"
#include "specdataview.h"

typedef QPair<QString,double>  stringDoublePair ;

class specMergeAction : public specItemAction
{
    Q_OBJECT
private:
	bool getMergeCriteria(QList<stringDoublePair>& toCompare, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties, bool &spectralAdaptation) ;
	QModelIndexList allChildren(const QModelIndex& parent) const ;
public:
	explicit specMergeAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specDataView) ; }
private:
	specUndoCommand* generateUndoCommand() ;

};

#endif // SPECMERGEACTION_H
