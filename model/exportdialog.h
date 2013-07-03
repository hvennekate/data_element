#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStringList>
#include <QScrollArea>

#include "names.h"

class exportFormatItem ;
class exportLayoutItem ;

class exportDialog : public QDialog
{
	Q_OBJECT
private:
	QPushButton *addButton, *addDataButton ;
	QBoxLayout *layout, *inLayout, *dataLayout ;
	QStringList descriptors ;
	QStringList dataTypes ;
	QScrollArea *scrollHeader, *scrollData ;
	void prepareHeader() ;
	void prepareData() ;
	template<class itemType> QList<itemType*> getItems() const ;
private slots :
	void addHeaderItem() ;
	void addDataItem() ;
public:
	explicit exportDialog (QWidget *parent = 0 );
	void setDataTypes(const QStringList& dataTypes) ;
	void setDescriptors(const QStringList& descriptors) ;
	QList<QPair<bool,QString> > headerFormat() const ;
	QList<QPair<spec::value,QString> > dataFormat() const ;
	~exportDialog();
};

#endif
