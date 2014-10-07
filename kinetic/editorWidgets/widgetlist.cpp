#include "widgetlist.h"
#include "ui_widgetlist.h"
#include <QScrollBar>
#include <QDebug>

widgetList::widgetList(QWidget *parent) :
	QScrollArea(parent),
	ui(new Ui::widgetList)
{
	ui->setupUi(this);
	connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(needWidget())) ;
}

widgetList::~widgetList()
{
	delete ui;
}

QWidgetList widgetList::widgets()
{
	QWidgetList wl ;
	for (int i = 0 ; i < ui->verticalLayout->indexOf(ui->line) ; ++i)
		wl << ui->verticalLayout->itemAt(i)->widget() ;
	return wl ;
}

void widgetList::setExtendable(bool on)
{
	ui->line->setVisible(on) ;
	ui->pushButton->setVisible(on) ;
}

void widgetList::addWidget(QWidget *widget)
{
	widget->setParent(this) ;
	ui->verticalLayout->insertWidget(ui->verticalLayout->indexOf(ui->line), widget);
}
