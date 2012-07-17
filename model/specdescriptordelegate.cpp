#include "specdescriptordelegate.h"
#include <QLineEdit>
#include <QAbstractItemModel>

specDescriptorDelegate::specDescriptorDelegate(QObject* parent)
 : QItemDelegate(parent)
{
}

QWidget * specDescriptorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		      const QModelIndex &index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	return (new QLineEdit(parent)) ;
}

void specDescriptorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	((QLineEdit*) editor)->setText(
			index.model()->data(index, Qt::DisplayRole).toString()
				      );
}

void specDescriptorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
		      const QModelIndex &index) const
{
	model->setData(index, ((QLineEdit*) editor)->text(), Qt::EditRole) ;
}

void specDescriptorDelegate::updateEditorGeometry(QWidget *editor,
		      const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect) ;
}


specDescriptorDelegate::~specDescriptorDelegate()
{
}


