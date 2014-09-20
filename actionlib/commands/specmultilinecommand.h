#ifndef SPECMULTILINECOMMAND_H
#define SPECMULTILINECOMMAND_H

#include "specmultipleitemcommand.h"

class specMultiLineCommand : public specMultipleItemCommand
{
	friend class specDescriptorFlagsCommand ;
private:
	QString descriptorName ;
	void doIt() ;
	void undoIt();
	void writeCommandData(QDataStream &out) const ;
	void readCommandData(QDataStream &in) ;
	type typeId() const { return specStreamable::multilineCommandId ; }
	void generateDescription() ;
	QString description() const ;
protected:
	void signalModelChanged() ;
public:
	explicit specMultiLineCommand(QString d = "", specUndoCommand* parent = 0);
	explicit specMultiLineCommand(specUndoCommand* parent);
	void setDescriptor(const QString&) ;

};

class specDescriptorFlagsCommand : public specMultiLineCommand // TODO check in old code if this was used for anything other than multiline
{
private:
	typedef QPair<specGenealogy, qint8> itemPropertyPair ;
	typedef QVector<itemPropertyPair> itemsContainer ;
	specStreamable::type myType ;
	void readCommand(QDataStream &in) ;
	void writeCommand(QDataStream &out) const ;
	type typeId() const { return myType ; }
	itemsContainer oldItems ;
	void doIt();
	void parentAssigned();
public:
	explicit specDescriptorFlagsCommand(specUndoCommand* parent = 0) ;
};

#endif // SPECMULTILINECOMMAND_H
