#include "specdelegate.h"
#include <QModelIndex>
#include <QTextStream>
#include "textedit.h"
#include "names.h"
#include <QCompleter>
#include <QSet>
#include "specmodel.h"

specDelegate::specDelegate(QObject* parent)
	: QItemDelegate(parent),
	  completerMap(new QMap<QString, QStringList*>())
{
}

specDelegate::~specDelegate()
{
	foreach(QStringList * list, completerMap->values())
	delete list ;
	delete completerMap ;
}

void specDelegate::syncCompleterMap(const QStringList& descriptors) const
{
	QSet<QString> newDescriptors(descriptors.toSet()),
	     currentDescriptors(completerMap->keys().toSet()) ;
	foreach(const QString & descriptor, currentDescriptors - newDescriptors)
	delete completerMap->take(descriptor) ;
	foreach(const QString & descriptor, newDescriptors - currentDescriptors)
	completerMap->insert(descriptor, new QStringList()) ;
}

QWidget* specDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option)
	TextEdit* editor = new TextEdit(parent) ;

	const specModel* model = qobject_cast<const specModel*> (index.model()) ;
	if(model)
	{
		syncCompleterMap(model->descriptors()) ;
		QCompleter* completer = new QCompleter(* (completerMap->value(model->descriptors() [index.column()])), editor) ;   // TODO use parent instead
		completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
		editor->setCompleter(completer);
	}

	return editor ;
}

void specDelegate::setEditorData(QWidget* e, const QModelIndex& index) const
{
	QTextEdit* editor = qobject_cast<QTextEdit*> (e) ;
	if(!editor) return ;
	QString text = index.model()->data(index, Qt::EditRole).toString() ;
	editor->setText(text) ;
	int activeLine = index.data(spec::ActiveLineRole).toInt() ;
	for (int i = 0 ; i < activeLine ; ++i)
		editor->moveCursor(QTextCursor::Down);
}

void specDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QTextEdit* ed = qobject_cast<QTextEdit*>(editor) ;
	if (!ed) return ;
	QString text = ed->toPlainText() ;
	specModel* sm = qobject_cast<specModel*> (model) ;
	if(sm)
	{
		sm->setData(index,
			       QList<QVariant>() << text << ed->textCursor().blockNumber(),
			       Qt::EditRole) ;
		QString descriptor = sm->descriptors() [index.column()] ;
		if(completerMap->contains(descriptor))
		{
			QSet<QString> newWords = completerMap->value(descriptor)->toSet() ;
			foreach(const QString & word, text.split(QRegExp("\\s+")))
				newWords << word ;
			QStringList newList = newWords.toList() ;
			qSort(newList) ;
			completerMap->value(descriptor)->swap(newList) ;
		}
	}
	else
		model->setData(index, text) ;
}

void specDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QRect geom = option.rect ;
	if(!index.column())  // TODO :  Improve!!
		geom.setX(geom.x() + option.decorationSize.width() + 3) ;
	editor->setGeometry(geom);
}

QSize specDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = QItemDelegate::sizeHint(option, index) ;
	size.setHeight(qMax(16, size.height())) ; // TODO get actual icon size
	return size ;
}
