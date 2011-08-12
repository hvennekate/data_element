#ifndef SPECDOCKTITLE_H
#define SPECDOCKTITLE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class specDockTitle : public QWidget
{
Q_OBJECT
private:
	QHBoxLayout* layout ;
	QPushButton* close, *floating;
	QLabel* title ;
public:
    specDockTitle(QWidget* parent = 0);

    ~specDockTitle();

};

#endif
