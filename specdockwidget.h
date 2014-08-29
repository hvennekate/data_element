#ifndef SPECDOCKWIDGET_H
#define SPECDOCKWIDGET_H

#include <QDockWidget>
#include <QString>
#include <QDataStream>
#include "specstreamable.h"
#include <QMap>
#include <QItemSelection>

class specActionLibrary ;
class specPlot ;

class specDockWidget : public QDockWidget
{
	Q_OBJECT
private:
	QString widgetTypeName ;
	QMap<specStreamable::type, int> selectedTypes ;
	specPlot* Plot ;
	bool changingTitle ;
private slots:
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) ;
//	void currentChanged(const QModelIndex& current, const QModelIndex& previous) ;
protected:
	void changeEvent(QEvent* event) ;
	virtual QList<QWidget*> mainWidgets() const = 0 ;
public:
	void setupWindow(specActionLibrary* actions) ;
	explicit specDockWidget(QString type, QWidget* parent = 0, bool floating = true);
};

#endif // SPECDOCKWIDGET_H
