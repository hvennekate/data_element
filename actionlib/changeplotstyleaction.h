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

protected:

	void execute() ;

private:
	QMenu *lineColorMenu,
		*symbolMenu,
		*symbolInnerColorMenu,
		*symbolOuterColorMenu,
		*symbolSizeMenu,
		*lineWidthMenu ;
	QVector<Qt::GlobalColor> colors ;
	QVector<double> sizes ;
	QVector<QAction*> symbolActions,
		lineWidthActions,
		lineColorActions,
		symbolInnerColorActions,
		symbolOuterColorActions,
		symbolSizeActions ;
	QColor getColor(int index) ;
	double getSize(int index, bool& ok) ;
private slots:
	void actionTriggered(QAction*) ;

};

#endif // CHANGEPLOTSTYLEACTION_H
