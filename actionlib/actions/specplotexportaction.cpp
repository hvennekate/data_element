#include "specplotexportaction.h"
#include "qwt_plot.h"
#include "qwt_plot_renderer.h"
#include "ui_sizedialog.h"
#include <QDialog>

specPlotExportAction::specPlotExportAction(QObject *parent) :
	specAbstractPlotAction(parent),
	dialog(new QDialog),
	ui(new Ui::specSizeDialog)
{
	setIcon(QIcon(":/exportPlot.png")) ;
	setToolTip(tr("Export plot")) ;
	setWhatsThis(tr("Exports the plot to an image file"));
	setText(tr("Export plot..."));
	setShortcut(tr("Ctrl+O"));

	ui->setupUi(dialog) ;
	ui->width->setMaximum(INFINITY) ;
	ui->height->setMaximum(INFINITY) ;
}

void specPlotExportAction::execute()
{
	QwtPlotRenderer renderer ;
	renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground);
	renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground);
	renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame);

	// TODO resolution
	if (dialog->exec() != QDialog::Accepted) return ;

	renderer.exportTo(plot(), "plot.pdf",
			  QSize(ui->width->value(), ui->height->value())) ;
}
