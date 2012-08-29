#ifndef SPECSPLITTER_H
#define SPECSPLITTER_H

#include <QSplitter>

class specSplitter : public QSplitter
{
	Q_OBJECT
public:
	explicit specSplitter(Qt::Orientation o, QWidget *parent = 0);
	explicit specSplitter(QWidget *parent= 0) ;

private:
	void contextMenuEvent(QContextMenuEvent *);
};

#endif // SPECSPLITTER_H
