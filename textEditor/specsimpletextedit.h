#ifndef SPECSIMPLETEXTEDIT_H
#define SPECSIMPLETEXTEDIT_H

#include <QWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QAction>
#include <QFontComboBox>
#include <QDoubleSpinBox>

class specSimpleTextEdit : public QWidget
{
Q_OBJECT
private:
	QTextEdit *content ;
	QToolBar  *toolBar ;
	QAction   *bold ;
	QAction   *italic ;
	QFontComboBox *font ;
	QDoubleSpinBox  *size ;
private slots:
	void toggleBold() ;
	void toggleItalic() ;
	void formatChange(const QTextCharFormat&) ;
	void changeFontSize(const double &) ;
	void newContent() ;

public:
    explicit specSimpleTextEdit(QWidget *parent = 0);
    void setText(const QString&) ;
    QString getText() const ;

signals:
	void contentChanged(QString newContent) ;


};

#endif // SPECSIMPLETEXTEDIT_H
