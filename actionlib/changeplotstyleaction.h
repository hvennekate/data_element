#ifndef CHANGEPLOTSTYLEACTION_H
#define CHANGEPLOTSTYLEACTION_H

#include "actionlib/specundoaction.h"
#include "spectral/specdataview.h"
#include "specstylecommand.h"

class specGenealogy ;

class changePlotStyleAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit changePlotStyleAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specView) ; }

signals:

public slots:

protected:

	void execute() ;
//	void specStyleCommand *command() = 0 ;

private:
	QAction *lineColorAction, *symbolAction ;
	QVector<QAction*> symbolActions ;
private slots:
	void actionTriggered() ;

};

#endif // CHANGEPLOTSTYLEACTION_H
