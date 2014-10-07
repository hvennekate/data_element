#include "activefitvareditor.h"
#include <QVariant>
#include <QVBoxLayout>
#include <QCheckBox>

activeFitVarEditor::activeFitVarEditor(QWidget *parent) :
	abstractVariableEditor(parent)
{
	setLayout(new QVBoxLayout(this));
}

QString activeFitVarEditor::variableDefinition() const
{
	QStringList activated ;
	foreach(QCheckBox* cb, findChildren<QCheckBox*>())
		if (cb->isChecked())
			activated << cb->text() ;
	return activated.join(", ") ;
}

void activeFitVarEditor::initialize(const QString &initString)
{
	foreach(QCheckBox* cb, findChildren<QCheckBox*>())
		cb->setCheckState(Qt::Unchecked) ;
	foreach(const QString& s, initString.split(QRegExp("\\s*,\\s*")))
		foreach(QCheckBox* cb, findChildren<QCheckBox*>())
			if (s == cb->text())
				cb->setCheckState(Qt::Checked) ;
}

void activeFitVarEditor::setData(const QVariant &data)
{
	// alte aufraeumen
	foreach(QCheckBox* cb, findChildren<QCheckBox*>())
		delete cb ;
	foreach(const QString& s, data.toStringList())
		layout()->addWidget(new QCheckBox(s, this)) ;
}
