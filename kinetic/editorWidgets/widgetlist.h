#ifndef WIDGETLIST_H
#define WIDGETLIST_H

#include <QScrollArea>

namespace Ui {
	class widgetList;
}

class widgetList : public QScrollArea
{
	Q_OBJECT

public:
	explicit widgetList(QWidget *parent = 0);
	~widgetList();
	QWidgetList widgets() ;
	void setExtendable(bool on) ;
signals:
	void needWidget() ;
public slots:
	void addWidget(QWidget*) ;
private:
	Ui::widgetList *ui;
};

#endif // WIDGETLIST_H
