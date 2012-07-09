#ifndef SPECUNDOCOMMAND_H
#define SPECUNDOCOMMAND_H

#include <QUndoCommand>
#include "specoutstream.h"
#include "specinstream.h"

class specUndoCommand : public QUndoCommand
{
protected:
	QObject *pW ;
public:
	explicit specUndoCommand(specUndoCommand *parent = 0);
//	virtual void redo() = 0;
//	virtual void undo() = 0;

	virtual void write(specOutStream& out) const = 0 ;
	virtual bool read(specInStream& in) = 0 ;
	virtual bool mergeable(const specUndoCommand* other) { Q_UNUSED(other) ; return true ; }
	void setParentObject(QObject*) ; // consider making virtual
	QObject *parentObject() const ;
signals:

public slots:

};

#endif // SPECUNDOCOMMAND_H
