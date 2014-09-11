#include "editimportdirsaction.h"
#include <QDialog>
#include "ui_logconverterimportdirectoriesdialog.h"
#include "speclogtodataconverter.h"
#include <QFileDialog>
#include <QMessageBox>

editImportDirsAction::editImportDirsAction(QObject *parent) :
	QAction(parent),
	dialog(new QDialog),
	ui(new Ui::importDirectoriesDialog)
{
	setIcon(QIcon::fromTheme("system-search")) ;
	setStatusTip(tr("Change, delete or add directories to search for data files")) ;
	setWhatsThis(tr("This action allows you to change the directories which will be searched for data files."));
	setText("Edit search path...") ;

	dialog->setWindowTitle(tr("Edit search path list"));
	ui->setupUi(dialog) ;
	ui->directoryListWidget->addItems(specLogToDataConverter::searchDirectories());
	ui->addItem->setIcon(QIcon::fromTheme("list-add")) ;
	ui->deleteItems->setIcon(QIcon::fromTheme("list-remove")) ;
	connect(ui->directoryListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
		this, SLOT(editItem(QListWidgetItem*))) ;
	connect(this, SIGNAL(triggered()), this, SLOT(editDirs())) ;
	connect(ui->addItem, SIGNAL(clicked()), this, SLOT(addEntry())) ;
	connect(ui->deleteItems, SIGNAL(clicked()), this, SLOT(deleteItems())) ;
}

editImportDirsAction::~editImportDirsAction()
{
	delete ui ;
	delete dialog ;
}

void editImportDirsAction::editDirs()
{
	if (QDialog::Accepted == dialog->exec())
		specLogToDataConverter::setSearchDirectories(allDirectories()) ;
	else
	{
		ui->directoryListWidget->clear();
		ui->directoryListWidget->addItems(specLogToDataConverter::searchDirectories()) ;
	}
}

QStringList editImportDirsAction::allDirectories() const
{
	QStringList dirs ;
	for (int i = 0 ; i < ui->directoryListWidget->count() ; ++i)
		dirs << ui->directoryListWidget->item(i)->text() ;
	return dirs ;
}

void editImportDirsAction::editItem(QListWidgetItem *listItem) const
{
	QString newDir = getDirFromUser(listItem->text()) ;
	if (!newDir.isEmpty()) listItem->setText(newDir) ;
}

void editImportDirsAction::deleteItems() const
{
	foreach(QListWidgetItem* item, ui->directoryListWidget->selectedItems())
		delete item ;
}

void editImportDirsAction::addEntry() const
{
	QString newDir = getDirFromUser() ;
	if (!newDir.isEmpty())
		ui->directoryListWidget->insertItem(
					ui->directoryListWidget->currentRow() == -1 ?
						ui->directoryListWidget->count() :
						ui->directoryListWidget->currentRow(),
					newDir);
}

QString editImportDirsAction::getDirFromUser(QString dir) const
{
	while (true) // TODO mit editItem zusammenfuehren
	{
		QString newDir = QFileDialog::getExistingDirectory(0,
							   dir.isEmpty() ?
								   tr("Add new search directory")
								 : tr("Change search directory"),
							   dir) ;
		if (newDir.isEmpty()) break ;
		if (dir == newDir) break ;
		if (!allDirectories().contains(newDir)) return newDir ;
		if (QMessageBox::No == QMessageBox::warning(0,
							    tr("Already in list"),
							    tr("The folder \"")
							    + newDir
							    + tr("\" is already contained in the list of search directories."
								 "Specify a different one?"),
							    QMessageBox::Yes | QMessageBox::No,
							    QMessageBox::No))
			break ;
	}
	return QString() ;
}
