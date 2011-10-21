#ifndef CHANGEPLOTSTYLEACTION_H
#define CHANGEPLOTSTYLEACTION_H

#include <specundoaction.h>

class changePlotStyleAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit changePlotStyleAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return specPlot ; }

signals:

public slots:

protected:
	void execute() ;

};

#endif // CHANGEPLOTSTYLEACTION_H
