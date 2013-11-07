#include "specplotlabelcommand.h"
#include "specplot.h"

specPlotLabelCommand::specPlotLabelCommand(specUndoCommand* parent)
	: specUndoCommand(parent)
{
}

void specPlotLabelCommand::setLabelText(const QString& t)
{
	text = t ;
}

void specPlotLabelCommand::undoIt()
{
	doIt() ;
}

void specPlotLabelCommand::writeCommand(QDataStream& out) const
{
	out << text ;
}

void specPlotLabelCommand::readCommand(QDataStream& in)
{
	in >> text ;
}

// TODO rewrite, this is dissatisfactory.
void specPlotTitleCommand::doIt()
{
	specPlot* plot = (specPlot*) parentObject() ;
	QString temp = plot->title().text() ;
	plot->setTitle(text) ;
	text = temp ;
}

void specPlotYLabelCommand::doIt()
{
	specPlot* plot = (specPlot*) parentObject() ;
	QString temp = plot->axisTitle(QwtPlot::yLeft).text() ;
	plot->setAxisTitle(QwtPlot::yLeft, text) ;
	text = temp ;
}

void specPlotXLabelCommand::doIt()
{
	specPlot* plot = (specPlot*) parentObject() ;
	QString temp = plot->axisTitle(QwtPlot::xBottom).text() ;
	plot->setAxisTitle(QwtPlot::xBottom, text) ;
	text = temp ;
}

specPlotTitleCommand::specPlotTitleCommand(specUndoCommand* parent)
	: specPlotLabelCommand(parent)
{
}

specPlotXLabelCommand::specPlotXLabelCommand(specUndoCommand* parent)
	: specPlotLabelCommand(parent)
{
}

specPlotYLabelCommand::specPlotYLabelCommand(specUndoCommand* parent)
	: specPlotLabelCommand(parent)
{
}

specPlotLabelCommand* generatePlotLabelCommand(specStreamable::type id, specUndoCommand* parent)
{
	switch(id)
	{
		case(specStreamable::plotTitleCommandId) : return new specPlotTitleCommand(parent) ;
		case(specStreamable::plotYLabelCommandId) : return new specPlotYLabelCommand(parent) ;
		case(specStreamable::plotXLabelCommandId) : return new specPlotXLabelCommand(parent) ;
		default: ;
	}
	return 0 ;
}
