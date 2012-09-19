#ifndef SPECMERGEACTION_H
#define SPECMERGEACTION_H

#include "specundoaction.h"
#include "specdataview.h"

class specMergeAction : public specUndoAction
{
    Q_OBJECT
	typedef QPair<QString,double>  stringDoublePair ;
private:
	bool getMergeCriteria(QList<stringDoublePair>& toCompare, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties, bool &spectralAdaptation) ;
	bool itemsAreEqual(specModelItem* first, specModelItem* second, const QList<stringDoublePair>& criteria) ;
	QModelIndexList allChildren(const QModelIndex& parent) const ;
public:
	explicit specMergeAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specDataView) ; }
protected:
	void execute() ;

};

#endif // SPECMERGEACTION_H
