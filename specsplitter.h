#ifndef SPECSPLITTER_H
#define SPECSPLITTER_H

#include <QSplitter>

class specSplitterHandle : public QSplitterHandle
{
    Q_OBJECT
private:
    void mousePressEvent(QMouseEvent *) ;
public:
    specSplitterHandle(Qt::Orientation orientation, QSplitter * parent) ;
};

class specSplitter : public QSplitter
{
	Q_OBJECT
public:
	explicit specSplitter(Qt::Orientation o, QWidget *parent = 0);
	explicit specSplitter(QWidget *parent= 0) ;

private:
    QSplitterHandle* createHandle() ;
};

#endif // SPECSPLITTER_H
