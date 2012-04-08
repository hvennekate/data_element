#include "specundocommand.h"

specUndoCommand::specUndoCommand(specUndoCommand *parent) :
    QUndoCommand(parent),
	pW(0) //parent->parentWidget()) // TODO adapt calling code
{
}

void specUndoCommand::setParentWidget(QWidget * par)
{
	pW = par ;
}

QWidget* specUndoCommand::parentWidget() const
{
	return pW ;
}
