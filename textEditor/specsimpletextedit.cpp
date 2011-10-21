#include "specsimpletextedit.h"
#include <QVBoxLayout>

specSimpleTextEdit::specSimpleTextEdit(QWidget *parent) :
    QWidget(parent),
    content(new QTextEdit),
    toolBar(new QToolBar),
    bold(new QAction(QIcon::fromTheme("format-text-bold"),tr("Fett"),this)),
    italic(new QAction(QIcon::fromTheme("format-text-italic"),tr("Kursiv"),this)),
    font(new QFontComboBox(this)),
    size(new QDoubleSpinBox(this))
{
	setLayout(new QVBoxLayout) ;

	layout()->addWidget(toolBar) ;
	layout()->addWidget(content) ;

	bold->setCheckable(true) ;
	italic->setCheckable(true) ;

	toolBar->addAction(bold) ;
	toolBar->addAction(italic) ;
	toolBar->addWidget(font) ;
	toolBar->addWidget(size) ;

	formatChange(content->currentCharFormat()) ;

	connect(bold,SIGNAL(triggered()),this,SLOT(toggleBold())) ;
	connect(italic,SIGNAL(triggered()),this,SLOT(toggleItalic())) ;
	connect(font,SIGNAL(currentFontChanged(QFont)),content,SLOT(setCurrentFont(QFont))) ;
	connect(content,SIGNAL(currentCharFormatChanged(QTextCharFormat)),this,SLOT(formatChange(QTextCharFormat))) ;
	connect(size,SIGNAL(valueChanged(double)),this,SLOT(changeFontSize(double))) ;
	connect(content,SIGNAL(textChanged()),this,SLOT(newContent())) ;
}

void specSimpleTextEdit::newContent()
{
	emit contentChanged(getText());
}

void specSimpleTextEdit::toggleBold()
{
	QFont font = content->currentFont() ;
	font.setBold( ! font.bold() );
	content->setCurrentFont(font) ;
}

void specSimpleTextEdit::toggleItalic()
{
	content->setFontItalic( ! content->currentFont().italic());
}

void specSimpleTextEdit::formatChange(const QTextCharFormat & format)
{
	QFont currentFont = format.font() ;
	bold->setChecked(currentFont.bold()) ;
	italic->setChecked(currentFont.italic()) ;
	font->setCurrentFont(currentFont) ;
	size->setValue(currentFont.pointSizeF());
	emit content->textChanged();
}

QString specSimpleTextEdit::getText() const
{
	return content->toHtml() ;
}

void specSimpleTextEdit::setText(const QString &text)
{
	content->setHtml(text) ;
}

void specSimpleTextEdit::changeFontSize(const double & newSize)
{
	QFont font = content->currentFont() ;
	font.setPointSizeF(newSize);
	content->setCurrentFont(font) ;
}
