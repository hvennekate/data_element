#include "svgitemproperties.h"
#include "ui_svgitemproperties.h"
#include "specsvgitem.h"
#include "qwt_plot.h"
#include <QSvgRenderer>
#include "actionlib/specresizesvgcommand.h"
#include "specmodel.h"

svgItemProperties::svgItemProperties(specSVGItem* i, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::svgItemProperties),
	item(i)
{
	ui->setupUi(this);

	ui->itemPreview->load(i->data);
	QSize svgSize = QSvgRenderer(item->data).defaultSize() ;
	originalAspectRatio = (double) svgSize.width()/svgSize.height() ;
	qDebug() << "original aspect ratio:" << originalAspectRatio ;
	svgSize.scale(300,300,Qt::KeepAspectRatio);
	ui->itemPreview->setFixedSize(svgSize) ;
//	ui->itemPreview

	ui->anchorRadioButtons->setId(ui->topLeftRadio,     specSVGItem::topLeft);
	ui->anchorRadioButtons->setId(ui->topRadio,         specSVGItem::top);
	ui->anchorRadioButtons->setId(ui->topRightRadio,    specSVGItem::topRight);
	ui->anchorRadioButtons->setId(ui->leftRadio,        specSVGItem::left);
	ui->anchorRadioButtons->setId(ui->centerRadio,      specSVGItem::center);
	ui->anchorRadioButtons->setId(ui->rightRadio,       specSVGItem::right);
	ui->anchorRadioButtons->setId(ui->bottomLeftRadio,  specSVGItem::bottomLeft);
	ui->anchorRadioButtons->setId(ui->bottomRadio,      specSVGItem::bottom);
	ui->anchorRadioButtons->setId(ui->bottomRightRadio, specSVGItem::bottomRight);

	ui->anchorRadioButtons->button(item->anchor)->setChecked(true) ;

	ui->xValue->setRange(-INFINITY,INFINITY);
	ui->yValue->setRange(-INFINITY,INFINITY);
	ui->widthValue->setRange(0,INFINITY);
	ui->heightValue->setRange(0,INFINITY);

	ui->xValue->setValue(item->ownBounds.x.second) ;
	ui->yValue->setValue(item->ownBounds.y.second) ;
	ui->widthValue->setValue(item->ownBounds.width.second) ;
	ui->heightValue->setValue(item->ownBounds.height.second) ;

	ui->xPixelsButton->setChecked(item->ownBounds.x.first != QwtPlot::axisCnt) ;
	ui->yPixelsButton->setChecked(item->ownBounds.y.first != QwtPlot::axisCnt) ;
	ui->widthPixelsButton->setChecked(item->ownBounds.width.first != QwtPlot::axisCnt) ;
	ui->heightPixelsButton->setChecked(item->ownBounds.height.first != QwtPlot::axisCnt) ;
}

svgItemProperties::~svgItemProperties()
{
	delete ui;
}

specUndoCommand* svgItemProperties::generateCommand(QObject *parent)
{
	specSVGItem::SVGCornerPoint newAnchor =
			(specSVGItem::SVGCornerPoint) ui->anchorRadioButtons->checkedId() ;
	specSVGItem::SVGCornerPoint oldAnchor = item->setAnchor(newAnchor) ;
	specSVGItem::bounds newBounds =
		{ specSVGItem::dimension(ui->xPixelsButton->isChecked() ?
				QwtPlot::xBottom : QwtPlot::axisCnt,
			ui->xValue->value()),
		  specSVGItem::dimension(ui->yPixelsButton->isChecked() ?
				QwtPlot::yLeft   : QwtPlot::axisCnt,
			ui->yValue->value()),
		  specSVGItem::dimension(ui->widthPixelsButton->isChecked()?
				QwtPlot::xBottom : QwtPlot::axisCnt,
			ui->widthValue->value()),
		  specSVGItem::dimension(ui->heightPixelsButton->isChecked()?
				QwtPlot::yLeft   : QwtPlot::axisCnt,
			ui->heightValue->value())
		},
			oldBounds = item->getBounds() ;
	if (newAnchor == oldAnchor && newBounds == oldBounds) return 0 ;

	specResizeSVGcommand *command = new specResizeSVGcommand ;
	command->setParentObject(parent) ;
	item->setBounds(newBounds) ;
	command->setItem(((specModel*) parent)->index(item),oldBounds,oldAnchor) ;
	return command ;
}

void svgItemProperties::on_widthOriginalAspect_clicked()
{
	ui->widthValue->setValue(ui->heightValue->value()*originalAspectRatio) ;
}

void svgItemProperties::on_heightOriginalAspect_clicked()
{
	ui->heightValue->setValue(ui->widthValue->value()/originalAspectRatio);
}
