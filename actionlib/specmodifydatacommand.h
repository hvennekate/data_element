#ifndef SPECMODIFYDATACOMMAND_H
#define SPECMODIFYDATACOMMAND_H

#include "specundocommand.h"
#include "specmodelitem.h"
#include "specgenealogy.h"
#include <QMap>

class specModifyDataCommand : public specUndoCommand
{
private:
	QMap<specGenealogy,QList<int> > corrections ;
public:
	explicit specModifyDataCommand(specUndoCommand *parent = 0) ;

	void redo() ;
	void undo() ;
	int id() const { return spec::modifyDataId; }
};

#endif // SPECMODIFYDATACOMMAND_H
