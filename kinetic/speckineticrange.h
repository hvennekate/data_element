#ifndef SPECKINETICRANGE_H
#define SPECKINETICRANGE_H

#include <specrange.h>
#include "speckinetic.h"

QDataStream& operator<<(QDataStream&, const specKineticRange&);
QDataStream& operator>>(QDataStream&, specKineticRange&);

class specKineticRange : public specRange
{
	private:
		specKinetic *parent ;
public:
	specKineticRange(double,double,specKinetic *parent=0);
	~specKineticRange();

	QMenu* contextMenu() ;
	void refreshPlotData();
	friend QDataStream& operator<<(QDataStream&, const specKineticRange&);
	friend QDataStream& operator>>(QDataStream&, specKineticRange&);
};

#endif
