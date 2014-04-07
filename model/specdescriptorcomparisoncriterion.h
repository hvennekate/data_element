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
	explicit specDescriptorComparisonCriterion(const QString& descriptor, bool numeric = false) ;
	specDescriptorComparisonCriterion(const QString& descriptor, double tolerance) ;

	QString descriptor() const ;
	void setDescriptor(const QString& s) ;

	bool isNumeric() const ;
	void setNumeric(bool a = true) ;

	double tolerance() const ;
	void setTolerance(double d) ;

	bool itemsEqual(const specModelItem* a, const specModelItem* b) const ;
	typedef QList<specDescriptorComparisonCriterion> container ;
	static bool itemsEqual(specModelItem *a, specModelItem *b, const container& criteria) ;
};

#endif // SPECDESCRIPTORCOMPARISONCRITERION_H
