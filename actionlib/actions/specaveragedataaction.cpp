#include "specaveragedataaction.h"
#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QHBoxLayout>
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include <cmath>

specAverageDataAction::specAverageDataAction(QObject *parent) :
    specUndoAction(parent)
{
	setIcon(QIcon(":/ave.png")) ;
	setToolTip(tr("Average Data")) ;
	setWhatsThis(tr("Smooth data by averaging.  You can choose between plainly averaging any number of data points or calculating a moving average."));
}

void specAverageDataAction::execute()
{
	// TODO subclass QDialog, make one instance persistent in memory
	// also:  minimale Anzahl Punkte, wenn nicht laufend: 2, sonst 1
	QDialog dialog(parentWidget()) ;
	dialog.setWindowTitle("Mitteln") ;
	dialog.setLayout(new QVBoxLayout(&dialog)) ;
	QSpinBox *number = new QSpinBox(&dialog) ;
	number->setMinimum(1) ;
	number->setSuffix(" Punkte") ;
	number->setSpecialValueText("1 Punkt") ;
	dialog.layout()->addWidget(number) ;
	QCheckBox *running = new QCheckBox("Laufend mitteln",&dialog) ;
	QLabel *runningLabel = new QLabel("Punkte symmetrisch zu beiden Seiten\n(wenn vorhanden)",&dialog) ;
	runningLabel->setVisible(running->isChecked()) ;
	dialog.layout()->addWidget(runningLabel);
	dialog.layout()->addWidget(running) ;
	connect(running,SIGNAL(toggled(bool)),runningLabel,SLOT(setVisible(bool))) ;

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,&dialog) ;
	dialog.layout()->addWidget(buttonBox);
	connect(buttonBox,SIGNAL(accepted()),&dialog,SLOT(accept())) ;
	connect(buttonBox,SIGNAL(rejected()),&dialog,SLOT(reject())) ;

	if (!dialog.exec())
		return ;

	specDataView *view = (specDataView*) parentWidget() ;
	QModelIndexList indexes = view->getSelection() ;
	int numAverages = number->value() ;
	specMultiCommand *groupCommand = new specMultiCommand ;
	groupCommand->setParentObject(view) ;
	groupCommand->setMergeable(false) ; // TODO consider dataExchange command (also to avoid frequent replots)
//	library->push(groupCommand);
	foreach(QModelIndex index, indexes)
	{
		specDataItem *item = dynamic_cast<specDataItem*>(view->model()->itemPointer(index)) ;
		if (!item) continue ;
		QVector<specDataPoint> oldData(item->allData()), newData ;
		item->applyCorrection(oldData);
		if (running->isChecked())
		{
//			newData.resize(oldData.size());
			for (int i = 0 ; i < oldData.size() ; ++i)
			{
				int limit = qMin(i+numAverages+1, oldData.size()) ;
				specDataPoint dataPoint ; //= newData[i];
				for (int j = qMax(0,i-numAverages) ; j < limit ; ++j)
					dataPoint += oldData[j] ;
				dataPoint /= limit - qMax(0,i-numAverages) ;
				newData << dataPoint ;
			}
		}
		else
		{
//			newData.resize(oldData.size()/numAverages+1) ;
			int avs = std::ceil(double(oldData.size())/numAverages) ;
			for (int i = 0 ; i < avs ; ++i)
			{
				specDataPoint dataPoint ; //= newData[i] ;
				int limit = qMin(oldData.size(), (i+1)*numAverages) ;
				for (int j = i*numAverages ; j < limit ; ++ j)
					dataPoint += oldData[j] ;
				dataPoint /= limit - i*numAverages ;
				newData << dataPoint ;
			}
		}
		item->reverseCorrection(newData);
		specExchangeDataCommand *command = new specExchangeDataCommand(groupCommand) ;
		command->setParentObject(view->model()) ;
		command->setItem(index,newData);
	}
	groupCommand->setText(tr("Average data")) ;
	library->push(groupCommand);
}
