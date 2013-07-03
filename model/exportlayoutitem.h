#ifndef EXPORTLAYOUTITEM_H
#define EXPORTLAYOUTITEM_H

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QHBoxLayout>
#include <QPushButton>

class exportLayoutItem : public QWidget
{
	Q_OBJECT
private:
	QCheckBox *isFreeForm ;
	QComboBox *descriptor ;
	QLineEdit *freeText ;
	QPushButton *removeButton ;
	QHBoxLayout *layout ;
private slots:
	void freeFormMode(int on=Qt::Unchecked) ;
	void remove() ;

public:
	exportLayoutItem (QStringList&, QWidget *parent = 0 );

	bool isFreeText() ;
	QString text() ;
	void setDescriptors(const QStringList&) ;
	~exportLayoutItem();
signals:
	void changed() ;
};

#endif
