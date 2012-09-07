#ifndef SPECMERGEACTION_H
#define SPECMERGEACTION_H

#include "specundoaction.h"
#include "specview.h"

class specMergeAction : public specUndoAction
{
    Q_OBJECT
private:
	bool getMergeCriteria(QList<QPair<QStringList::size_type, double> >& toCompare, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties, bool &spectralAdaptation) ;
	bool itemsAreEqual(specModelItem* first, specModelItem* second, const QList<QPair<QStringList::size_type, double> >& criteria, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties) ;
	QModelIndexList allChildren(const QModelIndex& parent) const ;
public:
	explicit specMergeAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specView) ; }
protected:
	void execute() ;

};

#endif // SPECMERGEACTION_H
