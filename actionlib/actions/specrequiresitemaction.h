#ifndef SPECREQUIRESITEMACTION_H
#define SPECREQUIRESITEMACTION_H
#include "specitemaction.h"

class specRequiresItemAction : public specItemAction
{
	Q_OBJECT
public:
	explicit specRequiresItemAction(QObject *parent = 0);
protected:
	bool specificRequirements() ;
	bool postProcessingRequirements() const ;
private:
	QList<specStreamable::type> requiredTypes() const ;
};

class specRequiresMetaItemAction : public specRequiresItemAction
{
	Q_OBJECT
private:
	QList<specStreamable::type> requiredTypes() const ;
public:
	explicit specRequiresMetaItemAction(QObject* parent = 0) ;
};

class specRequiresDataItemAction : public specRequiresItemAction
{
	Q_OBJECT
private:
	QList<specStreamable::type> requiredTypes() const ;
public:
	explicit specRequiresDataItemAction(QObject* parent = 0) ;
};

#endif // SPECREQUIRESITEMACTION_H
