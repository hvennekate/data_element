#include "exportformatitem.h"
#include <QLabel>

exportFormatItem::exportFormatItem(const QStringList& values, QWidget* parent)
	: QWidget(parent)
{
	Value = new QComboBox() ;
	Separator = new QComboBox() ;
	removeButton = new QPushButton(QIcon::fromTheme("list-remove"), "") ;
	layout = new QHBoxLayout() ;

	QStringList separators ;
	separators << "none" << "space" << "tab" << "new line" ;
	Value->addItems(values) ;
	Separator->addItems(separators) ;

	layout->addWidget(new QLabel("value:")) ;
	layout->addWidget(Value) ;
	layout->addWidget(new QLabel("separator:")) ;
	layout->addWidget(Separator) ;
	layout->addStretch() ;
	layout->addWidget(removeButton) ;
	layout->setContentsMargins(0, 0, 0, 0) ;
	setLayout(layout) ;

	removeButton->setFixedHeight(16) ;
	removeButton->setFixedWidth(16) ;
	removeButton->setFlat(true) ;

	connect(removeButton, SIGNAL(clicked()), this, SLOT(remove())) ;

}

void exportFormatItem::remove()
{
	//	if (parentWidget() && parentWidget()->layout())
	//		parentWidget()->layout()->removeWidget(this) ;
	delete this ;
}

exportFormatItem::~exportFormatItem()
{
}

QString exportFormatItem::separator()
{
	QString retString ;
	switch((spec::separator) Separator->currentIndex())
	{
		case spec::space:
			retString = " " ; break ;
		case spec::tab:
			retString = '\t' ; break ;
		case spec::newline:
			retString = '\n'; break ;
		default: ;
	}
	return retString ;
}

spec::value exportFormatItem::value()
{
	return (spec::value) Value->currentIndex() ;
}

void exportFormatItem::setValue(spec::value v)
{
	Value->setCurrentIndex(v) ;
}

void exportFormatItem::setSeparator(spec::separator s)
{
	Separator->setCurrentIndex(s) ;
}

void exportFormatItem::setDataTypes(const QStringList& ds)
{
	QString currentText = Value->currentText() ;
	if(!ds.contains(currentText))
	{
		remove() ;
		return ;
	}
	Value->clear();
	Value->addItems(ds) ;
	Value->setCurrentIndex(ds.indexOf(currentText));
}
