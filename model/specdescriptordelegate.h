#ifndef SPECDESCRIPTORDELEGATE_H
#define SPECDESCRIPTORDELEGATE_H

#include <QItemDelegate>

class specDescriptorDelegate : public QItemDelegate
{
public:
    specDescriptorDelegate(QObject *parent=0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
			  const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
		      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
			      const QStyleOptionViewItem &option, const QModelIndex &index) const;

    ~specDescriptorDelegate();

};

#endif
