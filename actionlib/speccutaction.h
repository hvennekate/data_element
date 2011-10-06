#ifndef SPECCUTACTION_H
#define SPECCUTACTION_H

#include "specdeleteaction.h"

class specCutAction : public specDeleteAction
{
    Q_OBJECT
public:
    explicit specCutAction(QObject *parent = 0);

signals:

public slots:

protected:
	void execute();

};

#endif // SPECCUTACTION_H
