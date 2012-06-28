#ifndef SPECMETARANGE_H
#define SPECMETARANGE_H
#include "specrange.h"
#include <QObject>

class specMetaVariable ;

class specMetaRange : public specRange
{
private:
	int selected ;
	specMetaVariable *parent ;
public:
	specMetaRange(double x1, double x2, specMetaVariable *parent) ;
	void refreshPlotData();
};

#endif // SPECMETARANGE_H
