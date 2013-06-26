#ifndef SPECDOCKWIDGET_H
#define SPECDOCKWIDGET_H

#include <QDockWidget>
#include <QString>
#include <QDataStream>

class specActionLibrary ;

class specDockWidget : public QDockWidget
{
	Q_OBJECT
private:
	QString widgetTypeName ;
	bool changingTitle ;
protected:
	void changeEvent(QEvent *event) ;
	virtual QList<QWidget*> mainWidgets() const = 0 ;
public:
	void setupWindow(specActionLibrary* actions) ;
	explicit specDockWidget(QString type, QWidget *parent = 0, bool floating = true);
};

#endif // SPECDOCKWIDGET_H
