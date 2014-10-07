#ifndef EXCLUSIVEVALIDATOR_H
#define EXCLUSIVEVALIDATOR_H

#include <QValidator>
#include <QRegExp>
#include <QSet>

class QLineEdit ;

class exclusiveValidator : public QValidator
{
	Q_OBJECT
private:
	QList<QRegExp> forbiddenExpressions ;
	QSet<QLineEdit*> coveredWidgets ;
	QStringList getOtherEntries() const ;
private slots:
	void removeWidget() ;
public:
	explicit exclusiveValidator(QObject *parent = 0);
	void setForbiddenExpressions(const QList<QRegExp>& exps) ;
	void addForbiddenExpressions(const QRegExp& exp) ;
	void setLinkedWidgets(const QList<QLineEdit*>& wl) ;
	void addLinkedWidget(QLineEdit* w) ;

	void fixup(QString & s) ;
	State validate(QString&s, int&pos) const ;

signals:

public slots:

};

#endif // EXCLUSIVEVALIDATOR_H
