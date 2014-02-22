#include "specshortcutdialog.h"
#include "ui_specshortcutdialog.h"
#include "specappwindow.h"
#include "specplotwidget.h"
#include <QSettings>

shortcutModel::actionItem::actionItem(const QAction & a)
{
	init(a) ;
}

shortcutModel::actionItem::actionItem(const QAction * a)
{
	init(*a) ;
}

void shortcutModel::actionItem::init(const QAction & a)
{
	icon = a.icon() ;
	shortCuts = a.shortcuts() ;
	title = a.iconText() ;
}

bool shortcutModel::actionItem::operator <(const shortcutModel::actionItem& other) const
{
	return title < other.title ;
}

QString shortcutModel::actionItem::shortCutString() const
{
	return shortCutList().join(", ") ;
}

QStringList shortcutModel::actionItem::shortCutList() const
{
	QStringList result ;
	foreach (const QKeySequence shortcut, shortCuts)
		result << shortcut ;
	return result ;
}

void shortcutModel::actionItem::setShortcuts(const QString & s)
{
	setShortcuts(s.split(QRegExp("\\s*,\\s*")));
}

void shortcutModel::actionItem::setShortcuts(const QStringList & l)
{
	shortCuts.clear();
	foreach(const QString& s, l)
		shortCuts << s ;
}

QString shortcutModel::settingsKey(const QString &category, const QString &command) const
{
	return "shortcuts/" + category + "/" + command ;
}

#define ITERATEOVERALLACTIONS(DOWITHACTIONS) \
	for (modelDataType::iterator i = modelContent.begin() ; i != modelContent.end() ; ++i) { \
		for(actionItemContainer::iterator a = i.value().begin() ; a != i.value().end() ; ++a) { \
	DOWITHACTIONS }}

void shortcutModel::applyShortcutsToSettings()
{
	QSettings settings ;
	ITERATEOVERALLACTIONS(settings.setValue(settingsKey(i.key(), a->title), a->shortCutList());)
}

shortcutModel::shortcutModel(QObject* parent)
	: QAbstractItemModel(parent)
{
	specAppWindow window ;
	specPlotWidget plotWidget(&window) ;

	foreach(QAction * action, window.findChildren<QAction*>())
	{
		if(action->text().isEmpty()) continue ;
		if(action->menu()) continue ;
		QObject* parent = action->parent() ;
		while(parent &&
			!qobject_cast<specAppWindow*> (parent) &&
			!qobject_cast<specDockWidget*> (parent))
			parent = parent->parent() ;
		if(!parent) continue ;

		modelContent[parent->objectName()] << action ;
	}

	// User settings override defaults
	QSettings settings ;
	ITERATEOVERALLACTIONS(
		// Compose key TODO outsource to function
		QString key = settingsKey(i.key(), a->title) ;
		// Get user settings, if they exist
		if(settings.contains(key))
			a->setShortcuts(settings.value(key).toStringList());
	)

	// Sort actions alphabetically
	foreach(const QString& key, modelContent.keys())
		qSort(modelContent[key]) ;
}

Qt::ItemFlags shortcutModel::flags(const QModelIndex& index) const
{
	return QAbstractItemModel::flags(index) |
	       (index.column() == 1 && index.parent().isValid() ?
		Qt::ItemIsEditable :
		Qt::NoItemFlags) ;
}

int shortcutModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 2 ;
}

QModelIndex shortcutModel::index(int row, int column, const QModelIndex& parent) const
{
	return createIndex(row, column, parent.isValid() ? parent.row() : -1) ;
}

QModelIndex shortcutModel::parent(const QModelIndex& child) const
{
	return child.internalId() == -1 ?
	       QModelIndex() :
	       createIndex(child.internalId(), 0, -1) ;
}

int shortcutModel::rowCount(const QModelIndex& parent) const
{
	if(!parent.isValid()) return modelContent.size() ;  // root item
	if(parent.parent().isValid()) return 0 ;  // child of a folder
	return modelContent[parent.data().toString()].size() ;
}

const shortcutModel::actionItem* shortcutModel::fromIndex(const QModelIndex & index) const
{
	if (!index.parent().isValid()) return 0 ;
	return &(modelContent[index.parent().data().toString()][index.row()]) ;
}

shortcutModel::actionItem* shortcutModel::fromIndex(const QModelIndex & i)
{
	return const_cast<actionItem*>(fromIndex(i)) ;
}

QVariant shortcutModel::data(const QModelIndex& index, int role) const
{
	const actionItem *ai = fromIndex(index) ;
	if(!index.column())
	{
		if(Qt::DecorationRole == role)
		{
			if(ai) return ai->icon ;
			return QIcon::fromTheme("folder") ;
		}
		else if(Qt::DisplayRole == role)
		{
			if(ai) return ai->title ;
			return modelContent.keys()[index.row()];
		}
	}

	if(!index.parent().isValid()) return QVariant() ;

	if(ai && (Qt::DisplayRole == role || Qt::EditRole == role))
		return ai->shortCutString() ;

	return QVariant() ;
}

QVariant shortcutModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(Qt::DisplayRole == role && orientation == Qt::Horizontal)
		return section ? tr("Shortcut") : tr("Action") ;
	return QVariant() ;
}

bool shortcutModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	actionItem* ai = fromIndex(index) ;
	if(Qt::EditRole != role) return false ;
	if(!index.parent().isValid()) return false ;
	if(!index.column()) return false ;
	if(!ai) return false ;

	QList<QKeySequence> sequences ;
	foreach(const QString & seq, value.toString().split(QRegExp("\\s*,\\s*")))
	{
		if (sequences.contains(seq)) continue ;
		sequences << seq ;
	}

	QMap<QKeySequence, actionItem*> assignment ;
	ITERATEOVERALLACTIONS(foreach(QKeySequence seq, a->shortCuts) assignment[seq] = a.operator ->() ;) ;

	QList<QKeySequence>::iterator i = sequences.begin() ;
	while (i != sequences.end())
	{
		if (!assignment.contains(*i) || assignment[*i] == ai)
		{
			++i ;
			continue ;
		}
		if (QMessageBox::Yes ==
				QMessageBox::question(0,
						      tr("Shortcut already assigned"),
						      tr("The shortcut ")
						      + i->toString()
						      + tr("is already assigned to ")
						      + assignment[*i]->title
						      + tr(". Reassign?"),
						      QMessageBox::Yes | QMessageBox::No,
						      QMessageBox::No))
		{
			assignment[*i]->shortCuts.removeAll(*i) ;
			++i ;
		}
		else
			i = sequences.erase(i) ;
	}

	ai->shortCuts = sequences ;
	return true ;
}


specShortcutDialog::specShortcutDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::specShortcutDialog)
{
	ui->setupUi(this);
	setModal(true) ;
	ui->treeView->setModel(new shortcutModel(this)) ;
	ui->treeView->expandAll() ;
	ui->treeView->resizeColumnToContents(0) ;
	ui->treeView->resizeColumnToContents(1) ;
	connect(this, SIGNAL(accepted()), this, SLOT(assignShortcuts())) ;
}

void specShortcutDialog::assignShortcuts()
{
	shortcutModel* model = findChild<shortcutModel*>() ;
	if(model)
		model->applyShortcutsToSettings();

	if(!parent()) return ;
	QSettings settings ;
	settings.beginGroup("shortcuts");
	foreach(const QString & widgetName, settings.childGroups())
	{
		QList<QWidget*> widgets(parent()->findChildren<QWidget*> (widgetName));
		if(parent()->objectName() == widgetName && parentWidget())
			widgets << parentWidget() ;
		settings.beginGroup(widgetName) ;

		QMap<QString, QList<QKeySequence> > sequencePerAction ;
		foreach(const QString & actionName, settings.childKeys())
		foreach(const QString & seq, settings.value(actionName).toStringList())
		sequencePerAction[actionName] << seq ;

		foreach(QWidget * widget, widgets)
		foreach(QAction * action, widget->findChildren<QAction*>())
		if(sequencePerAction.contains(action->text()))
			action->setShortcuts(sequencePerAction[action->text()]) ;

		settings.endGroup();
	}
}

specShortcutDialog::~specShortcutDialog()
{
	delete ui;
}
