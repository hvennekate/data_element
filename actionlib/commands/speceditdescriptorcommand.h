#ifndef SPECEDITDESCRIPTORCOMMAND_H
#define SPECEDITDESCRIPTORCOMMAND_H

#include "specsingleitemcommand.h"

class specEditDescriptorCommand : public specSingleItemCommand<specModelItem>
{
private:
	QStringList previousContent ; // TODO change this to variant, make folder responsible for spreading to children
	QString descriptor ;
	QVector<int> previousActiveLine ; // TODO multiline bzw. gleich ganzen Descriptor mitnehmen
	void doIt() ;
	void undoIt() ;
	void writeCommand(QDataStream& out) const ;
	void readCommand(QDataStream& in) ;
	type typeId() const {return specStreamable::editDescriptorCommandId ;}
	QString description() const ;
	void generateDescription() ;
public:
	explicit specEditDescriptorCommand(specUndoCommand* parent = 0) ;
	void setItem(specModelItem*, QString descriptor,
		     QString newContent, int activeLine = 0) ;
	//	bool mergeWith(const QUndoCommand *other) ;
	//	bool mergeable(const specUndoCommand *other) ;
};

#endif // SPECEDITDESCRIPTORCOMMAND_H
