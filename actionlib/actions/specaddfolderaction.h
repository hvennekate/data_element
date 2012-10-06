#ifndef SPECADDFOLDERACTION_H
#define SPECADDFOLDERACTION_H

#include "specitemaction.h"

class specAddFolderAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specAddFolderAction(QObject *parent = 0);
private:
	specUndoCommand* generateUndoCommand() ;
};

#endif // SPECADDFOLDERACTION_H
