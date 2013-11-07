#ifndef SPECMETARANGE_H
#define SPECMETARANGE_H
#include "specrange.h"
#include <QObject>

class specMetaVariable ;
class specMetaItem ;

class specMetaRange : public specRange
{
private:
	specMetaVariable* parent ;
	type typeId() const { return specStreamable::metaRange ; }
public:
	struct addressObject
	{
		specMetaItem* item;
		int variable, range ;
	};

	specMetaRange(double x1, double x2, double yinit = 0, specMetaVariable* parent = 0) ;
	addressObject address() ;
	void attach(QwtPlot* plot);
	void detach();
	~specMetaRange() ;
	int rtti() const { return spec::kineticRange ; }
};

#endif // SPECMETARANGE_H
