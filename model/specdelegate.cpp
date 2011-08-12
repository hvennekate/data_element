#include "specdelegate.h"
#include <QLineEdit>
#include <QModelIndex>
#include <QTextStream>

specDelegate::specDelegate(QObject *parent)
 : QItemDelegate(parent)
{
}

QWidget* specDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QLineEdit *editor = new QLineEdit(parent) ;
	editor->setFrame(false) ;
// 	QRect geometry = editor->geometry() ;
// 	geometry.setWidth(geometry.width()-parent->iconSize()) ;
// 	geometry.setX(geometry.x()-parent->iconSize()) ;
	return editor ;
}

void specDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	((QLineEdit*) editor)->setText(index.model()->data(index, Qt::DisplayRole).toString()) ;
}

void specDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	model->setData(index,((QLineEdit*) editor)->text(),Qt::EditRole) ;
}

void specDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QRect geom = option.rect ;
	if(!index.column()) // TODO :  Improve!!
		geom.setX(geom.x()+option.decorationSize.width()+3) ;
	editor->setGeometry(geom);
}


specDelegate::~specDelegate()
{
}


