#ifndef SPECEDITDESCRIPTORCOMMAND_H
#define SPECEDITDESCRIPTORCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specEditDescriptorCommand : public specUndoCommand
{
private:
	QString previousContent ;
	QString descriptor ;
	int previousActiveLine ;
	specGenealogy *item ;
	void doIt() ;
	void undoIt() ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const {return specStreamable::editDescriptorCommandId ;}
public:
	explicit specEditDescriptorCommand(specUndoCommand* parent = 0) ;
	void setItem(const QModelIndex &index, QString descriptor,
		     QString newContent, int activeLine= 0) ;
//	bool mergeWith(const QUndoCommand *other) ;
//	bool mergeable(const specUndoCommand *other) ;
};

#endif // SPECEDITDESCRIPTORCOMMAND_H
