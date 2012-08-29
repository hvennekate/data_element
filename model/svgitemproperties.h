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
private:
	Ui::svgItemProperties *ui;
	specSVGItem *item ;
};

#endif // SVGITEMPROPERTIES_H
