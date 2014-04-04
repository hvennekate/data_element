#include "specmergedialog.h"
#include "ui_specmergedialog.h"
#include <QSet>
#include "names.h"
#include <QItemDelegate>
#include <QDoubleSpinBox>

class mergeModel::mergeModelPrivate
{
public:
	struct mergeItem
	{
		bool enabled ;
		bool numeric ;
		double tolerance ;
		QString name ;
		mergeItem()
			: enabled(false),
			  numeric(false),
			  tolerance(0)
		{}
		mergeItem(const QString& s, bool n)
			: enabled(false),
			  numeric(n),
			  tolerance(0),
			  name(s)
		{}
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
	if(index.column() == 1 && d->items[index.row()].numeric)
		return Qt::ItemIsEditable | Qt::ItemIsEnabled ;
	return Qt::NoItemFlags ;
	//	return f ;
}

QVariant mergeModel::data(const QModelIndex& index, int role) const
{
	if(role == Qt::CheckStateRole && index.column() == 0)
		return d->items[index.row()].enabled ? Qt::Checked : Qt::Unchecked ;
	if(role != Qt::DisplayRole)
		return QVariant() ;
	mergeModelPrivate::mergeItem item = d->items[index.row()] ;
	switch(index.column())
	{
		case 0:
			return item.name ;
		case 1:
			return item.numeric ? QVariant(item.tolerance) : QVariant() ;
		default:
			return QVariant() ;
	}
}

bool mergeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(index.column() == 0 && Qt::CheckStateRole == role)
	{
		d->items[index.row()].enabled = value.toBool() ;
		emit dataChanged(index, index) ;
		return true ;
	}
	if(index.column() == 1 && Qt::EditRole == role)
	{
		d->items[index.row()].tolerance = value.toDouble() ;
		emit dataChanged(index, index) ;
		return true ;
	}
	return false ;
}

QList<stringDoublePair> mergeModel::getMergeCriteria() const
{
	QList<stringDoublePair> result ;
	foreach(mergeModelPrivate::mergeItem item, d->items)
	if(item.enabled)
		result << stringDoublePair(item.name, item.tolerance) ;
	return result ;
}

void mergeModel::setDescriptors(const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties)
{
	beginResetModel();
	// Determine who's who...
	QStringList oldDesc ;
	foreach(const mergeModelPrivate::mergeItem & item, d->items)
	oldDesc << item.name ;
	QSet<QString> oldSet(oldDesc.toSet()), newSet(descriptors.toSet()) ;
	QSet<QString> newDescs(newSet - oldSet),
	     removedDescs(oldSet - newSet) ;
	// add
	foreach(const QString & s, newSet - oldSet)
	{
		d->items << mergeModelPrivate::mergeItem(s, descriptorProperties[descriptors.indexOf(s)] & spec::numeric) ;
		oldDesc << s;
	}
	// remove
	foreach(const QString & s, oldSet - newSet)
	{
		int i = oldDesc.indexOf(s) ;
		d->items.removeAt(i) ;
		oldDesc.removeAt(i);
	}
	// set attributes
	foreach(const QString & s, oldSet & newSet)
	d->items[oldDesc.indexOf(s)].numeric = descriptorProperties[descriptors.indexOf(s)] & spec::numeric ;
	// rearrange
	foreach(const QString & s, descriptors)
	{
		int oldIndex = oldDesc.indexOf(s),
		    newIndex = descriptors.indexOf(s) ;
		qSwap(d->items[oldIndex], d->items[newIndex]) ;
		qSwap(oldDesc[oldIndex], oldDesc[newIndex]) ;
	}
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

void specMergeDialog::setDescriptors(const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties)
{
	mm->setDescriptors(descriptors, descriptorProperties);
	ui->criteria->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
	ui->criteria->resizeColumnsToContents();
	ui->criteria->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	ui->criteria->setFixedWidth(ui->criteria->sizeHint().width() + 15);  // TODO bad!
}

void specMergeDialog::getMergeCriteria(QList<stringDoublePair>& toCompare, bool& doSpectralAdaptation, bool& sortBeforeMerge) const
{
	doSpectralAdaptation = ui->alignment->isChecked() ;
	sortBeforeMerge = ui->listingOrder->isChecked() ;
	toCompare = mm->getMergeCriteria() ;
}

specMergeDialog::~specMergeDialog()
{
	delete ui;
}
