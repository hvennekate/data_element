#ifndef SPECUNDOCOMMAND_H
#define SPECUNDOCOMMAND_H

#include <QUndoCommand>
#include "specstreamable.h"

class specUndoCommand : public QUndoCommand, public specStreamable
{
private:
	QObject *pO ;
	virtual void writeToStream(QDataStream &out) const ;
	virtual void readFromStream(QDataStream &in) ;
protected:
	virtual void doIt() = 0;
	virtual void undoIt() = 0 ;
	virtual void parentAssigned() {}
	virtual void writeCommand(QDataStream &out) const = 0 ;
	virtual void readCommand(QDataStream &in) = 0 ;
public:
	explicit specUndoCommand(specUndoCommand *parent = 0);
	void redo() ;
	void undo() ;

	virtual bool mergeable(const specUndoCommand* other) { Q_UNUSED(other) ; return true ; }
	void setParentObject(QObject*) ; // consider making virtual
	int id() const { return typeId() ; }
	QObject *parentObject() const ;
signals:

public slots:

};

#endif // SPECUNDOCOMMAND_H
