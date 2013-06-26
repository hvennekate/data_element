#ifndef SPECRENAMEDESCRIPTORCOMMAND_H
#define SPECRENAMEDESCRIPTORCOMMAND_H

#include "specundocommand.h"
#include <QMap>
#include <QString>

class specRenameDescriptorCommand : public specUndoCommand
{
	QMap<QString, QString> map ;
	void writeCommand(QDataStream &out) const ;
	void readCommand(QDataStream &in) ;
	void doIt();
	void undoIt() ;
	type typeId() const { return specStreamable::renameDescriptorCommandId ; }
public:
	specRenameDescriptorCommand(specUndoCommand* parent = 0);
	void setRenamingMap(QMap<QString, QString> map) ;
};

#endif // SPECRENAMEDESCRIPTORCOMMAND_H
