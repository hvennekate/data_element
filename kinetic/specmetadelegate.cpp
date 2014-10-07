#include "specmetadelegate.h"
#include "specmodel.h"
#include "widgetlist.h"
#include "metavariableeditor.h"
#include "exclusivevalidator.h"
#include <QLineEdit>
#include "specmetamodel.h"
#include "fitvariableeditor.h"
#include "activefitvareditor.h"

void specMetaDelegate::provideEditor() const
{
	widgetList *wl = qobject_cast<widgetList*>(sender()) ;
	if (!wl) return ;
	initializeVariableEditor(wl) ;
}

void specMetaDelegate::initializeVariableEditor(widgetList *parent, QString initString) const
{
	if (!parent) return ;
	QString role = parent->property("specEditorToCreate").toString() ;
	abstractVariableEditor *editor = 0 ;
	if ("variables" == role) editor = new metaVariableEditor(parent) ;
	if (tr("Fit variables") == role) editor = new fitVariableEditor(parent) ;
	if (tr("Fit parameters") == role) editor = new activeFitVarEditor(parent) ;
	if (!editor) return ;
	if ("variables" == role) editor->setData(*descriptors) ;
	if (tr("Fit parameters") == role) editor->setData(*fitVariableNames) ;
	if (initString.isEmpty()) initString = "a" ;
	editor->initialize(initString) ;

	QLineEdit* le = editor->findChild<QLineEdit*>("variableName") ;
	if (le)
	{
		QString varName = le->text() ;
		variableNameValidator->fixup(varName) ;
		le->setText(varName) ;
		variableNameValidator->addLinkedWidget(le) ;
	}
	parent->addWidget(editor) ;
}

specMetaDelegate::specMetaDelegate(QObject* parent)
	: specDelegate(parent),
	  variableNameValidator(new exclusiveValidator(this)),
	  descriptors(new QStringList),
	  fitVariableNames(new QStringList)
{
	QList<QRegExp> forbiddenExps ;
	forbiddenExps << QRegExp("^[0-9]") << QRegExp("\\W") ;
	variableNameValidator->setForbiddenExpressions(forbiddenExps);
}

void specMetaDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyleOptionViewItem style = option ;
	if(!(
		    ((specModel*) index.model())->itemPointer(index)
		    ->descriptor("errors").isEmpty())
	  )
	{
		QFont font = style.font ;
		font.setBold(true) ;
		style.font = font ;
	}
	specDelegate::paint(painter, style, index) ;
}

QWidget *specMetaDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QString columnTitle = index.model()->headerData(index.column(), Qt::Horizontal).toString() ;
	if (columnTitle == "variables"
			|| columnTitle == tr("Fit variables")
			|| columnTitle == tr("Fit parameters"))
	{
		widgetList *wl = new widgetList(parent) ;
		descriptors->clear();
		const specMetaModel* model = qobject_cast<const specMetaModel*>(index.model()) ;
		if (model)
			*descriptors = model->getDataModel()->descriptors() ;
		(*fitVariableNames) = index.data(spec::fitVariablesRole).toStringList() ;
		connect(wl, SIGNAL(needWidget()), this, SLOT(provideEditor())) ;
		wl->setProperty("specEditorToCreate", columnTitle) ;
		if (columnTitle == tr("Fit parameters"))
			wl->setExtendable(false) ;
		wl->setMinimumWidth(200);
		if ("variables" == columnTitle) wl->setMinimumWidth(340);
		wl->setMaximumWidth(400);
		wl->setMinimumHeight(120) ;
		wl->setMaximumHeight(250);
		return wl ;
	}

	return specDelegate::createEditor(parent, option, index) ;
}

void specMetaDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString columnTitle = index.model()->headerData(index.column(), Qt::Horizontal).toString() ;
	if (columnTitle == "variables"|| columnTitle == tr("Fit variables"))
	{
		widgetList *wl = qobject_cast<widgetList*>(editor) ;
		if (!wl) return ;
		foreach(const QString& definition, index.data(Qt::EditRole).toString().split("\n"))
			if (!definition.isEmpty())
				initializeVariableEditor(wl, definition) ;
		return ;
	}
	if (columnTitle == tr("Fit parameters"))
	{
		widgetList *wl = qobject_cast<widgetList*>(editor) ;
		if (!wl) return ;
		initializeVariableEditor(wl, index.data(Qt::EditRole).toString()) ;
		return ;
	}

	specDelegate::setEditorData(editor, index) ;
}

void specMetaDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QString columnTitle = index.model()->headerData(index.column(), Qt::Horizontal).toString() ;
	if (columnTitle == "variables" || columnTitle == tr("Fit variables"))
	{
		widgetList *wl = qobject_cast<widgetList*>(editor) ;
		if (!wl) return ;
		QStringList variableDefinitions ;
		foreach(QWidget* widget, wl->widgets())
		{
			abstractVariableEditor *editor = qobject_cast<abstractVariableEditor*>(widget) ;
			if (!editor) continue ;
			variableDefinitions << editor->variableDefinition() ;
		}
		model->setData(index, variableDefinitions.join("\n")) ;
		return ;
	}
	else specDelegate::setModelData(editor, model, index) ;

	specMetaModel* metaModel = qobject_cast<specMetaModel*>(model) ;
	if (metaModel &&
			(index.model()->headerData(index.column(), Qt::Horizontal).toString() == "x"
			 || index.model()->headerData(index.column(), Qt::Horizontal).toString() == "y"
			 || index.model()->headerData(index.column(), Qt::Horizontal).toString() == tr("Fit expression")))
	{
		bool fit = index.model()->headerData(index.column(), Qt::Horizontal).toString() == tr("Fit expression") ;
		QStringList variables(index.data(fit
						 ? spec::fitVariablesRole
						 : spec::variableNamesRole).toStringList()),
				variablesInFormulae(index.data(fit
							       ? spec::fitVariablesInFormulaRole
							       : spec::variablesInFormulaeRole).toStringList()) ;
		if (fit) variablesInFormulae.removeAll("x") ;
		QStringList toBeAddedVariables((variablesInFormulae.toSet() - variables.toSet()).toList());
		toBeAddedVariables.replaceInStrings(QRegExp("$"), "=\"\"") ;
		QModelIndex variableDefinitions = index.sibling(index.row(), metaModel->descriptors().indexOf(fit
													      ? tr("Fit variables")
													      : QString("variables"))) ;
		QString oldVariables = model->data(variableDefinitions, Qt::EditRole).toString()  ;
		if (!oldVariables.isEmpty()) oldVariables += "\n" ;
		model->setData(variableDefinitions, oldVariables + toBeAddedVariables.join("\n")) ;
	}
}
