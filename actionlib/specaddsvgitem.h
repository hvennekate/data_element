#ifndef SPECADDSVGITEMACTION_H
#define SPECADDSVGITEMACTION_H

#include "specundoaction.h"

class specAddSVGItemAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specAddSVGItemAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;
private:
	void execute() ;
};

#endif // SPECADDSVGITEMACTION_H
