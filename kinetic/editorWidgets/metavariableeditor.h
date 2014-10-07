#ifndef METAVARIABLEEDITOR_H
#define METAVARIABLEEDITOR_H

#include "abstractvariableeditor.h"

class specMetaVariable ;
class QDoubleValidator ;
class specMetaParser ;

namespace Ui {
	class metaVariableEditor;
}

class metaVariableEditor : public abstractVariableEditor
{
	Q_OBJECT
public:
	explicit metaVariableEditor(QWidget *parent = 0);
	~metaVariableEditor();
	QString variableDefinition() const ;
	void initialize(const QString& string) ;
	void setData(const QVariant& data) ;

private slots:
	void on_valueSource_currentIndexChanged(const QString &typeName) ;
	void sortMinMaxValues();

private:
	Ui::metaVariableEditor *ui;

	QStringList plotDataTypes() const ; // TODO move to specMetaVariable
	QStringList typeSymbols() const ; // TODO move to specMetaVariable and use more direct functions
	bool isPlotVarType() const ;
	QDoubleValidator *minValidator, *maxValidator ;
};

#endif // METAVARIABLEEDITOR_H
