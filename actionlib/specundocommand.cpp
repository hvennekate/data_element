#include "specundocommand.h"

specUndoCommand::specUndoCommand(specUndoCommand *parent) :
    QUndoCommand(parent),
	pW(0)
{
}

void specUndoCommand::setParentWidget(QWidget * par)
{
	pW = par ;
}

QWidget* specUndoCommand::parentWidget()
{
	return pW ;
}
