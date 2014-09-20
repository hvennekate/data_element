#ifndef SPECDESCRIPTORCOMPARISONCRITERION_H
#define SPECDESCRIPTORCOMPARISONCRITERION_H
#include <QString>
#include <QList>

class specModelItem ;

class specDescriptorComparisonCriterion
{
private:
	double toleranceValue ;
	QString descriptorName ;
public:
	specDescriptorComparisonCriterion();
	explicit specDescriptorComparisonCriterion(const QString& descriptor, double tolerance = 0) ;

	QString descriptor() const ;
	void setDescriptor(const QString& s) ;

	double tolerance() const ;
	void setTolerance(double d) ;

	bool itemsEqual(const specModelItem* a, const specModelItem* b) const ;
	typedef QList<specDescriptorComparisonCriterion> container ;
	static bool itemsEqual(specModelItem *a, specModelItem *b, const container& criteria) ;
};

#endif // SPECDESCRIPTORCOMPARISONCRITERION_H
