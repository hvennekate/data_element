#ifndef CHANGEPLOTSTYLEACTION_H
#define CHANGEPLOTSTYLEACTION_H

#include "specrequiresitemaction.h"
#include "specdataview.h"
#include "specstylecommand.h"

class specGenealogy ;

class changePlotStyleAction : public specRequiresItemAction
{
	Q_OBJECT
public:
	explicit changePlotStyleAction(QObject *parent = 0);
	const std::type_info& possibleParent() { return typeid(specView) ; }
private:
	specUndoCommand* generateUndoCommand() ;
	QMenu *lineColorMenu,
	*symbolMenu,
	*symbolInnerColorMenu,
	*symbolOuterColorMenu,
	*symbolSizeMenu,
	*lineWidthMenu,
	*penStyleMenu ;
	QVector<Qt::GlobalColor> colors ;
	QVector<double> sizes ;
	QVector<QAction*> symbolActions,
	lineWidthActions,
	lineColorActions,
	symbolInnerColorActions,
	symbolOuterColorActions,
	symbolSizeActions,
	penStyleActions ;
	QColor getColor(int index) ;
	double getSize(int index, bool& ok) ;
	QObject *currentTrigger ;
	QAction *currentAction ;
private slots:
	void actionTriggered(QAction*) ;
};

#endif // CHANGEPLOTSTYLEACTION_H
