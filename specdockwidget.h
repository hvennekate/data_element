#ifndef SPECDOCKWIDGET_H
#define SPECDOCKWIDGET_H

#include <QDockWidget>
#include <QString>
#include <QDataStream>

class specDockWidget : public QDockWidget
{
	Q_OBJECT
private:
	QString widgetTypeName ;
	bool changingTitle ;
protected:
	void changeEvent(QEvent *event) ;
public:
	explicit specDockWidget(QString type, QWidget *parent = 0);
};

#endif // SPECDOCKWIDGET_H
