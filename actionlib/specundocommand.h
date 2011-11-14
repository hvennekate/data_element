#ifndef SPECUNDOCOMMAND_H
#define SPECUNDOCOMMAND_H

#include <QUndoCommand>

class specUndoCommand : public QUndoCommand
{
protected:
	QWidget *pW ;
public:
	explicit specUndoCommand(specUndoCommand *parent = 0);
//	virtual void redo() = 0;
//	virtual void undo() = 0;

	virtual QDataStream& write(QDataStream& out) const = 0 ;
	virtual QDataStream& read(QDataStream& in) = 0 ;
	virtual bool mergeable(const specUndoCommand* other) { Q_UNUSED(other) ; return true ; }
	void setParentWidget(QWidget*) ;
	QWidget *parentWidget() ;
signals:

public slots:

};

#endif // SPECUNDOCOMMAND_H
