#ifndef FITVARIABLEEDITOR_H
#define FITVARIABLEEDITOR_H

#include "abstractvariableeditor.h"

namespace Ui {
	class fitVariableEditor;
}

class fitVariableEditor : public abstractVariableEditor
{
	Q_OBJECT

public:
	explicit fitVariableEditor(QWidget *parent = 0);
	~fitVariableEditor();
	QString variableDefinition() const ;
	void initialize(const QString& def) ;

private:
	Ui::fitVariableEditor *ui;
};

#endif // FITVARIABLEEDITOR_H
