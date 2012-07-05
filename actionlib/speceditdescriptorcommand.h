#ifndef SPECEDITDESCRIPTORCOMMAND_H
#define SPECEDITDESCRIPTORCOMMAND_H

#include "specundocommand.h"
#include "specdataitem.h"
#include "specgenealogy.h"

class specEditDescriptorCommand : public specUndoCommand
{
private:
	QString previousContent ;
	QString descriptor ;
	int previousActiveLine ;
	specGenealogy *item ;
public:
	explicit specEditDescriptorCommand(specUndoCommand* parent = 0) ;
	void setItem(const QModelIndex &index, QString descriptor,
		     QString newContent, int activeLine) ;
	QDataStream& write(QDataStream &out) const ;
	QDataStream& read(QDataStream &in) ;
	void redo() ;
	void undo() ;
	int id() const {return spec::editDescriptorId ;}
	bool mergeWith(const QUndoCommand *other) ;
	bool mergeable(const specUndoCommand *other) ;
};

#endif // SPECEDITDESCRIPTORCOMMAND_H
