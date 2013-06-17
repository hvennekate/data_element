#ifndef SPECFORMULAVALIDATOR_H
#define SPECFORMULAVALIDATOR_H

#include <QValidator>
#include <QStringList>
#include <QRegExp>

class specFormulaValidator : public QValidator
{
	Q_OBJECT
private:
	QRegExp variables ;
	static int openCloseDifference(const QString& s) ;
public:
	explicit specFormulaValidator(const QRegExp&, QObject *parent = 0);
	void fixup(QString &) const;
	State validate(QString &, int &) const ;
signals:
	void evaluationError(const QString&) const ;
};

#endif // SPECFORMULAVALIDATOR_H
