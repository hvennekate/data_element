#ifndef SPECMERGEDIALOG_H
#define SPECMERGEDIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include "names.h"

namespace Ui
{
	class specMergeDialog;
}

typedef QPair<QString, double>  stringDoublePair ;
class mergeModel : public QAbstractTableModel
{
	Q_OBJECT
private:
	class mergeModelPrivate ;
	mergeModelPrivate* d ;
public:
	mergeModel(QObject* parent = 0) ;
	~mergeModel() ;
	void setDescriptors(const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties) ;
	QList<stringDoublePair> getMergeCriteria() const ;

	int rowCount(const QModelIndex& parent) const ;
	int columnCount(const QModelIndex& parent) const ;
	QVariant data(const QModelIndex& index, int role) const ;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const ;
	bool setData(const QModelIndex& index, const QVariant& value, int role) ;
	Qt::ItemFlags flags(const QModelIndex& index) const ;
};

class specMergeDialog : public QDialog
{
	Q_OBJECT

public:
	explicit specMergeDialog(QWidget* parent = 0);
	~specMergeDialog();
	void setDescriptors(const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties) ;
	void getMergeCriteria(QList<stringDoublePair>& toCompare, bool& doSpectralAdaptation) const ;

private:
	Ui::specMergeDialog* ui;

	mergeModel* mm ;

};

class mergeDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	explicit mergeDelegate(QObject* parent = 0) ;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // SPECMERGEDIALOG_H
