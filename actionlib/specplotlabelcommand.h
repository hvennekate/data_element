#ifndef SPECPLOTLABELCOMMAND_H
#define SPECPLOTLABELCOMMAND_H

#include "specundocommand.h"

class specPlotLabelCommand : public specUndoCommand
{
private:
	void undoIt();
	void writeToStream(QDataStream &out) const ;
	void readFromStream(QDataStream &in) ;
protected:
	QString text ;
public:
	specPlotLabelCommand(specUndoCommand *parent = 0) ;
	void setLabelText(const QString& ) ;
};

specPlotLabelCommand* generatePlotLabelCommand(specStreamable::type id, specUndoCommand* parent = 0) ;

class specPlotTitleCommand : public specPlotLabelCommand
{
private:
	void doIt() ;
	type typeId() const { return specStreamable::plotTitleCommandId ; }
public:
	specPlotTitleCommand(specUndoCommand *parent = 0) ;
};

class specPlotYLabelCommand : public specPlotLabelCommand
{
private:
	void doIt() ;
	type typeId() const { return specStreamable::plotYLabelCommandId ; }
public:
	specPlotYLabelCommand(specUndoCommand *parent = 0) ;
};

class specPlotXLabelCommand : public specPlotLabelCommand
{
private:
	void doIt() ;
	type typeId() const { return specStreamable::plotXLabelCommandId ; }
public:
	specPlotXLabelCommand(specUndoCommand *parent= 0) ;
};

#endif // SPECPLOTLABELCOMMAND_H
