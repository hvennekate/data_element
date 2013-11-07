#include "exportlayoutitem.h"
#include <QTextStream>

exportLayoutItem::exportLayoutItem(QStringList& descriptors, QWidget* parent)
	: QWidget(parent)
{
	isFreeForm = new QCheckBox("Free form string") ;
	descriptor = new QComboBox() ;
	freeText = new QLineEdit() ;
	removeButton = new QPushButton(QIcon(":/remove.png"), "") ;
	layout = new QHBoxLayout() ;

	connect(isFreeForm, SIGNAL(stateChanged(int)), this, SLOT(freeFormMode(int))) ;

	descriptor->addItems(descriptors) ;

	layout->addWidget(descriptor) ;
	layout->addWidget(freeText) ;
	layout->addWidget(isFreeForm) ;
	layout->addWidget(removeButton) ;
	layout->setContentsMargins(0, 0, 0, 0) ;
	setLayout(layout) ;

	// 	isFreeForm->resize() ;
	isFreeForm->setFixedSize(isFreeForm->sizeHint()) ;
	descriptor->setFixedHeight(descriptor->sizeHint().height()) ;
	descriptor->setMaximumWidth(QWIDGETSIZE_MAX) ;
	freeText->setFixedHeight(descriptor->sizeHint().height()) ;
	freeText->setMaximumWidth(QWIDGETSIZE_MAX) ;
	removeButton->setFixedHeight(16) ;  //removeButton->sizeHint().height()) ;
	removeButton->setFixedWidth(16) ;  //removeButton->sizeHint().height()) ;

	removeButton->setFlat(true) ;

	freeFormMode(Qt::Unchecked) ;

	connect(removeButton, SIGNAL(clicked()), this, SLOT(remove())) ;
}

void exportLayoutItem::freeFormMode(int on)
{
	(on == Qt::Checked ? (QWidget*) descriptor : (QWidget*) freeText)->hide() ;
	(on == Qt::Checked ? (QWidget*) freeText : (QWidget*) descriptor)->show() ;
}

void exportLayoutItem::setDescriptors(const QStringList& ds)
{
	QString currentText = descriptor->currentText() ;
	if(!ds.contains(currentText) && !isFreeText())
	{
		remove();
		return ;
	}
	descriptor->clear();
	descriptor->addItems(ds) ;
	if(ds.contains(currentText))
		descriptor->setCurrentIndex(ds.indexOf(currentText)) ;
}

void exportLayoutItem::remove()
{
	//	if (parentWidget() && parentWidget()->layout())
	//		parentWidget()->layout()->removeWidget(this) ;
	delete this ;
}

exportLayoutItem::~exportLayoutItem()
{
}

bool exportLayoutItem::isFreeText()
{
	return isFreeForm->checkState() == Qt::Checked ;
}

QString exportLayoutItem::text()
{
	return (isFreeText() ? freeText->text() : descriptor->currentText()) ;
}
