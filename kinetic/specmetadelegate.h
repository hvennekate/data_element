#ifndef SPECMETADELEGATE_H
#define SPECMETADELEGATE_H
#include "specdelegate.h"

class metaVariableEditor ;
class exclusiveValidator ;
class widgetList ;

class specMetaDelegate : public specDelegate
{
	Q_OBJECT
private:
	void initializeVariableEditor(widgetList *parent = 0, QString initString = QString()) const ;
	exclusiveValidator* variableNameValidator ;
	QStringList *descriptors, *fitVariableNames ;
private slots:
	void provideEditor() const ;
public:
	explicit specMetaDelegate(QObject* parent = 0) ;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const ;

	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const ;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
};

#endif // SPECMETADELEGATE_H
