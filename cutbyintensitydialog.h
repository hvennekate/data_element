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
	QVBoxLayout *layout ;
	specPlot *plot ;
	QPushButton *newRange, *deleteRange ;
	QList<QwtPlotCurve*> intensities ;
	QList<specModelItem*> items ;
	QDialogButtonBox *buttons ;
	int huevalue ;
private slots:
	void addRange() ;
public:
	explicit cutByIntensityDialog(QWidget *parent = 0);
	~cutByIntensityDialog() ;
	void assignSpectra(QList<specModelItem*>) ;
public slots :
	void performDeletion() ;

signals:

public slots:

};

#endif // CUTBYINTENSITYDIALOG_H
