#include <QDialog>
#include <QValidator>
class QLineEdit ;
class QCheckBox ;

class stringListEntryWidget : public QWidget
{
    Q_OBJECT
private:
    QLineEdit *newValue ;
    QCheckBox *Active ;
private slots:
    void checkStateChanged(int) ;
public:
    explicit stringListEntryWidget(QString content, QWidget *parent = 0) ;
    void setAllWidgets(QList<stringListEntryWidget*> widgets) ;
    QString content() const ;
    QString oldContent() const ;
    bool active() const ;
signals:
    void contentChanged() ;
};

class stringListEntryValidator : public QValidator
{
    Q_OBJECT
    QList<stringListEntryWidget*> allWidgets ;
public:
    stringListEntryValidator(QList<stringListEntryWidget*> allWidgets, stringListEntryWidget* parent = 0) ;
    State validate(QString &, int &) const ;
    void fixup(QString &s) const;
};

class stringListChangeDialog : public QDialog
{
    Q_OBJECT
    QList<stringListEntryWidget*> widgets ;
public:
    explicit stringListChangeDialog(QStringList strings, QWidget *parent = 0);
    QStringList inactive() const ;
    QMap<QString,QString> active() const ;
};

/* ============== Former Header ============== */


#include "specdescriptoreditaction.h"
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDialogButtonBox>
#include "specmulticommand.h"
#include "specdeletedescriptorcommand.h"
#include "specrenamedescriptorcommand.h"

stringListEntryValidator::stringListEntryValidator(QList<stringListEntryWidget *> aW, stringListEntryWidget *parent)
	: QValidator(parent),
	  allWidgets(aW)
{}

QValidator::State stringListEntryValidator::validate(QString &s, int &pos) const
{
	Q_UNUSED(pos)
    if (s == "") return Intermediate ;
	if (stringListEntryWidget* p = qobject_cast<stringListEntryWidget*>(parent()))
		if (!p->active()) return Acceptable ;
	foreach(stringListEntryWidget* widget, allWidgets)
		if (widget != (stringListEntryWidget*) parent() &&
				widget->active() &&
				widget->content() == s)
			return Intermediate ;
	return Acceptable ;
}

void stringListEntryValidator::fixup(QString &s) const
{
	int n = 0 ;
	int count = 0 ;
	QString original = s ;
	while (validate(s,n) != Acceptable)
		s = original + QString::number(count++) ;
}

stringListEntryWidget::stringListEntryWidget(QString content, QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout(this) ;
	Active = new QCheckBox(content, this) ;
	Active->setChecked(true) ;
	newValue = new QLineEdit(content, this) ;

	layout->addWidget(Active) ;
	layout->addWidget(newValue) ;
	connect(Active,SIGNAL(stateChanged(int)),this,SLOT(checkStateChanged(int))) ;
}

bool stringListEntryWidget::active() const
{
	return Active->isChecked() ;
}

QString stringListEntryWidget::oldContent() const
{
	return Active->text() ;
}

QString stringListEntryWidget::content() const
{
	return newValue->text() ;
}

void stringListEntryWidget::setAllWidgets(QList<stringListEntryWidget *> widgets)
{
	disconnect(0,0,this,SLOT(verify())) ;
	newValue->setValidator(new stringListEntryValidator(widgets,this)) ;
}

void stringListEntryWidget::checkStateChanged(int s)
{
	newValue->setEnabled(Qt::Checked == s);
	if (newValue->isEnabled())
	{
		QString content = newValue->text() ;
		if (newValue->validator())
			newValue->validator()->fixup(content) ;
		if (newValue->text() != content)
		{
			newValue->selectAll();
			newValue->insert(content);
		}
	}
}

stringListChangeDialog::stringListChangeDialog(QStringList strings, QWidget *parent) :
	QDialog(parent)
{
	QScrollArea *scrollArea = new QScrollArea(this) ;
	QVBoxLayout *layout = new QVBoxLayout(this), *scrollLayout = new QVBoxLayout(scrollArea) ;
	layout->addWidget(scrollArea) ;
	foreach(QString string, strings)
	{
		stringListEntryWidget *widget = new stringListEntryWidget(string,this) ;
		widgets << widget ;
		scrollLayout->addWidget(widget) ;
	}
	foreach(stringListEntryWidget *widget, widgets)
		widget->setAllWidgets(widgets);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) ;
	layout->addWidget(buttonBox) ;
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept())) ;
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject())) ;
}

QStringList stringListChangeDialog::inactive() const
{
	QStringList result ;
	foreach(stringListEntryWidget* widget, widgets)
		if (!widget->active())
			result << widget->content() ;
	return result ;
}

QMap<QString, QString> stringListChangeDialog::active() const
{
	QMap<QString, QString> map ;
	foreach(stringListEntryWidget* widget, widgets)
		if (widget->active())
			map[widget->oldContent()] = widget->content() ;
	return map ;
}

specDescriptorEditAction::specDescriptorEditAction(QObject *parent)
    : specUndoAction(parent)
{
    setIcon(QIcon::fromTheme("document-properties")) ;
    setToolTip(tr("Edit columns")) ;
    setWhatsThis(tr("Use this action to edit/delete column headers."));
    setText(tr("Edit columns")) ;

}

specUndoCommand* specDescriptorEditAction::generateUndoCommand()
{
    specView *view = qobject_cast<specView*>(parent()) ;
    if (!view) return ;
    specModel *model = view->model() ;
    if (!model) return ;
    QStringList descriptors = model->descriptors() ;
    descriptors.removeAll("") ;
    stringListChangeDialog dialog(descriptors,this) ;
    dialog.exec() ;
    if (dialog.result() != QDialog::Accepted) return ;
    QStringList toDelete = dialog.inactive() ;
    QMap<QString, QString> toExchange = dialog.active() ;

    // TODO check for actual changes

    specMultiCommand *command = new specMultiCommand ;
    command->setText(tr("Modify column heads")) ;
    foreach(const QString& key, toDelete)
    {
        specDeleteDescriptorCommand *deleteCommand = new specDeleteDescriptorCommand(command,key) ;
        deleteCommand->setParentObject(model) ;
    }
    specRenameDescriptorCommand *renameCommand = new specRenameDescriptorCommand(command) ;
    renameCommand->setRenamingMap(toExchange) ;
    renameCommand->setParentObject(model) ;
    return command ;
}
