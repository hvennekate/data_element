#include "specaverageneighboraction.h"
#include "specdataview.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include "utility-functions.h"

specAverageNeighborAction::specAverageNeighborAction(QObject *parent) :
	specRequiresItemAction(parent)
{
	setIcon(QIcon(":/averageNeighbor.png")) ;
	setToolTip(tr("Average proximal data points")) ;
	setWhatsThis(tr("Averages data points if their x-value is approximately equal.")) ;
	setText(tr("Average by proximity...")) ;
	setShortcut(tr("Ctrl+Shift+a")) ;
}

const std::type_info& specAverageNeighborAction::possibleParent()
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

specUndoCommand* specAverageNeighborAction::generateUndoCommand()
{
	QDialog dialog ;
	dialog.setLayout(new QVBoxLayout);

	QLineEdit *toleranceEdit = new QLineEdit("0",&dialog) ;
	toleranceEdit->setValidator(new QDoubleValidator(0, INFINITY, 2, toleranceEdit)) ;
	QHBoxLayout *toleranceLayout = new QHBoxLayout ;
	toleranceLayout->addWidget(new QLabel(tr("Tolerance:"), &dialog)) ;
	toleranceLayout->addWidget(toleranceEdit) ;
	dialog.layout()->addItem(toleranceLayout) ;

	QCheckBox *rightToLeft = new QCheckBox(tr("Invert direction (start from larger x values)"),&dialog) ;
	dialog.layout()->addWidget(rightToLeft) ;

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
							 Qt::Horizontal,
							 &dialog) ;
	dialog.layout()->addWidget(buttons) ;
	connect(buttons, SIGNAL(accepted()), &dialog, SLOT(accept())) ;
	connect(buttons, SIGNAL(rejected()), &dialog, SLOT(reject())) ;

	if (QDialog::Rejected == dialog.exec()) return 0 ;

	specMultiCommand *groupCommand = new specMultiCommand ;
	groupCommand->setParentObject(model) ;
	groupCommand->setMergeable(false) ;
	groupCommand->setText(tr("Averaged data with a tolerance of ") + toleranceEdit->text() + tr(".")) ;

	tolerantComparison comp(toleranceEdit->text().toDouble()) ;
	foreach(QModelIndex index, selection)
	{
		specDataItem *item = dynamic_cast<specDataItem*>(model->itemPointer(index)) ;
		if (!item) continue ;
		specExchangeDataCommand *command = new specExchangeDataCommand(groupCommand) ;
		command->setParentObject(model) ;
		QVector<specDataPoint> newData, oldData = item->getDataExcept(QList<specRange*>()) ;
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

	if (!groupCommand->childCount())
	{
		delete groupCommand ;
		return 0 ;
	}

	return groupCommand ;
}
