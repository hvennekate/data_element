#include "specprintplotaction.h"
#include "specplot.h"
#include <QPrinter>
#include <QPrintDialog>
#include <qwt_plot_renderer.h>

specPrintPlotAction::specPrintPlotAction(QObject *parent) :
	specUndoAction(parent),
	printer(new QPrinter(QPrinter::HighResolution))
{
	this->setIcon(QIcon::fromTheme("document-print")) ;
	specPlot *plot = (specPlot*) parentWidget() ;
	QSize plotSize = plot->size() ;
	double aspectRatio = (double) plotSize.width() / plotSize.height() ;
	printer->setOrientation(aspectRatio > 1. ? QPrinter::Landscape : QPrinter::Portrait) ;
}

const std::type_info &specPrintPlotAction::possibleParent()
{
	return typeid(specPlot) ;
}

specPrintPlotAction::~specPrintPlotAction()
{
	delete printer ;
}

void specPrintPlotAction::execute()
{
	qDebug("executing print action") ;
	specPlot *plot = (specPlot*) parentWidget() ;
	QSize plotSize = plot->size() ;
	double aspectRatio = (double) plotSize.width() / plotSize.height() ;

	QPrintDialog dialog(printer) ;
	if (dialog.exec())
	{
		// TODO
		// Prepare page margins so as to match size of plot
		// -- Alternatively, SVG-Items would require rescaling in order to not be distorted.
		// This, however would raise questions on their anchoring.
		// Alternatives:  -- ask user (better)
		//                -- enable user to scale plot to currently printable area/desired aspect ratio (best)
		//                   (might require printer settings though)
		QRectF pageSize = printer->pageRect(QPrinter::Millimeter) ;
		double printRatio = pageSize.width()/pageSize.height() ;
		qreal top,left,bottom,right ;
		printer->getPageMargins(&left,&top,&right,&bottom,QPrinter::Millimeter);
		if (printRatio > aspectRatio)
		{
			double frameAdd = (pageSize.width() - pageSize.height()*aspectRatio)/2.;
			printer->setPageMargins(left+frameAdd,top,right+frameAdd,bottom,QPrinter::Millimeter);
		}
		else if (printRatio < aspectRatio)
		{
			double frameAdd = (pageSize.height() - pageSize.width()/aspectRatio)/2. ;
			printer->setPageMargins(left,top+frameAdd,right,bottom+frameAdd,QPrinter::Millimeter);
		}

		QwtPlotRenderer renderer ;
		renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground,true) ;
		renderer.setLayoutFlag(QwtPlotRenderer::KeepFrames,true) ;
		renderer.renderTo(plot,*printer) ;
		printer->setPageMargins(left,top,right,bottom,QPrinter::Millimeter) ;
	}
}
