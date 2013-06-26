#ifndef SPECEDITDESCRIPTORCOMMAND_H
#define SPECEDITDESCRIPTORCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specEditDescriptorCommand : public specUndoCommand
{
private:
	QStringList previousContent ;
	QString descriptor ;
	QVector<int> previousActiveLine ;
	specGenealogy item ;
	void doIt() ;
	void undoIt() ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
	type typeId() const {return specStreamable::editDescriptorCommandId ;}
	void parentAssigned();
public:
	explicit specEditDescriptorCommand(specUndoCommand* parent = 0) ;
	void setItem(const QModelIndex &index, QString descriptor,
		     QString newContent, int activeLine= 0) ;
	//	bool mergeWith(const QUndoCommand *other) ;
	//	bool mergeable(const specUndoCommand *other) ;
};

#endif // SPECEDITDESCRIPTORCOMMAND_H
