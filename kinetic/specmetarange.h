#ifndef SPECMETARANGE_H
#define SPECMETARANGE_H
#include "specrange.h"
#include <QObject>

class specMetaVariable ;
class specMetaItem ;

class specMetaRange : public specRange
{
private:
	specMetaVariable *parent ;
	type typeId() const { return specStreamable::metaRange ; }
public:
	struct addressObject
	{
		specMetaItem* item;
		int variable, range ;
	};

	specMetaRange(double x1, double x2, specMetaVariable *parent) ;
	addressObject address() ;
	void attach(QwtPlot *plot);
	void detach();
	~specMetaRange() ;
};

#endif // SPECMETARANGE_H
