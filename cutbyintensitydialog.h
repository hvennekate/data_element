#ifndef CUTBYINTENSITYDIALOG_H
#define CUTBYINTENSITYDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include "specplot.h"
#include "specmodelitem.h"

class cutByIntensityDialog : public QDialog
{
	Q_OBJECT
private:
	QVBoxLayout* layout ;
	specPlot* plot ;
	QPushButton* newRange, *deleteRange ;
	QList<specModelItem*> items ;
	QDialogButtonBox* buttons ;
	CanvasPicker* picker ;
	int huevalue ;
private slots:
	void addRange() ;
	void removeRange() ;
	void rangeModified(specCanvasItem* range, int point, double newX, double newY) ;
public:
	explicit cutByIntensityDialog(QWidget* parent = 0);
	~cutByIntensityDialog() ;
	void assignSpectra(QList<specModelItem*>) ;
	QList<specRange*> ranges() ;
};

#endif // CUTBYINTENSITYDIALOG_H
