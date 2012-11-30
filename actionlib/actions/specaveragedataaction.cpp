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

specAverageDataAction::specAverageDataAction(QObject *parent) :
    specRequiresItemAction(parent)
{
	setIcon(QIcon(":/ave.png")) ;
	setToolTip(tr("Average Data")) ;
	setWhatsThis(tr("Smooth data by averaging.  You can choose between plainly averaging any number of data points or calculating a moving average."));
	setText(tr("Average data...")) ;
	setShortcut(tr("Ctrl+Shift+a"));
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

specUndoCommand* specAverageDataAction::generateUndoCommand()
{
	// TODO subclass QDialog, make one instance persistent in memory
	// also:  minimale Anzahl Punkte, wenn nicht laufend: 2, sonst 1
	QDialog dialog(parentWidget()) ;
	dialog.setWindowTitle(tr("Average")) ;
	dialog.setLayout(new QVBoxLayout(&dialog)) ;

    QRadioButton *byPointNumber = new QRadioButton(tr("Average by point"), &dialog) ;
    dialog.layout()->addWidget(byPointNumber);
    byPointNumber->setChecked(true) ;

	QSpinBox *number = new QSpinBox(&dialog) ;
	number->setMinimum(1) ;
	number->setSuffix(tr(" points")) ;
	number->setSpecialValueText(tr("1 point")) ;
	dialog.layout()->addWidget(number) ;
	QCheckBox *running = new QCheckBox(tr("running average"),&dialog) ;
	QLabel *runningLabel = new QLabel("Points will be taken symmetrically\nfrom both sides\n(if available)",&dialog) ;
	runningLabel->setVisible(running->isChecked()) ;
	dialog.layout()->addWidget(runningLabel);
	dialog.layout()->addWidget(running) ;
	connect(running,SIGNAL(toggled(bool)),runningLabel,SLOT(setVisible(bool))) ;

    QRadioButton *byTolerance = new QRadioButton(tr("Average by x value"), &dialog) ;
    dialog.layout()->addWidget(byTolerance);

    QLineEdit *toleranceEdit = new QLineEdit("0",&dialog) ;
    toleranceEdit->setValidator(new QDoubleValidator(0, INFINITY, 2, toleranceEdit)) ;
    QHBoxLayout *toleranceLayout = new QHBoxLayout ;
    toleranceLayout->addWidget(new QLabel(tr("Tolerance:"), &dialog)) ;
    toleranceLayout->addWidget(toleranceEdit) ;
    dialog.layout()->addItem(toleranceLayout) ;

    QCheckBox *rightToLeft = new QCheckBox(tr("Invert direction (start from larger x values)"),&dialog) ;
    dialog.layout()->addWidget(rightToLeft) ;

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,&dialog) ;
	dialog.layout()->addWidget(buttonBox);
	connect(buttonBox,SIGNAL(accepted()),&dialog,SLOT(accept())) ;
	connect(buttonBox,SIGNAL(rejected()),&dialog,SLOT(reject())) ;

	if (!dialog.exec())
		return 0 ;

	int numAverages = number->value() ;
	specMultiCommand *groupCommand = new specMultiCommand ;
    groupCommand->setParentObject(model) ;
    groupCommand->setMergeable(false) ;

    if (byPointNumber->isChecked())
    {
        foreach(QModelIndex index, selection)
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
        groupCommand->setText(tr("Average data (") + QString::number(number->value()) + tr(" points") +  (running->isChecked() ? tr(", running)") : tr(")"))) ;
    }
    else if (byTolerance->isChecked())
    {
        groupCommand->setText(tr("Averaged data with a tolerance of ") + toleranceEdit->text() + tr(".")) ;

        tolerantComparison comp(toleranceEdit->text().toDouble()) ;
        foreach(QModelIndex index, selection)
        {
            specDataItem *item = dynamic_cast<specDataItem*>(model->itemPointer(index)) ;
            if (!item) continue ;
            specExchangeDataCommand *command = new specExchangeDataCommand(groupCommand) ;
            command->setParentObject(model) ;
            QVector<specDataPoint> newData, oldData(item->allData()) ;
            if (rightToLeft->isChecked())
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
