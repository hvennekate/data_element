#ifndef ABSTRACTVARIABLEEDITOR_H
#define ABSTRACTVARIABLEEDITOR_H

#include <QWidget>

class abstractVariableEditor : public QWidget
{
	Q_OBJECT
public:
	explicit abstractVariableEditor(QWidget *parent = 0);
	virtual QString variableDefinition() const = 0;
	virtual void initialize(const QString& initString) = 0;
	virtual void setData(const QVariant& data) { Q_UNUSED(data) ; }
};

#endif // ABSTRACTVARIABLEEDITOR_H
