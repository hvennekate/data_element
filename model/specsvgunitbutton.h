#ifndef SPECSVGUNITBUTTON_H
#define SPECSVGUNITBUTTON_H

#include <QPushButton>

class specSvgUnitButton : public QPushButton
{
	Q_OBJECT
private:
private slots:
	void stateChanged() ;
public:
	explicit specSvgUnitButton(QWidget* parent = 0);
};

#endif // SPECSVGUNITBUTTON_H
