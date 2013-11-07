#ifndef SPECCOMMANDGENERATOR_H
#define SPECCOMMANDGENERATOR_H

class specUndoCommand ;

class specCommandGenerator
{
private:
	specUndoCommand* parent ;
public:
	specCommandGenerator(specUndoCommand* parent = 0) ;
	specUndoCommand* commandById(int id) const ;
};

#endif // SPECCOMMANDGENERATOR_H
