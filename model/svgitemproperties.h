#ifndef SVGITEMPROPERTIES_H
#define SVGITEMPROPERTIES_H

#include <QDialog>

class specSVGItem ;
class specUndoCommand ;

namespace Ui {
class svgItemProperties;
}

class svgItemProperties : public QDialog
{
	Q_OBJECT

public:
	explicit svgItemProperties(specSVGItem* item, QWidget *parent = 0);
	~svgItemProperties();
	specUndoCommand *generateCommand(QObject* parent) ;
private slots:
	void on_widthOriginalAspect_clicked();

	void on_heightOriginalAspect_clicked();

private:
	Ui::svgItemProperties *ui;
	specSVGItem *item ;
	double originalAspectRatio ;
};

#endif // SVGITEMPROPERTIES_H
