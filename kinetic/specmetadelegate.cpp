#include "specmetadelegate.h"
#include "specmodel.h"

specMetaDelegate::specMetaDelegate(QObject *parent)
	: specDelegate(parent)
{
}

void specMetaDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem style = option ;
	if (!(
				((specModel*)index.model())->itemPointer(index)
				->descriptor("errors").isEmpty())
			)
	{
		QFont font = style.font ;
		font.setBold(true) ;
		style.font = font ;
	}
	specDelegate::paint(painter, style, index) ;
}
