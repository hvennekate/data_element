#ifndef SPECUNDOCOMMAND_H
#define SPECUNDOCOMMAND_H

#include <QUndoCommand>
#include "specstreamable.h"

class specModel ;

class specUndoCommand : public QUndoCommand, public specStreamable
{
private:
	QObject* pO ;
	void readLabel(QDataStream& in) ;
	virtual void writeToStream(QDataStream& out) const ;
	virtual void readFromStream(QDataStream& in) ;
	void readAlternative(QDataStream &in, type t) ;
	virtual void readAlternativeCommand(QDataStream& in, type t) ;
protected:
	virtual QString description() const ;
	virtual void doIt() = 0;
	virtual void undoIt() = 0 ;
	virtual void parentAssigned() {}
	virtual void writeCommand(QDataStream& out) const = 0 ;
	virtual void readCommand(QDataStream& in) = 0 ;
	specModel* model() const ; // TODO to subclass
public:
	explicit specUndoCommand(specUndoCommand* parent = 0);
	void redo() ;
	void undo() ;

	virtual bool mergeable(const specUndoCommand* other) { Q_UNUSED(other) ; return false ; }
	void setParentObject(QObject*) ;  // consider making virtual
	int id() const { return typeId() ; }
	QObject* parentObject() const ;
signals:

public slots:

};

#endif // SPECUNDOCOMMAND_H
