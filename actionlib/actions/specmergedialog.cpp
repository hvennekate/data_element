#include "specmergedialog.h"
#include "ui_specmergedialog.h"
#include <QSet>
#include "names.h"
#include <QItemDelegate>
#include <QDoubleSpinBox>

class mergeModel::mergeModelPrivate
{
public:
	struct mergeItem : specDescriptorComparisonCriterion
	{
		bool enabled ;
	public:
		mergeItem(const QString& s = QString())
			: specDescriptorComparisonCriterion(s),
			  enabled(false)
		{}
		bool isEnabled() const { return enabled ; }
		void enable(bool a = true) { enabled = a ; }
	};

	QList<mergeItem> items ;
};

mergeModel::mergeModel(QObject* parent)
	: QAbstractTableModel(parent),
	  d(new mergeModelPrivate)
{}

mergeModel::~mergeModel()
{
	delete d ;
}

int mergeModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return d->items.size() ;
}

int mergeModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 2 ;
}

QVariant mergeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole) return QVariant() ;
	if(orientation != Qt::Horizontal) return QVariant() ;
	switch(section)
	{
		case 0:
			return tr("Matching criterion") ;
		case 1:
			return tr("Tolerance") ;
		default:
			return QVariant() ;
	}
}

Qt::ItemFlags mergeModel::flags(const QModelIndex& index) const
{
	//	Qt::ItemFlags f = QAbstractTableModel::flags(index) ;
	if(index.column() == 0) return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
	if(index.column() == 1)	return Qt::ItemIsEditable | Qt::ItemIsEnabled ; // TODO checkable?
	return Qt::NoItemFlags ;
	//	return f ;
}

QVariant mergeModel::data(const QModelIndex& index, int role) const
{
	if(role == Qt::CheckStateRole && index.column() == 0)
		return d->items[index.row()].isEnabled() ? Qt::Checked : Qt::Unchecked ;
	if(role != Qt::DisplayRole)
		return QVariant() ;
	mergeModelPrivate::mergeItem item = d->items[index.row()] ;
	switch(index.column())
	{
		case 0:
			return item.descriptor();
		case 1:
			return QVariant(item.tolerance()) ;
		default:
			return QVariant() ;
	}
}

bool mergeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(index.column() == 0 && Qt::CheckStateRole == role)
	{
		d->items[index.row()].enable(value.toBool()) ;
		emit dataChanged(index, index) ;
		return true ;
	}
	if(index.column() == 1 && Qt::EditRole == role)
	{
		d->items[index.row()].setTolerance(value.toDouble()) ;
		emit dataChanged(index, index) ;
		return true ;
	}
	return false ;
}

specDescriptorComparisonCriterion::container mergeModel::getMergeCriteria() const
{
	specDescriptorComparisonCriterion::container result ;
	foreach(mergeModelPrivate::mergeItem item, d->items)
		if(item.isEnabled())
			result << item ;
	return result ;
}

void mergeModel::setDescriptors(const QStringList& descriptors)
{
	beginResetModel();

	// create new list
	QList<mergeModelPrivate::mergeItem> newItems ;
	foreach (const QString& descriptor, descriptors)
	{
		bool old = false ;
		foreach(mergeModelPrivate::mergeItem item, d->items)
		{
			old = (item.descriptor() == descriptor) ;
			if (old)
			{
				newItems << item ;
				break ;
			}
		}
		if (!old)
			newItems << mergeModelPrivate::mergeItem(descriptor) ;
	}
	d->items.swap(newItems) ;
	endResetModel();
}

mergeDelegate::mergeDelegate(QObject* parent)
	: QItemDelegate(parent)
{}

QWidget* mergeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	QDoubleSpinBox* spinBox = new QDoubleSpinBox(parent) ;
	spinBox->setMinimum(0);
	spinBox->setDecimals(4);
	return spinBox ;
}

void mergeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*> (editor) ;
	if(!spinBox) return ;
	model->setData(index, spinBox->value()) ;
}

void mergeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*> (editor) ;
	if(!spinBox) return ;
	spinBox->setValue(index.data().toDouble());
}

void mergeDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

specMergeDialog::specMergeDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::specMergeDialog),
	mm(new mergeModel(this))

{
	ui->setupUi(this);
	ui->criteria->setModel(mm) ;
	ui->criteria->setItemDelegateForColumn(1, new mergeDelegate(ui->criteria));
}

void specMergeDialog::setDescriptors(const QStringList& descriptors)
{
	mm->setDescriptors(descriptors);
	ui->criteria->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	ui->criteria->resizeColumnsToContents();
	ui->criteria->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->criteria->setFixedWidth(ui->criteria->sizeHint().width() + 15);  // TODO bad!
}

void specMergeDialog::getMergeCriteria(specDescriptorComparisonCriterion::container& toCompare, spec::correctionMode& spectralAdaptation) const
{
	spectralAdaptation = spec::noCorrection ;
	if (ui->useOffset->isChecked())
	{
		spectralAdaptation = spec::offset ;
		if (ui->useSlope->isChecked())
			spectralAdaptation = spec::offsetAndSlope ;
	}
	toCompare = mm->getMergeCriteria() ;
}

specMergeDialog::~specMergeDialog()
{
	delete ui;
}
