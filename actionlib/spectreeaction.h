#ifndef SPECTREEACTION_H
#define SPECTREEACTION_H

#include "specundoaction.h"
#include "model/specview.h"

class specTreeAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specTreeAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specView) ; }
protected:
	void execute() ;

private:
	QVector<QPair<specFolderItem*,QList<specModelItem*> > > moveTargets ;  // TODO do this differently (i.e. not modify this vector quite as often)
	specView *view ;
	specModel *model ;
};

#endif // SPECTREEACTION_H
