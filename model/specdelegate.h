#ifndef SPECDELEGATE_H
#define SPECDELEGATE_H

#include <QItemDelegate>
#include <QMap>

class QStringList ;

class specDelegate : public QItemDelegate
{
	Q_OBJECT
	QMap<QString, QStringList*>* completerMap ;
	void syncCompleterMap(const QStringList&) const;
public:
	explicit specDelegate(QObject* parent = 0);
	~specDelegate() ;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) ;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const ;
};

#endif
