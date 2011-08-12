#include "exportdialog.h"
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QTextStream>
#include <QLabel>

exportDialog::exportDialog (QStringList* desc, QWidget *parent )
	: QDialog ( parent ), descriptors(desc)
{
	setWindowTitle(tr("Output format")) ;
	setMinimumWidth(400) ;
	
	layout = new QVBoxLayout(this) ;
	setLayout(layout) ;
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
			| QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	
	prepareHeader() ;
	prepareData() ;
	layout->addWidget(new QLabel("Item description format")) ;
	layout->addWidget(scrollHeader) ;
	layout->addWidget(new QLabel("Individual data point output format")) ;
	layout->addWidget(scrollData) ;
	layout->addWidget(buttonBox) ;
}

void exportDialog::prepareHeader()
{
	scrollHeader = new QScrollArea() ;
	
	//set up scroll widget
	QWidget *scrollWidget = new QWidget() ;
	inLayout = new QVBoxLayout(scrollWidget) ;
	scrollWidget->setLayout(inLayout) ;
	scrollHeader->setWidget(scrollWidget) ;
	scrollHeader->setWidgetResizable(true) ;
	
	// prepare add button
	QWidget *add = new QWidget() ;
	QHBoxLayout *addButtonLayout = new QHBoxLayout() ;
	addButtonLayout->addStretch() ;
	addButton = new QPushButton(QIcon(":/add.png"),"");
	addButton->setFixedHeight(16) ;
	addButton->setFixedWidth(16) ;
	addButton->setFlat(true) ;
	addButtonLayout->addWidget(addButton) ;
	addButtonLayout->setContentsMargins(0,4,0,0) ;
	add->setLayout(addButtonLayout) ;
	inLayout->addWidget(add) ;
	connect(addButton,SIGNAL(clicked()),this,SLOT(addHeaderItem())) ;
	
	// styling layout of scroll area
	inLayout->addStretch() ;
	inLayout->setSpacing(0) ;
	inLayout->setContentsMargins(0,0,0,0) ;
}

void exportDialog::prepareData()
{
	scrollData = new QScrollArea() ;
	
	//set up scroll widget
	QWidget *scrollWidget = new QWidget() ;
	dataLayout = new QVBoxLayout(scrollWidget) ;
	scrollWidget->setLayout(dataLayout) ;
	scrollData->setWidget(scrollWidget) ;
	scrollData->setWidgetResizable(true) ;
	
	// prepare add button
	QWidget *add = new QWidget() ;
	QHBoxLayout *addButtonLayout = new QHBoxLayout() ;
	addButtonLayout->addStretch() ;
	addDataButton = new QPushButton(QIcon(":/add.png"),"");
	addDataButton->setFixedHeight(16) ;
	addDataButton->setFixedWidth(16) ;
	addDataButton->setFlat(true) ;
	addButtonLayout->addWidget(addDataButton) ;
	addButtonLayout->setContentsMargins(0,4,0,0) ;
	add->setLayout(addButtonLayout) ;
	dataLayout->addWidget(add) ;
	connect(addDataButton,SIGNAL(clicked()),this,SLOT(addDataItem())) ;
	
	// styling layout of scroll area
	dataLayout->addStretch() ;
	dataLayout->setSpacing(0) ;
	dataLayout->setContentsMargins(0,0,0,0) ;
}

void exportDialog::addHeaderItem()
{
	inLayout->insertWidget(inLayout->count()-2,new exportLayoutItem(*descriptors)) ;
}

void exportDialog::addDataItem()
{
	dataLayout->insertWidget(dataLayout->count()-2,new exportFormatItem()) ;
}

QList<QPair<spec::value,QString> > exportDialog::dataFormat()
{
	QList<QPair<spec::value, QString> > returnData ;
	for (int i = 0 ; i < dataLayout->count()-1 ; i++)
	{
		exportFormatItem* pointer ;
		if (inLayout->itemAt(i) && (pointer = dynamic_cast<exportFormatItem*>(dataLayout->itemAt(i)->widget())))
			returnData << QPair<spec::value, QString>(pointer->value(), pointer->separator()) ;
	}
	return returnData ;
}

QList<QPair<bool,QString> > exportDialog::headerFormat()
{
	QList<QPair<bool,QString> > returnData ;
	for (int i = 0 ; i < inLayout->count()-1 ; i++)
	{
		exportLayoutItem *pointer ;
		if (inLayout->itemAt(i) && (pointer = dynamic_cast<exportLayoutItem*>(inLayout->itemAt(i)->widget())) )
			returnData << QPair<bool,QString>(pointer->isFreeText(),pointer->text()) ;
	}
	return returnData ;
}

exportDialog::~exportDialog()
{
}


