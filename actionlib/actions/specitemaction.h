#ifndef SPECITEMACTION_H
#define SPECITEMACTION_H

#include "specundoaction.h"

class specView ;
class specModel ;


class specItemAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specItemAction(QObject *parent = 0) ;
	virtual const std::type_info& possibleParent() ;
	virtual QList<specStreamable::type> requiredTypes() const ;
private: // TODO
	virtual bool requirements() { return true ;}
protected:
	void execute();
	specView* view ;
	specModel* model ;
	QModelIndex currentIndex, insertionIndex ;
	QModelIndexList selection ;
	QList<specModelItem*> pointers ;
	specModelItem* currentItem ;
	int insertionRow ;
	void expandSelectedFolders(QList<specModelItem*>& items, QList<specModelItem*>& folders) ;
protected:
	virtual specUndoCommand* generateUndoCommand() = 0 ;
};

#endif // SPECITEMACTION_H
