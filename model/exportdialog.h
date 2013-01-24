#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStringList>
#include <QScrollArea>
#include "exportlayoutitem.h"
#include "exportformatitem.h"

class exportDialog : public QDialog
{
	Q_OBJECT
	private:
		QPushButton *addButton, *addDataButton ;
		QBoxLayout *layout, *inLayout, *dataLayout ;
		QStringList *descriptors ;
        QStringList dataTypes ;
		QScrollArea *scrollHeader, *scrollData ;
		void prepareHeader() ;
		void prepareData() ;
	private slots :
		void addHeaderItem() ;
		void addDataItem() ;
	public:
        exportDialog (QStringList* descriptors, const QStringList& dataTypes, QWidget *parent = 0 );
		QList<QPair<bool,QString> > headerFormat() ;
		QList<QPair<spec::value,QString> > dataFormat() ;
		~exportDialog();
};

#endif
