#include "specdelegate.h"
#include <QModelIndex>
#include <QTextStream>
#include <QTextEdit>
#include "names.h"

specDelegate::specDelegate(QObject *parent)
	: QItemDelegate(parent)
{
}

bool specDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	return QItemDelegate::editorEvent(event,model,option,index) ;
}

QWidget* specDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	QTextEdit *editor = new QTextEdit(parent) ;
	QPalette palette = editor->palette() ;
	palette.setColor(QPalette::Base,palette.toolTipBase().color());
	palette.setColor(QPalette::Text,palette.toolTipText().color());
	editor->setPalette(palette) ;
	editor->setFrameShadow(QFrame::Plain);
	editor->setFrameShape(QFrame::NoFrame) ;
	editor->setAcceptRichText(false) ;
	editor->setBackgroundRole(QPalette::Text);
	editor->setLineWrapMode(QTextEdit::NoWrap) ;
	editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	return editor ;
}

void specDelegate::setEditorData(QWidget *e, const QModelIndex &index) const
{
	QTextEdit *editor = qobject_cast<QTextEdit*>(e) ;
	if (!editor) return ;
	editor->setText(index.model()->data(index, Qt::EditRole).toString()) ;
}

void specDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QTextEdit *ed = (QTextEdit*) editor ;
	model->setData(index,
		       QList<QVariant>() << ed->toPlainText() << ed->textCursor().blockNumber(),
		       Qt::EditRole) ;
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


