#ifndef SPECLOGMESSAGE_H
#define SPECLOGMESSAGE_H

#include <specdataitem.h>

class specLogMessage : public specDataItem
{
public:
	specLogMessage(QHash<QString,specDescriptor> description, specFolderItem* par=0, QString tag="");
	~specLogMessage();

	bool changeDescriptor ( QString key, QString value );
	bool isEditable ( QString key ) const;
	QIcon decoration() const;
	QMultiMap< double, QPair<double,double> >* kinetics ( QList< specKineticRange* > arg1 ) const;
	specDataItem& operator += ( const specDataItem& toAdd );
	spec::descriptorFlags descriptorProperties ( const QString& key ) const;
	void exportData ( const QList< QPair < bool , QString > >& arg1, const QList< QPair < spec :: value , QString > >& arg2, QTextStream& arg3 );
	void addToSlope ( const double& arg1 ) {}
// 	void flatten(bool) {}
	void moveYBy ( const double& arg1 ) {}
	void refreshPlotData() {}
	void scaleBy ( const double& arg1 ) {}

protected:
	QDataStream& readFromStream ( QDataStream& arg1 );
	QDataStream& writeToStream ( QDataStream& arg1 ) const;

};

#endif
