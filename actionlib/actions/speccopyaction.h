#ifndef SPECCOPYACTION_H
#define SPECCOPYACTION_H

#include "specrequiresitemaction.h"

class specCopyAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit specCopyAction(QObject *parent = 0);
	static void copyToClipboard(specModel*, const QModelIndexList& selection) ;
private:
	specUndoCommand *generateUndoCommand() ;
};

#endif // SPECCOPYACTION_H
