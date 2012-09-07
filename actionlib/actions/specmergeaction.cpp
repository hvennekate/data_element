#include "specmergeaction.h"
#include "specdeletecommand.h"
#include "specaddfoldercommand.h"
#include "specmulticommand.h"
#include "specspectrumplot.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include "names.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QScrollArea>
#include <QLabel>

specMergeAction::specMergeAction(QObject *parent)
	: specUndoAction(parent)
{
	setIcon(QIcon(":/merge.png")) ;
	setToolTip(tr("Merge items")) ;
	setWhatsThis(tr("Merge selected data items.  You may define criteria and processing options for merging."));
}

void specMergeAction::execute()
{
	specView *view = (specView*) parentWidget() ;
	specModel *model = view->model() ;
	QModelIndexList indexes = view->getSelection() ;
	qSort(indexes) ;
	QList<specDataItem*> items ;
	foreach(QModelIndex index, indexes)
		 items << dynamic_cast<specDataItem*>(model->itemPointer(index)) ;
	items.removeAll(0) ;
	if (items.isEmpty()) return ;

	// let user define similarities
	QList<QPair<QStringList::size_type, double> > criteria ;
	bool spectralAdaptation ;
	if (!getMergeCriteria(criteria, model->descriptors(), model->descriptorProperties(),spectralAdaptation)) return ;

	QVector<specModelItem*> toBeDeleted ;
	QVector<specModelItem*> newlyInserted ;

	// Create and insert new merged items
	while (!items.isEmpty())
	{
		// form chunk -> only merge if same parent! (new logic)
		QList<specDataItem*> chunk ;
		specFolderItem * const parent = items.first()->parent() ;
		const int row = parent->childNo(items.first()) ;
		while (!items.isEmpty() && items.first()->parent() == parent && parent->childNo(items.first()) == row + chunk.size())
			chunk << items.takeFirst() ;

		QVector<specModelItem*> toInsert ;
		while (!chunk.isEmpty())
		{
			specDataItem* item = chunk.takeFirst() ;
			specDataItem *newItem = new specDataItem(QVector<specDataPoint>(),QHash<QString,specDescriptor>()) ;
			*newItem += *item ; // TODO ueberarbeiten
			toBeDeleted << item ; // TODO check possibility of immediate deletion
			QList<specModelItem*> toMergeWith ;
			// look for items to merge with
			for (int i = 0 ; i < chunk.size() ; ++i)
			{
				if (itemsAreEqual(newItem,chunk[i],criteria, model->descriptors(), model->descriptorProperties()))
				{
					specDataItem *other = dynamic_cast<specDataItem*>(chunk[i]) ;
					if (!other)
					{
						chunk.takeAt(i--) ;
						continue ;
					}
					toMergeWith << other ;
					toBeDeleted << chunk.takeAt(i--) ;
				}
			}
			// if there are others, do the merge
			if (!toMergeWith.isEmpty())
			{
				if (spectralAdaptation)
				{
					foreach(specModelItem* other,toMergeWith)
					{
						// generate reference spectrum
						QMap<double,double> reference ;
						newItem->revalidate();
						const QwtSeriesData<QPointF>* spectrum = newItem->QwtPlotCurve::data() ;
						for (size_t i = 0 ; i < spectrum->size() ; ++i)
						{
							const QPointF point = spectrum->sample(i) ;
							reference[point.x()] = point.y() ;
						}

						// define spectral ranges
						QwtPlotItemList ranges ;
						ranges << new specRange(reference.begin().key(), (reference.end() -1).key()) ; // DANGER
						// TODO set up sensible ranges to account for "holes" in the spectrum

						// generate list of spectra
						// TODO generate this list directly above
						QwtPlotItemList spectra ;
						for (QList<specModelItem*>::iterator i = toMergeWith.begin() ; i != toMergeWith.end() ; ++i)
							spectra << (QwtPlotItem*) *i ;

						// perform spectral adaptation
						specMultiCommand *correctionCommand= specSpectrumPlot::generateCorrectionCommand(ranges, QwtPlotItemList() << (QwtPlotItem*) other, reference, view) ;
						correctionCommand->redo();
						*newItem += *((specDataItem*) other) ;
						correctionCommand->undo();
						delete correctionCommand ;
					}
				}
				else
					foreach(specModelItem* other, toMergeWith)
						*newItem += *((specDataItem*) other) ; // TODO check this cast
				newItem->flatten();
			}
			toInsert << newItem ; // this creates overhead if  there are many items not to be merged...
		}
		if (model->insertItems(toInsert.toList(), model->index(parent), row))
			newlyInserted += toInsert ; // TODO else...
	}

	specMultiCommand *command = new specMultiCommand ;
	command->setParentObject(view) ;
	command->setMergeable(false) ;

	// preparing insertion command
	specAddFolderCommand *insertionCommand = new specAddFolderCommand(command) ;
	QModelIndexList insertList = model->indexList(newlyInserted.toList()) ;
	insertionCommand->setItems(insertList) ;
	insertionCommand->setParentObject(view->model()) ;

	// prepare to delete the old items
	specDeleteCommand *deletionCommand = new specDeleteCommand(command) ;
	QModelIndexList deleteList = model->indexList(toBeDeleted.toList()) ;
	deletionCommand->setItems(deleteList) ;
	deletionCommand->setParentObject(view->model()) ;

	library->push(command) ;
}


bool specMergeAction::getMergeCriteria(QList<QPair<QStringList::size_type, double> >& toCompare, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties, bool& doSpectralAdaptation)
{ // TODO separate class
	QDialog *descriptorMatch = new QDialog() ;
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel) ;
	connect(buttons,SIGNAL(accepted()), descriptorMatch, SLOT(accept()));
	connect(buttons,SIGNAL(rejected()), descriptorMatch, SLOT(reject()));
	QGridLayout *descriptorLayout = new QGridLayout ;
	QVBoxLayout *dialogLayout = new QVBoxLayout ;
	dialogLayout->addWidget(new QLabel(tr("Criteria selected below will be used\n (possibly including some numerical tolerance, if given) to determine\n whether items match and need to be merged."),descriptorMatch)) ;
	//TODO Label fuer tolerance ...
	descriptorLayout->addWidget(new QLabel(tr("To merge, ... must match"),descriptorMatch),0,0) ;
	bool hasNumeric = false ;
	for(QStringList::size_type i = 0 ; i < descriptors.size() ; i++)
	{
		descriptorLayout->addWidget(new QCheckBox(descriptors[i]),i+1,0) ;
		if(descriptorProperties[i] & spec::numeric)
		{
			QLineEdit *validatorLine = new QLineEdit("0") ;
			validatorLine->setValidator(new QDoubleValidator(validatorLine)) ;
			((QDoubleValidator*) validatorLine->validator())->setBottom(0) ;
			descriptorLayout->addWidget(validatorLine,i+1,1) ;
			hasNumeric = true ;
		}
	}
	if (hasNumeric)
		descriptorLayout->addWidget(new QLabel("Numerical tolerance",descriptorMatch),0,1);
	QScrollArea *scrollArea = new QScrollArea ;
	QWidget *areaWidget = new QWidget ;
	QCheckBox *spectralAdaptation = new QCheckBox(tr("Try to align merged data sets\n using offset/slope correction"),descriptorMatch) ;
	areaWidget->setLayout(descriptorLayout) ;
	scrollArea->setWidget(areaWidget) ;
	dialogLayout->addWidget(scrollArea) ;
	dialogLayout->addWidget(spectralAdaptation);
	dialogLayout->addWidget(buttons) ;
	descriptorMatch->setLayout(dialogLayout) ;
	bool retval = descriptorMatch->exec() ;

// 	QList<QPair<QStringList::size_type, double> > toCompare ;
	for (int i = 1 ; i < descriptorLayout->count() ; i++)
	{
		int row, column, rowspan, columnspan ;
		descriptorLayout->getItemPosition(i, &row, &column, &rowspan, &columnspan) ;
		if (column == 0 && ((QCheckBox*) descriptorLayout->itemAt(i)->widget())->checkState() == Qt::Checked)
			toCompare << qMakePair(row-1,
				descriptorProperties[row] & spec::numeric ?
						((QLineEdit*) descriptorLayout->itemAt(i+1)->widget())->text().toDouble() : 0.) ;
	}

	doSpectralAdaptation = spectralAdaptation->checkState() ;
	delete descriptorMatch ;
	return retval ;
}


bool specMergeAction::itemsAreEqual(specModelItem* first, specModelItem* second, const QList<QPair<QStringList::size_type, double> >& criteria, const QStringList& descriptors, const QList<spec::descriptorFlags>& descriptorProperties) // TODO insert actual strings into merge criteria
{
	if (!first || !second)
		return false ;
	// TODO may need to be revised if descriptor contains actual numeric value, not QString
	bool equal = true ;
	for(QList<QPair<QStringList::size_type, double> >::size_type i = 0 ; i < criteria.size() ; i++)
	{
		QStringList::size_type descriptor = criteria[i].first ;
		double tolerance = criteria[i].second ;
		if (descriptorProperties[descriptor] & spec::numeric)
		{
			double a = first->descriptor(descriptors[descriptor]).toDouble(),
				b = second->descriptor(descriptors[descriptor]).toDouble() ;
			equal = equal && b-tolerance <= a && a <= b+tolerance ;
		}
		else
			equal = equal && first->descriptor(descriptors[descriptor]) ==
				second->descriptor(descriptors[descriptor]) ;
	}
	return equal ;
}
