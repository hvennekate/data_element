#ifndef SPECDESCRIPTORFLAGSCOMMAND_H
#define SPECDESCRIPTORFLAGSCOMMAND_H

#include "specundocommand.h"
#include "specgenealogy.h"

class specDescriptorFlagsCommand : public specUndoCommand
{
private:
    typedef QPair<specGenealogy,qint8>  itemPropertyPair ;
    typedef QVector<itemPropertyPair> itemsContainer ;
    itemsContainer items ;
    QString key ;
    void doIt() ;
    void undoIt() ;
    void writeCommand(QDataStream &out) const ;
    void readCommand(QDataStream &in) ;
    type typeId() const { return specStreamable::descriptorFlagsCommand ; }
    void parentAssigned();
public:
    explicit specDescriptorFlagsCommand(specUndoCommand* parent = 0) ;
    void setItems(const QModelIndexList& list, QString key, spec::descriptorFlags f) ;
    void addItem(const QModelIndex& index, QString Key, spec::descriptorFlags f) ;
};

#endif // SPECDESCRIPTORFLAGSCOMMAND_H
