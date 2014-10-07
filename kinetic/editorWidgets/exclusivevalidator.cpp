#include "exclusivevalidator.h"
#include <QLineEdit>
#include "utility-functions.h"

QStringList exclusiveValidator::getOtherEntries() const
{
	QStringList others ;
	foreach(QLineEdit* editor, coveredWidgets)
		if (!editor->hasFocus())
			others << editor->text() ;
	return others ;
}

void exclusiveValidator::removeWidget()
{
	coveredWidgets.remove((QLineEdit*)sender()) ;
}

exclusiveValidator::exclusiveValidator(QObject *parent) :
	QValidator(parent)
{
}

void exclusiveValidator::setForbiddenExpressions(const QList<QRegExp> &exps)
{
	forbiddenExpressions = exps ;
}

void exclusiveValidator::addForbiddenExpressions(const QRegExp &exp)
{
	forbiddenExpressions << exp ;
}

void exclusiveValidator::setLinkedWidgets(const QList<QLineEdit *> &wl)
{
	foreach(QLineEdit* widget, coveredWidgets)
		disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(removeWidget(QObject*))) ;
	coveredWidgets.clear();
	foreach(QLineEdit* widget, wl)
		addLinkedWidget(widget) ;
}

void exclusiveValidator::addLinkedWidget(QLineEdit *w)
{
	coveredWidgets << w ;
	connect(w, SIGNAL(destroyed()), SLOT(removeWidget())) ;
}

void exclusiveValidator::fixup(QString &s)
{
	s = uniqueString(s, getOtherEntries()) ;
}

QValidator::State exclusiveValidator::validate(QString &s, int &pos) const
{
	Q_UNUSED(pos)
	// test for forbidden regExps
	foreach (const QRegExp& regExp, forbiddenExpressions)
		if (s.contains(regExp))
			return Invalid ;

	// test for other strings
	if (getOtherEntries().contains(s))
		return Intermediate ;

	return Acceptable ;
}
