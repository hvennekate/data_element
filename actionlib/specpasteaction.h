#ifndef SPECPASTEACTION_H
#define SPECPASTEACTION_H

#include "specundoaction.h"

class specPasteAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specPasteAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;

signals:

public slots:

private:
	void execute() ;
};

#endif // SPECPASTEACTION_H
