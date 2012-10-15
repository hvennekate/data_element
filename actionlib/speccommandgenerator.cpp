#include "speccommandgenerator.h"
#include "specaddfoldercommand.h"
#include "specdeletecommand.h"
#include "specmovecommand.h"
#include "specplotmovecommand.h"
#include "specmulticommand.h"
#include "specexchangedatacommand.h"
#include "specresizesvgcommand.h"
#include "specaddconnectionscommand.h"
#include "specdeleteconnectionscommand.h"
#include "speceditdescriptorcommand.h"
#include "specmetarangecommand.h"
#include "specplotlabelcommand.h"
#include "specexchangefitcurvecommand.h"
#include "spectogglefitstylecommand.h"
#include "specstylecommand.h"

specCommandGenerator::specCommandGenerator(specUndoCommand *p)
	: parent(p)
{
}

specUndoCommand *specCommandGenerator::commandById(int id) const
{
	switch(id)
	{
	case specStreamable::toggleFitStyleCommand:
		return new specToggleFitStyleCommand(parent) ;
	case specStreamable::exchangeFitCommand:
		return new specExchangeFitCurveCommand(parent) ;
	case specStreamable::deleteCommandId :
		return new specDeleteCommand(parent) ;
	case specStreamable::newFolderCommandId :
		return new specAddFolderCommand(parent) ;
	case specStreamable::moveItemsCommandId :
		return new specMoveCommand(parent) ;
	case specStreamable::movePlotCommandId :
		return new specPlotMoveCommand(parent) ;
	case specStreamable::multiCommandId :
		return new specMultiCommand(parent) ;
	case specStreamable::exchangeDataCommandId:
		return new specExchangeDataCommand(parent) ;
	case specStreamable::resizeSVGCommandId :
		return new specResizeSVGcommand(parent) ;
	case specStreamable::newConnectionsCommandId :
		return new specAddConnectionsCommand(parent) ;
	case specStreamable::deleteConnectionsCommandId :
		return new specDeleteConnectionsCommand(parent) ;
	case specStreamable::editDescriptorCommandId :
		return new specEditDescriptorCommand(parent) ;
	case specStreamable::penColorCommandId:
	case specStreamable::lineWidthCommandId:
	case specStreamable::symbolStyleCommandId:
	case specStreamable::symbolPenColorCommandId:
	case specStreamable::symbolSizeCommandId:
	case specStreamable::symbolBrushColorCommandId:
	case specStreamable::penStyleCommandId:
		return generateStyleCommand(id,parent) ;
	case specStreamable::metaRangeCommand :
		return new specMetaRangeCommand(parent) ;
	case specStreamable::plotTitleCommandId:
	case specStreamable::plotYLabelCommandId:
	case specStreamable::plotXLabelCommandId:
		generatePlotLabelCommand(id,parent) ;
	default:
		return 0 ;
	}
}
