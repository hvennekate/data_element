#ifndef ACTIVEFITVAREDITOR_H
#define ACTIVEFITVAREDITOR_H

#include "abstractvariableeditor.h"

class activeFitVarEditor : public abstractVariableEditor
{
	Q_OBJECT
public:
	explicit activeFitVarEditor(QWidget *parent = 0);
	QString variableDefinition() const ;
	void initialize(const QString &initString) ;
	void setData(const QVariant &data) ;

};

#endif // ACTIVEFITVAREDITOR_H
