#ifndef SPECMANAGEDATACOMMAND_H
#define SPECMANAGEDATACOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"
#include "specmodel.h"
#include "specdataitem.h"

class specManageDataCommand : public specUndoCommand
{
private:
	specGenealogy *item ;
	QVector<int> toTake ;
	QVector<specDataPoint> taken ;
public:
	specManageDataCommand(specUndoCommand *parent = 0) ;
	void setItem(const QModelIndex&, const QVector<int>&) ;

	bool ok() ;
	QDataStream& write(QDataStream &out) const ;
	QDataStream& read(QDataStream &in) ;
protected:
	void take() ;
	void insert() ;
};

#endif // SPECMANAGEDATACOMMAND_H
