#ifndef SPECSHORTCUTDIALOG_H
#define SPECSHORTCUTDIALOG_H

#include <QDialog>
#include <QAbstractItemModel>
#include <QIcon>

class shortcutModel : public QAbstractItemModel
{
	Q_OBJECT
private:
	struct actionItem {
		QIcon icon ;
		QString title ;
		QList<QKeySequence> shortCuts ;
		void init(const QAction&) ;
		actionItem(const QAction&) ;
		actionItem(const QAction*) ;
		QString shortCutString() const ;
		QStringList shortCutList() const ;
		void setShortcuts(const QStringList&) ;
		void setShortcuts(const QString&) ;
		bool operator<(const actionItem&) const;
	};
	typedef QList<actionItem> actionItemContainer ;
	typedef QMap<QString, actionItemContainer> modelDataType ;
	modelDataType modelContent ;
	QString settingsKey(const QString& category, const QString& command) const ;
	const actionItem *fromIndex(const QModelIndex&) const ;
	actionItem *fromIndex(const QModelIndex&) ;
public:
	explicit shortcutModel(QObject* parent = 0) ;
	QModelIndex index(int row, int column, const QModelIndex& parent) const ;
	QModelIndex parent(const QModelIndex& child) const ;
	int rowCount(const QModelIndex& parent) const ;
	int columnCount(const QModelIndex& parent) const ;
	QVariant data(const QModelIndex& index, int role) const ;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const ;
	bool setData(const QModelIndex& index, const QVariant& value, int role) ;
	Qt::ItemFlags flags(const QModelIndex& index) const ;

	void applyShortcutsToSettings() ;
};


namespace Ui
{
	class specShortcutDialog;
}

class specShortcutDialog : public QDialog
{
	Q_OBJECT

public:
	explicit specShortcutDialog(QWidget* parent = 0);
	~specShortcutDialog();
private:
	Ui::specShortcutDialog* ui;
private slots:
	void assignShortcuts() ;
};

#endif // SPECSHORTCUTDIALOG_H
