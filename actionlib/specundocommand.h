#ifndef SPECUNDOCOMMAND_H
#define SPECUNDOCOMMAND_H

#include <QUndoCommand>

class specUndoCommand : public QUndoCommand
{
protected:
	QWidget *pW ;
public:
	explicit specUndoCommand(specUndoCommand *parent = 0);
	virtual void redo() = 0 ;
	virtual void undo() = 0 ;

	virtual QDataStream& write(QDataStream&) = 0 ;
	virtual QDataStream& read(QDataStream&) = 0 ;
	void setParentWidget(QWidget*) ;
	QWidget *parentWidget() ;
signals:

public slots:

};

#endif // SPECUNDOCOMMAND_H
