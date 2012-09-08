#ifndef SPECADDFOLDERACTION_H
#define SPECADDFOLDERACTION_H

#include "specundoaction.h"

class specAddFolderAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specAddFolderAction(QObject *parent = 0);
	const std::type_info &possibleParent() ;
private:
	void execute() ;
};

#endif // SPECADDFOLDERACTION_H
