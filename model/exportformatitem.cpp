#include "exportformatitem.h"
#include <QLabel>

exportFormatItem::exportFormatItem ( QWidget *parent )
	: QWidget ( parent )
{
	Value = new QComboBox() ;
	Separator = new QComboBox() ;
	removeButton = new QPushButton(QIcon(":/remove.png"),"") ;
	layout = new QHBoxLayout() ;
	
	QStringList values, separators ;
	values << "time" << "wavenumber" << "signal" << "maximum intensity" ;
	separators << "none" << "space" << "tab" << "new line" ;
	Value->addItems(values) ;
	Separator->addItems(separators) ;
	
	layout->addWidget(new QLabel("value:")) ;
	layout->addWidget(Value) ;
	layout->addWidget(new QLabel("separator:")) ;
	layout->addWidget(Separator) ;
	layout->addStretch() ;
	layout->addWidget(removeButton) ;
	layout->setContentsMargins(0,0,0,0) ;
	setLayout(layout) ;
	
	removeButton->setFixedHeight(16) ;
	removeButton->setFixedWidth(16) ;
	removeButton->setFlat(true) ;
	
	connect(removeButton,SIGNAL(clicked()),this,SLOT(remove())) ;

}

void exportFormatItem::remove()
{
	if (parentWidget() && parentWidget()->layout())
		parentWidget()->layout()->removeWidget(this) ;
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
	}
	return retString ;
}

spec::value exportFormatItem::value()
{
	return (spec::value) Value->currentIndex() ;
}
