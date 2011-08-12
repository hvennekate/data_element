#ifndef EXPORTFORMATITEM_H
#define EXPORTFORMATITEM_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include "names.h"

class exportFormatItem : public QWidget
{
	Q_OBJECT
	private:
		QComboBox *Value, *Separator ;
		QPushButton *removeButton ;
		QHBoxLayout *layout ;
	private slots:
		void remove() ;
	public:
		exportFormatItem ( QWidget *parent = 0 );
		~exportFormatItem();
		QString separator() ;
		spec::value value() ;
	signals:
		void changed() ;

};

#endif
