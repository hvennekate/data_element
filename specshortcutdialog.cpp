#include "specshortcutdialog.h"
#include "ui_specshortcutdialog.h"
#include "specappwindow.h"
#include "specplotwidget.h"
#include <QSettings>

void shortcutModel::applyShortcutsToSettings() const
{
	QSettings settings ;
	for (int i = 0 ; i < dataList.size() ; ++i)
		for (int j = 0 ; j < dataList[i].second.size() ; ++j)
			settings.setValue("shortcuts/"
					  + dataList[i].first + "/"
					  + dataList[i].second[j].first.first,
					  dataList[i].second[j].second) ;
}

shortcutModel::shortcutModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	specAppWindow window ;
	specPlotWidget plotWidget(&window) ;

	QMap<QString, QList<QPair<
			QPair<QString, QIcon>,
			QStringList > > > premap ;
	foreach(QAction* action, window.findChildren<QAction*>())
	{
		if (action->text().isEmpty()) continue ;
		if (action->menu()) continue ;
		QObject* parent = action->parent() ;
		while (parent &&
				!qobject_cast<specAppWindow*>(parent) &&
				!qobject_cast<specDockWidget*>(parent))
			parent = parent->parent() ;
		if (!parent) continue ;

		QStringList seqs ;
		foreach(const QKeySequence& seq, action->shortcuts())
			seqs << seq ;
		premap[parent->objectName()] << qMakePair(
							qMakePair(action->text(), action->icon()),
							seqs) ;
	}

	// User settings override defaults
	QSettings settings ;
	foreach(const QString& category, premap.keys())
	{
		for (int i = 0 ; i < premap[category].size() ; ++i)
		{
			QString key = "shortcuts/"
					+ category
					+ "/"
					+ premap[category][i].first.first ;
			if (settings.contains(key))
				premap[category][i].second = settings.value(key).toStringList() ;
		}
	}

	foreach(const QString& category, premap.keys())
		dataList << qMakePair(category, premap[category]) ;
}

Qt::ItemFlags shortcutModel::flags(const QModelIndex &index) const
{
	return QAbstractItemModel::flags(index) |
			(index.column() == 1 && index.parent().isValid() ?
				 Qt::ItemIsEditable :
				 Qt::NoItemFlags) ;
}

int shortcutModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 2 ;
}

QModelIndex shortcutModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, parent.isValid() ? parent.row() : -1) ;
}

QModelIndex shortcutModel::parent(const QModelIndex &child) const
{
	return child.internalId() == -1 ?
				QModelIndex() :
				createIndex(child.internalId(), 0, -1) ;
}

int shortcutModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid()) return dataList.size() ; // root item
	if (parent.parent().isValid()) return 0 ; // child of a folder
	return dataList[parent.row()].second.size() ;
}

QVariant shortcutModel::data(const QModelIndex &index, int role) const
{
	if (!index.column())
	{
		if (Qt::DecorationRole == role)
		{
			if (index.parent().isValid())
				return dataList[index.parent().row()].
						second[index.row()].first.second ;
			return QIcon::fromTheme("folder") ;
		}
		else if (Qt::DisplayRole == role)
		{
			if (index.parent().isValid())
				return dataList[index.parent().row()].
						second[index.row()].first.first ;
			return dataList[index.row()].first ;
		}
	}

	if (!index.parent().isValid()) return QVariant() ;

	if (Qt::DisplayRole == role || Qt::EditRole == role)
		return dataList[index.parent().row()].
				second[index.row()].second.join(", ") ;

	return QVariant() ;
}

QVariant shortcutModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (Qt::DisplayRole == role && orientation == Qt::Horizontal)
		return section ? tr("Shortcut") : tr("Action") ;
	return QVariant() ;
}

bool shortcutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (Qt::EditRole != role) return false ;
	if (!index.parent().isValid()) return false ;
	if (!index.column()) return false ;

	QStringList seqList ;
	int parentRow = index.parent().row() ;
	foreach(const QString& seq, value.toString().split(", "))
	{
		bool assignable = true ;
		for (int i = 0 ; i < dataList[parentRow].second.size() ; ++i)
		{
			if (index.row() == i) continue ;
			if (dataList[parentRow].second[i].second.contains(seq))
			{
				if ( QMessageBox::Yes ==
				QMessageBox::question(0,
						      tr("Shortcut already assigned"),
						      tr("The shortcut ")
						      + seq
						      + tr("is already assigned to ")
						      + dataList[parentRow].second[i].first.first
						+ tr(". Reassign?"),
						QMessageBox::Yes | QMessageBox::No,
						QMessageBox::No))
					dataList[parentRow].second[i].second.removeAll(seq) ;
				else
					assignable = false ;
			}
		}
		if (assignable)
			seqList << seq ;
	}
	dataList[index.parent().row()].
			second[index.row()].second
			= seqList ;
	return true ;
}


specShortcutDialog::specShortcutDialog(QWidget *parent) :
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
	shortcutModel *model = findChild<shortcutModel*>() ;
	if (model)
		model->applyShortcutsToSettings();

	if (!parent()) return ;
	QSettings settings ;
	settings.beginGroup("shortcuts");
	foreach(const QString& widgetName, settings.childGroups())
	{
		QList<QWidget*> widgets(parent()->findChildren<QWidget*>(widgetName));
		if (parent()->objectName() == widgetName && parentWidget())
			widgets << parentWidget() ;
		settings.beginGroup(widgetName) ;

		QMap<QString, QList<QKeySequence> > sequencePerAction ;
		foreach(const QString& actionName, settings.childKeys())
			foreach(const QString& seq, settings.value(actionName).toStringList())
				sequencePerAction[actionName] << seq ;

		foreach(QWidget* widget, widgets)
			foreach(QAction* action, widget->findChildren<QAction*>())
				if (sequencePerAction.contains(action->text()))
					action->setShortcuts(sequencePerAction[action->text()]) ;

		settings.endGroup();
	}
}

specShortcutDialog::~specShortcutDialog()
{
	delete ui;
}
