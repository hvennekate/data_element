#ifndef SPECUNDOACTION_H
#define SPECUNDOACTION_H

#include <QAction>
#include <typeinfo>
#include "specview.h"
#include "specplot.h"
#include "specactionlibrary.h"

class specUndoAction : public QAction
{
	Q_OBJECT
public:
	explicit specUndoAction(QObject* parent = 0);
	virtual const std::type_info& possibleParent() = 0;
	void setLibrary(specActionLibrary*) ;
protected:
	specActionLibrary* library ;
	virtual void execute() = 0 ;
private slots:
	void gotTrigger() ;
};

#endif // SPECUNDOACTION_H
