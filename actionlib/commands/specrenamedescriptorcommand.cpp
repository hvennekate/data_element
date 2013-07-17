#include "specrenamedescriptorcommand.h"
#include "specmodel.h"

specRenameDescriptorCommand::specRenameDescriptorCommand(specUndoCommand *parent)
	: specUndoCommand(parent)
{
}

void specRenameDescriptorCommand::setRenamingMap(QMap<QString, QString> m)
{
	map.swap(m);
}

void specRenameDescriptorCommand::writeCommand(QDataStream &out) const
{
	out << map ;
}

void specRenameDescriptorCommand::readCommand(QDataStream &in)
{
	in >> map ;
}

void specRenameDescriptorCommand::doIt()
{
	specModel* myModel = qobject_cast<specModel*>(parentObject()) ;
	if (!myModel) return ;
	myModel->signalBeginReset();
	myModel->renameDescriptors(map) ;
	QMap<QString, QString> inverted ;
	foreach(const QString& key, map.keys())
		inverted[map[key]] = key ;
	map.swap(inverted);
}

void specRenameDescriptorCommand::undoIt()
{
	doIt() ;
}
