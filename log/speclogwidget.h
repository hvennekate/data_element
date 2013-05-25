#ifndef SPECLOGWIDGET_H
#define SPECLOGWIDGET_H

#include "specdockwidget.h"
#include "speclogview.h"

class QVBoxLayout ;

class specLogWidget : public specDockWidget, public specStreamable
{
	Q_OBJECT
private:
	specLogView* logView ;
	QVBoxLayout* layout ;
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
	type typeId() const { return specStreamable::logWidget ; }
public:
	explicit specLogWidget(QWidget *parent = 0);
	void addToolbar(specActionLibrary*) ;
	specView* view() { return logView ; }
};

#endif // SPECLOGWIDGET_H
