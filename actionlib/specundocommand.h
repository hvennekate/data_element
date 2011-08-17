#ifndef SPECUNDOCOMMAND_H
#define SPECUNDOCOMMAND_H

#include <QUndoCommand>

class specUndoCommand : public QUndoCommand
{
public:
	explicit specUndoCommand(specUndoCommand *parent = 0);
	virtual void redo() = 0 ;
	virtual void undo() = 0 ;

	QDataStream& write(QDataStream&) ;
	QDataStream& read(QDataStream&) ;
signals:

public slots:

};

#endif // SPECUNDOCOMMAND_H
