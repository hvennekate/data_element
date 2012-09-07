#ifndef SPECLABELACTION_H
#define SPECLABELACTION_H

#include "specundoaction.h"

class specSimpleTextEdit ;

class specLabelAction : public specUndoAction
{
	Q_OBJECT
private:
	void execute() ;
protected:
	virtual QString textToEdit() = 0 ;
	virtual specStreamable::type commandId() = 0 ;
public:
	explicit specLabelAction(QObject *parent = 0) ;
	const std::type_info &possibleParent() ;
};

class specTitleAction : public specLabelAction
{
	Q_OBJECT
public:
	explicit specTitleAction(QObject *parent = 0) ;
private:
	QString textToEdit() ;
	specStreamable::type commandId() ;
};

class specXLabelAction : public specLabelAction
{
	Q_OBJECT
public:
	explicit specXLabelAction(QObject* parent = 0) ;
private:
	QString textToEdit() ;
	specStreamable::type commandId() ;
};

class specYLabelAction : public specLabelAction
{
	Q_OBJECT
public:
	explicit specYLabelAction(QObject* parent = 0) ;
private:
	QString textToEdit() ;
	specStreamable::type commandId() ;
};

#endif // SPECLABELACTION_H
