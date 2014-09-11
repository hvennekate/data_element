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
#include <QTableWidget>
#include <QMouseEvent>

stringListEntryValidator::stringListEntryValidator(QList<stringListEntryWidget*> aW, stringListEntryWidget* parent)
	: stringListValidator(parent),
	  allWidgets(aW)
{}

stringListValidator::stringListValidator(QWidget* parent)
	: QValidator(parent),
	  forbiddenList(new QStringList)
{}

stringListValidator::~stringListValidator()
{
	delete forbiddenList ;
}

QValidator::State stringListValidator::validate(QString& s, int& pos) const
{
	Q_UNUSED(pos)
	return forbiddenList->contains(s) ? Intermediate : Acceptable ;
}

void stringListValidator::setForbidden(const QStringList& fl) const
{
	*forbiddenList = fl ;
}

QValidator::State stringListEntryValidator::validate(QString& s, int& pos) const
{
	Q_UNUSED(pos)
	stringListEntryWidget* parentPointer = qobject_cast<stringListEntryWidget*> (parent()) ;
	if(!parentPointer || !parentPointer->active()) return Acceptable ;

	QStringList forbiddenStrings ;
	forbiddenStrings << QString() ;
	foreach(stringListEntryWidget * widget, allWidgets)
	if(widget != parentPointer &&
		widget->active())
		forbiddenStrings << widget->content() ;
	setForbidden(forbiddenStrings);
	return stringListValidator::validate(s, pos) ;
}

void stringListValidator::fixup(QString& s) const
{
	int n = 0 ;
	int count = 0 ;
	QString original = s ;
	while(validate(s, n) != Acceptable)
		s = original + QString::number(count++) ;
}

stringListEntryWidget::stringListEntryWidget(QString content, QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout(this) ;
	Active = new QCheckBox(content, this) ;
	Active->setChecked(true) ;
	newValue = new QLineEdit(content, this) ;

	layout->addWidget(Active) ;
	layout->addStretch();
	layout->addWidget(newValue) ;
	connect(Active, SIGNAL(stateChanged(int)), this, SLOT(checkStateChanged(int))) ;
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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

void stringListEntryWidget::setAllWidgets(QList<stringListEntryWidget*> widgets)
{
	disconnect(0, 0, this, SLOT(verify())) ;
	newValue->setValidator(new stringListEntryValidator(widgets, this)) ;
}

void stringListEntryWidget::checkStateChanged(int s)
{
	newValue->setEnabled(Qt::Checked == s);
	if(newValue->isEnabled())
	{
		QString content = newValue->text() ;
		if(newValue->validator())
			newValue->validator()->fixup(content) ;
		if(newValue->text() != content)
		{
			newValue->selectAll();
			newValue->insert(content);
		}
	}
}

stringListChangeDialog::stringListChangeDialog(QStringList strings, QWidget* parent) :
	QDialog(parent)
{
	setWindowTitle(tr("Edit descriptors")) ;
//	QScrollArea* scrollArea = new QScrollArea(this) ;
//	QWidget* areaWidget = new QWidget(scrollArea) ;
//	QVBoxLayout* layout = new QVBoxLayout(this), *scrollLayout = new QVBoxLayout(areaWidget) ;
//	layout->addWidget(scrollArea) ;
//	foreach(QString string, strings)
//	{
//		stringListEntryWidget* widget = new stringListEntryWidget(string, this) ;
//		widgets << widget ;
//		scrollLayout->addWidget(widget) ;
//	}
//	foreach(stringListEntryWidget * widget, widgets)
//		widget->setAllWidgets(widgets);
//	scrollArea->setWidget(areaWidget);
//	layout->addWidget(buttonBox) ;

	QVBoxLayout* layout = new QVBoxLayout(this) ;

	QTableWidget *table = new QTableWidget(strings.size(), 2, this) ;
	layout->addWidget(table);
	// Disable/change headers

	for (int i = 0 ; i < strings.size() ; ++i)
	{
		QTableWidgetItem *item = new QTableWidgetItem(strings[i]) ;
		item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
		table->setItem(i, 0, item);

		QTableWidgetItem *flagItem = new QTableWidgetItem ;
		flagItem->setData(Qt::DisplayRole, (uint) (spec::editable | spec::numeric));
		table->setItem(i, 1, flagItem) ;
	}
	table->setItemDelegate(new descriptorDelegate(table));

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) ;
	layout->addWidget(buttonBox) ;
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept())) ;
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject())) ;
}

QStringList stringListChangeDialog::inactive() const
{
	QStringList result ;
//	foreach(stringListEntryWidget * widget, widgets)
//	if(!widget->active())
//		result << widget->content() ;
	return result ;
}

QMap<QString, QString> stringListChangeDialog::active() const
{
	QMap<QString, QString> map ;
//	foreach(stringListEntryWidget * widget, widgets)
//	if(widget->active())
//		map[widget->oldContent()] = widget->content() ;
	return map ;
}

specDescriptorEditAction::specDescriptorEditAction(QObject* parent)
	: specUndoAction(parent)
{
	setIcon(QIcon(":/editDescriptors.png")) ;
	setToolTip(tr("Edit columns")) ;
	setWhatsThis(tr("Use this action to edit/delete column headers."));
	setText(tr("Edit columns")) ;

}

void specDescriptorEditAction::execute()
{
	specView* view = qobject_cast<specView*> (parent()) ;
	if(!view) return ;
	specModel* model = view->model() ;
	if(!model) return ;
	QStringList descriptors = model->descriptors() ;
	descriptors.removeAll("") ;
	stringListChangeDialog dialog(descriptors) ;
	dialog.exec() ;
	if(dialog.result() != QDialog::Accepted) return ;
	QStringList toDelete = dialog.inactive() ;
	QMap<QString, QString> toExchange = dialog.active() ;
	foreach(const QString & key, toExchange.keys())
	if(toExchange[key] == key)
		toExchange.remove(key) ;

	// TODO check for actual changes

	specMultiCommand* command = new specMultiCommand ;
	command->setText(tr("Modify column heads")) ;
	command->setParentObject(model);
	foreach(const QString & key, toDelete)
	{
		specDeleteDescriptorCommand* deleteCommand = new specDeleteDescriptorCommand(command, key) ;
		deleteCommand->setParentObject(model) ;
	}
	specRenameDescriptorCommand* renameCommand = new specRenameDescriptorCommand(command) ;
	renameCommand->setRenamingMap(toExchange) ;
	renameCommand->setParentObject(model) ;
	if(!library)
	{
		command->redo();
		delete command ;
	}
	else
		library->push(command) ;
}

const std::type_info& specDescriptorEditAction::possibleParent()
{
	return typeid(specView) ;
}


descriptorDelegate::descriptorDelegate(QObject *parent)
	: QItemDelegate(parent),
	  numericMap(QPixmap(":/numeric.png")),
	  lockedMap(QPixmap(":/locked.png")),
	  lockedNumericMap(QPixmap(":/lockednumeric.png"))
{
}

void descriptorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() != 1)
	{
		QItemDelegate::paint(painter, option, index) ;
		return ;
	}
	spec::descriptorFlags flags = getFlags(index) ;
	painter->drawPixmap(option.rect.x(), option.rect.y(),
			    flags & spec::numeric
			    ? (flags & spec::editable ? numericMap
						      : lockedNumericMap)
			    : (flags & spec::editable ? QPixmap()
						      : lockedMap));
}

QSize descriptorDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() != 1)
		return QItemDelegate::sizeHint(option, index) ;
	return lockedNumericMap.size() ;
}

bool descriptorDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if (index.column() != 1)
		return QItemDelegate::editorEvent(event, model, option, index) ;

	if (event->type() == QEvent::MouseButtonPress)
	{
		spec::descriptorFlags flags = getFlags(index) ;
		flags = (flags == spec::editable) ? spec::def
						  : (flags == spec::def ? spec::numeric
									: ((flags == spec::numeric) ? (spec::editable | spec::numeric)
												   : spec::editable)) ;
		model->setData(index, (uint) setFlags(index, flags)) ;
		return false ;
	}

	return true ;
}

spec::descriptorFlags descriptorDelegate::getFlags(const QModelIndex &index) const
{
	spec::descriptorFlags flags = (spec::descriptorFlags) index.data().toInt() ;
	return flags & (spec::editable | spec::numeric) ;
}

spec::descriptorFlags descriptorDelegate::setFlags(const QModelIndex &index, spec::descriptorFlags f) const
{
	spec::descriptorFlags flags = (spec::descriptorFlags) index.data().toInt() ;
	return f | (flags & ~(spec::editable | spec::numeric)) ;
}

