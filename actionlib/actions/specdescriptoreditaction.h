#ifndef SPECDESCRIPTOREDITACTION_H
#define SPECDESCRIPTOREDITACTION_H

#include <QDialog>
#include <QValidator>
class QLineEdit ;
class QCheckBox ;

class stringListEntryWidget : public QWidget
{
	Q_OBJECT
private:
	QLineEdit* newValue ;
	QCheckBox* Active ;
private slots:
	void checkStateChanged(int) ;
public:
	explicit stringListEntryWidget(QString content, QWidget* parent = 0) ;
	void setAllWidgets(QList<stringListEntryWidget*> widgets) ;
	QString content() const ;
	QString oldContent() const ;
	bool active() const ;
signals:
	void contentChanged() ;
};

class stringListValidator : public QValidator
{
private:
	QStringList* forbiddenList ;
public:
	stringListValidator(QWidget* parent = 0) ;
	void fixup(QString&) const ;
	State validate(QString&, int&) const ;
	void setForbidden(const QStringList& fl) const ;
	~stringListValidator() ;
};

class stringListEntryValidator : public stringListValidator
{
	Q_OBJECT
	QList<stringListEntryWidget*> allWidgets ;
public:
	stringListEntryValidator(QList<stringListEntryWidget*> allWidgets, stringListEntryWidget* parent = 0) ;
	State validate(QString&, int&) const ;
};

class stringListChangeDialog : public QDialog
{
	Q_OBJECT
	QList<stringListEntryWidget*> widgets ;
public:
	explicit stringListChangeDialog(QStringList strings, QWidget* parent = 0);
	QStringList inactive() const ;
	QMap<QString, QString> active() const ;
};

/* ============== Former Header ============== */


#include "specundoaction.h"

class specDescriptorEditAction : public specUndoAction
{
	Q_OBJECT
public:
	explicit specDescriptorEditAction(QObject* parent = 0) ;
private:
	void execute() ;
	const std::type_info& possibleParent() ;
};

#endif // STRINGLISTCHANGEDIALOG_H
