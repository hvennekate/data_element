#include "specactionlibrary.h"
#include "specdeleteaction.h"
#include "specaddfolderaction.h"
#include "specmovecommand.h"
#include "speccopyaction.h"
#include "specpasteaction.h"
#include "speccutaction.h"
#include "changeplotstyleaction.h"
#include "spectreeaction.h"
#include "specmergeaction.h"
#include "specremovedataaction.h"
#include "specaveragedataaction.h"
#include "specaddsvgitem.h"
#include "specnewmetaitemaction.h"
#include "kinetic/specmetaview.h"
#include "specaddconnectionsaction.h"
#include "log/speclogview.h"
#include "specimportspecaction.h"
#include "names.h"
#include "speclabelaction.h"
#include <QUndoView>
#include "utility-functions.h"
#include "genericexportaction.h"
#include "specitempropertiesaction.h"
#include <QClipboard>
#include <QApplication>
#include "specaddfitaction.h"
#include "specconductfitaction.h"
#include "specremovefitaction.h"
#include "spectogglefitstyleaction.h"
#include "specmetaitem.h"
#include "specselectconnectedaction.h"
#include "specsetmultilineaction.h"
#include "specsvgitem.h"


QUndoView* specActionLibrary::undoView()
{
	return new QUndoView(undoStack) ;
}

specActionLibrary::specActionLibrary(QObject *parent) :
    QObject(parent)
{
	undoStack = new QUndoStack ;
    connect(undoStack,SIGNAL(cleanChanged(bool)),this,SIGNAL(stackClean(bool))) ;
}

void specActionLibrary::push(specUndoCommand * cmd)
{
	if (!cmd) return ;
	undoStack->push(cmd) ;
}

template<class toolMenu>
void specActionLibrary::addNewAction(toolMenu *bar, specUndoAction *action)
{
	action->setLibrary(this) ;
	bar->addAction(action) ;
}

QAction* specActionLibrary::redoAction(QObject* target)
{
	QAction *redoAction = undoStack->createRedoAction(target) ;
	redoAction->setIcon(QIcon::fromTheme("edit-redo")) ;
	return redoAction ;
}

QAction* specActionLibrary::undoAction(QObject* target)
{
	QAction *undoAction = undoStack->createUndoAction(target) ;
	undoAction->setIcon(QIcon::fromTheme("edit-undo"));
	return undoAction ;
}

QToolBar* specActionLibrary::toolBar(QWidget *target)
{
	addParent(target) ;
	specView* view ;
	if ((view = dynamic_cast<specView*>(target)) && view->model())
		addParent(view->model()) ;

	if (typeid(*target)== typeid(specDataView))
	{
		QToolBar *bar = new QToolBar(target) ;

		addNewAction(bar, new specAddFolderAction(target)) ;
		addNewAction(bar, new specAddSVGItemAction(target)) ;
		specImportSpecAction *importAction = new specImportSpecAction(target) ;
		importAction->setAcceptableImportFunctions(QList<QList<specModelItem *> (*)(QFile &)>() << readHVFile << readPEFile << readJCAMPFile);
		addNewAction(bar, importAction) ;
		bar->addSeparator() ;
		addNewAction(bar, new specCopyAction(target)) ;
		addNewAction(bar, new specCutAction(target)) ;
		addNewAction(bar, new specPasteAction(target)) ;
		addNewAction(bar, new specDeleteAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specTreeAction(target)) ;
		addNewAction(bar, new specMergeAction(target)) ;
		addNewAction(bar,new genericExportAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specRemoveDataAction(target)) ;
		addNewAction(bar, new specAverageDataAction(target)) ;
		addNewAction(bar, new changePlotStyleAction(target)) ;

		return bar ;
	}
	else if (dynamic_cast<specMetaView*>(target))
	{
		QToolBar *bar = new QToolBar(target) ;

		addNewAction(bar, new specAddFolderAction(target)) ;
		addNewAction(bar, new specNewMetaItemAction(target));
		addNewAction(bar, new specAddSVGItemAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new specCopyAction(target)) ;
		addNewAction(bar, new specCutAction(target)) ;
		addNewAction(bar, new specPasteAction(target)) ;
		addNewAction(bar, new specDeleteAction(target)) ;
		bar->addSeparator() ;
		addNewAction(bar, new changePlotStyleAction(target)) ;
		addNewAction(bar, new specAddConnectionsAction(target)) ;
		addNewAction(bar, new specSelectConnectedAction(target)) ;
		addNewAction(bar,new genericExportAction(target)) ;

		return bar ;
	}
	else if (dynamic_cast<specLogView*>(target))
	{
		QToolBar *bar = new QToolBar(target) ;

		addNewAction(bar, new specAddFolderAction(target)) ;
		specImportSpecAction *importAction = new specImportSpecAction(target) ;
		importAction->setFilters(QStringList() << "Log-files (*.log)");
		importAction->setAcceptableImportFunctions(QList<QList<specModelItem*> (*)(QFile&)>() << readLogFile);
		addNewAction(bar, importAction) ;
		bar->addSeparator() ;
		addNewAction(bar, new specCopyAction(target)) ;
		addNewAction(bar, new specCutAction(target)) ;
		addNewAction(bar, new specPasteAction(target)) ;
		addNewAction(bar, new specDeleteAction(target)) ;
        bar->addSeparator() ;
        addNewAction(bar, new specTreeAction(target)) ;

		return bar ;
	}
	else if (dynamic_cast<specPlot*>(target))
	{
		QToolBar *bar = new QToolBar(target) ;

		addNewAction(bar, new specTitleAction(target)) ;
		addNewAction(bar, new specXLabelAction(target)) ;
		addNewAction(bar, new specYLabelAction(target)) ;

		return bar ;
	}

	return new QToolBar(target) ;
}

void specActionLibrary::addDragDropPartner(specModel* model)
{
	if (!partners.contains(model))
		partners << model ;
	model->setDropBuddy(this) ;
}

void specActionLibrary::addParent(QObject *pointer)
{
	if (!parents.contains(pointer))
		parents << pointer ;
}

QObject* specActionLibrary::parentId(int num)
{
	return parents[num] ;
}

void specActionLibrary::writeToStream(QDataStream &out) const
{
	out << qint32(undoStack->count()) ;
	out << qint32(undoStack->index()) ;
	for (int i = 0 ; i < undoStack->count() ; ++i)
	{
		specUndoCommand* command = (specUndoCommand*) undoStack->command(i) ;
		out << type(command->id()) << qint32(parents.indexOf(command->parentObject()))
		    << *command ;
	}
    undoStack->setClean();
}

specStreamable* specActionLibrary::factory(const type &t) const
{
    return commandGenerator.commandById(t) ;
}

void specActionLibrary::readFromStream(QDataStream &in)
{
	qint32 num, position ;
	in >> num >> position ; // TODO really rely on num?

	type t ;
	QVector<qint32> parentIndex(num,0) ;

	for (int i = 0 ; i < num ; ++i)
	{
		in >> t >> parentIndex[i] ;
        specStreamable *streamable = produceItem(in) ;
        specUndoCommand *command = dynamic_cast<specUndoCommand*>(streamable) ;
		if (!command)
		{
            qDebug() << "Error reading command no." << i << "of type" << t ;
			undoStack->clear();
            parentIndex.clear();
            delete streamable ;
			continue ;
		}
        qDebug() << "Reading item:" << i << "total count:" << undoStack->count() << "/" << num ;
		undoStack->push(command) ;
	}

	undoStack->setIndex(position) ;
    for (int i = 0 ; i < undoStack->count() ; ++i)
		((specUndoCommand*) undoStack->command(i))->setParentObject(parents[parentIndex[i]]) ;
    qDebug() << "to be read:" << num << "actually on stack:" << undoStack->count() ;
    undoStack->setClean();
}

void specActionLibrary::setLastRequested(const QModelIndexList &list)
{
	lastRequested = list ;
}

int specActionLibrary::moveInternally(const QModelIndex &parent, int row, specView* target)
{
	int count = lastRequested.size() ;
	specMoveCommand *command = new specMoveCommand ;
	command->setParentObject(target->model());
	command->setItems(lastRequested,parent,row) ;
	command->setText(tr("Move items"));
	push(command) ;
	return count ;
}

int specActionLibrary::deleteInternally(specModel* model)
{
    int count = lastRequested.size() ;
    specUndoCommand *command = specDeleteAction::command(model, lastRequested) ;
    command->setText(tr("Move items"));
    push(command) ;
    return count ;
}

void specActionLibrary::addPlot(specPlot *plot)
{
    connect(undoStack,SIGNAL(indexChanged(int)),plot,SLOT(replot())) ;
}

void specActionLibrary::purgeUndo()
{
	undoStack->clear();
}

QMenu *specActionLibrary::contextMenu(QWidget *w)
{
	QMenu *cMenu = new QMenu(w) ;
	specMetaView* metaView = dynamic_cast<specMetaView*>(w) ;
	specView *view = dynamic_cast<specView*>(w) ;
	if (metaView && metaView->model())
	{
		specModelItem *currentItem = view->model()->itemPointer(metaView->currentIndex()) ;
		if (specMetaItem* mi = dynamic_cast<specMetaItem*>(currentItem))
		{
			addNewAction(cMenu, new specAddConnectionsAction(w)) ;
			if (mi->getFitCurve())
			{
				addNewAction(cMenu, new specRemoveFitAction(w)) ;
				addNewAction(cMenu, new specConductFitAction(w)) ;
				addNewAction(cMenu, new specToggleFitStyleAction(w)) ;
			}
			else
				addNewAction(cMenu, new specAddFitAction(w)) ;
		}
	}
	if (view && view->model())
	{
        addNewAction(cMenu, new specAddFolderAction(w)) ;
        specModelItem *currentItem = view->model()->itemPointer(view->currentIndex()) ;
        specSetMultilineAction *mlAction = new specSetMultilineAction(w) ;
        addNewAction(cMenu, mlAction) ;
        if (currentItem)
            mlAction->setChecked(
                    currentItem->descriptorProperties(
                        view->model()->descriptors()[
                            view->currentIndex().column()]) & spec::multiline) ; // TODO move to action
        if (dynamic_cast<specDataItem*>(currentItem)
                || dynamic_cast<specMetaItem*>(currentItem)
                || dynamic_cast<specSVGItem*>(currentItem))
            addNewAction(cMenu, new specItemPropertiesAction(w)) ;
        if (QApplication::clipboard()->mimeData())
            addNewAction(cMenu, new specPasteAction(w)) ;
        if (!view->getSelection().isEmpty())
        {
            addNewAction(cMenu, new specCopyAction(w)) ;
            addNewAction(cMenu, new specCutAction(w)) ;
            addNewAction(cMenu, new specDeleteAction(w)) ;
        }
	}
	return cMenu ;
}
