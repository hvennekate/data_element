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
	qDebug("Editor event") ;
	return QItemDelegate::editorEvent(event,model,option,index) ;
}

QWidget* specDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	QTextEdit *editor = new QTextEdit(parent) ;
//	editor->setFrame(false) ;
//	editor->setTextBackgroundColor(Qt::yellow);
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

void specDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	((QTextEdit*) editor)->setText(index.model()->data(index, spec::fullContentRole).toString()) ;
}

void specDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	qDebug("setting model data") ;
	int activeLine = 0 ;
	int lastNewLine = ((QTextEdit*) editor)->textCursor().position() ;
	while ((lastNewLine = ((QTextEdit*) editor)->toPlainText().lastIndexOf(QRegExp("\n"),lastNewLine) - 1) > -1)
		++ activeLine;
	model->setData(index,((QTextEdit*) editor)->toPlainText(),Qt::EditRole) ;
	if (activeLine != 0)
		model->setData(index,activeLine,spec::activeLineRole) ;
}

void specDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	qDebug("updating editor geometry") ;
	QRect geom = option.rect ;
	if(!index.column()) // TODO :  Improve!!
		geom.setX(geom.x()+option.decorationSize.width()+3) ;
	editor->setGeometry(geom);
}


specDelegate::~specDelegate()
{
}


