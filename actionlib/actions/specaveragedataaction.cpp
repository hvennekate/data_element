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
#include "specdataview.h"
#include "utility-functions.h"
#include <QRadioButton>
#include "ui_averagedialog.h"

specAverageDataAction::specAverageDataAction(QObject *parent) :
	specRequiresItemAction(parent),
	dialog(new QDialog),
	ui(new Ui::averageDialog)
{
	setIcon(QIcon(":/ave.png")) ;
	setToolTip(tr("Average Data")) ;
	setWhatsThis(tr("Smooth data by averaging.  You can choose between plainly averaging any number of data points or calculating a moving average."));
	setText(tr("Average data...")) ;
	setShortcut(tr("Ctrl+Shift+a"));
	ui->setupUi(dialog);
	connect(ui->running, SIGNAL(toggled(bool)), this, SLOT(runningToggled())) ;
	ui->toleranceEdit->setMaximum(INFINITY);
}

specAverageDataAction::~specAverageDataAction()
{
	delete ui ;
	delete dialog ;
}

const std::type_info& specAverageDataAction::possibleParent()
{
	return typeid(specDataView) ;
}

class tolerantComparison
{
private:
	double tolerance ;
public:
	tolerantComparison(double d) : tolerance(d) {}
	bool operator()(const specDataPoint& a, const specDataPoint& b)
	{
		return fabs(a.nu - b.nu) <= tolerance ;
	}
};

void specAverageDataAction::runningToggled()
{
	ui->number->setMinimum(ui->running->isChecked() ? 1 : 2);
}

specUndoCommand* specAverageDataAction::generateUndoCommand()
{
	if (!dialog->exec())
		return 0 ;

	int numAverages = ui->number->value() ;
	if (ui->byPointNumber->isChecked() && numAverages == 1 && !ui->running->isChecked()) return 0 ;
	specMultiCommand *groupCommand = new specMultiCommand ;
	groupCommand->setParentObject(model) ;
	groupCommand->setMergeable(false) ;

	if (ui->byPointNumber->isChecked())
	{
		foreach(QModelIndex index, selection)
		{
			specDataItem *item = dynamic_cast<specDataItem*>(view->model()->itemPointer(index)) ;
			if (!item) continue ;
			QVector<specDataPoint> oldData(item->allData()), newData ;
			item->applyCorrection(oldData);
			if (ui->running->isChecked())
			{
				for (int i = 0 ; i < oldData.size() ; ++i)
				{
					int limit = qMin(i+numAverages+1, oldData.size()) ;
					specDataPoint dataPoint ;
					for (int j = qMax(0,i-numAverages) ; j < limit ; ++j)
						dataPoint += oldData[j] ;
					dataPoint /= limit - qMax(0,i-numAverages) ;
					newData << dataPoint ;
				}
			}
			else
			{
				int avs = std::ceil(double(oldData.size())/numAverages) ;
				for (int i = 0 ; i < avs ; ++i)
				{
					specDataPoint dataPoint ;
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
		groupCommand->setText(tr("Average data (")
				      + QString::number(ui->number->value())
				      + tr(" points")
				      +  (ui->running->isChecked() ? tr(", running)") : tr(")"))) ;
	}
	else if (ui->byTolerance->isChecked())
	{
		groupCommand->setText(tr("Averaged data with a tolerance of ")
				      + ui->toleranceEdit->text()
				      + (ui->rightToLeft->isChecked() ? tr(", inverse x direction") : tr("."))) ;

		tolerantComparison comp(ui->toleranceEdit->text().toDouble()) ;
		foreach(QModelIndex index, selection)
		{
			specDataItem *item = dynamic_cast<specDataItem*>(model->itemPointer(index)) ;
			if (!item) continue ;
			specExchangeDataCommand *command = new specExchangeDataCommand(groupCommand) ;
			command->setParentObject(model) ;
			QVector<specDataPoint> newData, oldData(item->allData()) ;
			if (ui->rightToLeft->isChecked())
			{
				newData.reserve(oldData.size());
				std::reverse_copy(oldData.begin(), oldData.end(), std::back_inserter(newData)) ;
				oldData.clear();
				oldData.swap(newData) ;
			}
			averageToNew(oldData.begin(), oldData.end(), comp, std::back_inserter(newData)) ;
			command->setItem(index, newData) ;
		}
	}

	if (!groupCommand->childCount())
	{
		delete groupCommand ;
		return 0 ;
	}

	return groupCommand ;
}
