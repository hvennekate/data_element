#ifndef EDITIMPORTDIRSACTION_H
#define EDITIMPORTDIRSACTION_H

#include <QAction>

class QDialog ;
class QListWidgetItem ;
namespace Ui {
	class importDirectoriesDialog ;
}

class editImportDirsAction : public QAction
{
	Q_OBJECT
private:
	QDialog *dialog ;
	Ui::importDirectoriesDialog* ui ;
	QStringList allDirectories() const ;
	QString getDirFromUser(QString dir = QString()) const ;
private slots:
	void editDirs() ;
	void editItem(QListWidgetItem*listItem) const ;
	void deleteItems() const ;
	void addEntry() const ;
public:
	explicit editImportDirsAction(QObject *parent = 0);
	~editImportDirsAction() ;
};

#endif // EDITIMPORTDIRSACTION_H
