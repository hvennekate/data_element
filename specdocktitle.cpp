#include "specdocktitle.h"
#include <QTextStream>

specDockTitle::specDockTitle(QWidget* parent)
 : QWidget(parent)
{
	layout   = new QHBoxLayout ;
	close    = new QPushButton(QIcon(":/close_dock.png"),"") ;
	title    = new QLabel("test") ;
	floating = new QPushButton(QIcon(":/float_dock.png"),"") ;
	close->setFixedSize(11,11) ;
	floating->setFixedSize(11,11) ;
	title->setFixedHeight(11) ;
// 	setFixedHeight(30) ;
	close->setFlat(true) ;
	floating->setFlat(true) ;
	layout->setSpacing(4) ;
	layout->addWidget(title) ;
	layout->insertStretch(1);
	layout->addWidget(floating) ;
// 	layout->insertSpacing(3,4) ;
	layout->addWidget(close) ;
	setLayout(layout) ;
}


specDockTitle::~specDockTitle()
{
}


