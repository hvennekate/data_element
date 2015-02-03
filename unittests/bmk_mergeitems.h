#ifndef BMK_MERGEITEMS_H
#define BMK_MERGEITEMS_H

#include <QObject>
#include <QtTest>


class specModelItem ;

class bmk_mergeItems : public QObject
{
	Q_OBJECT
public:
	explicit bmk_mergeItems(QObject *parent = 0);
	~bmk_mergeItems();
private:
	QList<specModelItem*> items ;
private Q_SLOTS:
	void init() ;
	void cleanup() ;

	void merge() ;
	void merge_data() ;

	void runnerMerge() ;
	void runnerMerge_data() ;

	void parallelRunnerMerge() ;
	void parallelRunnerMerge_data();
};

#endif // BMK_MERGEITEMS_H
