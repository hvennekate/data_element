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
	QComboBox* Value, *Separator ;
	QPushButton* removeButton ;
	QHBoxLayout* layout ;
private slots:
	void remove() ;
public:
	exportFormatItem(const QStringList& values, QWidget* parent = 0);
	~exportFormatItem();
	QString separator() ;
	int value() ;
	void setValue(int) ;
	void setSeparator(spec::separator) ;
	void setDataTypes(const QStringList& ds) ;
signals:
	void changed() ;

};

#endif
