#ifndef SPECLOGWIDGET_H
#define SPECLOGWIDGET_H

#include <QDockWidget>
#include "speclogview.h"

class QVBoxLayout ;

class specLogWidget : public QDockWidget
{
	Q_OBJECT
private:
	specLogView* view ;
	QVBoxLayout* layout ;
public:
	explicit specLogWidget(specModel*, QWidget *parent = 0);
	void addToolbar(specActionLibrary*) ;
	void write(specOutStream&) ;
	bool read(specInStream&) ;
};

#endif // SPECLOGWIDGET_H
