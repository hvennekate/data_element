#ifndef SPECMETADELEGATE_H
#define SPECMETADELEGATE_H
#include "specdelegate.h"

class specMetaDelegate : public specDelegate
{
public:
	specMetaDelegate(QObject* parent = 0) ;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const ;
};

#endif // SPECMETADELEGATE_H
