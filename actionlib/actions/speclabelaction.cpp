#include "speclabelaction.h"
#include "textEditor/specsimpletextedit.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "specplotlabelcommand.h"

specLabelAction::specLabelAction(QObject *parent)
	: specUndoAction(parent)
{
}

specTitleAction::specTitleAction(QObject *parent)
	: specLabelAction(parent)
{
	setIcon(QIcon(":/changetitle.png")) ;
	setToolTip("Change Plot Title") ;
	setWhatsThis("Allows you to edit the plot's title.");
	setText(tr("Change plot title...")) ;
	setShortcut(tr("Ctrl+T"));
}

specXLabelAction::specXLabelAction(QObject *parent)
	: specLabelAction(parent)
{
	setIcon(QIcon(":/changexlabel.png")) ;
	setToolTip("Change X Label") ;
	setWhatsThis("Allows you to edit the plot's x axis label.");
	setText(tr("Plot x label...")) ;
	setShortcut(tr("Ctrl+Alt+x"));
}

specYLabelAction::specYLabelAction(QObject *parent)
	: specLabelAction(parent)
{
	setIcon(QIcon(":/changeylabel.png")) ;
	setToolTip("Change Y Label") ;
	setWhatsThis("Allows you to edit the plot's y axis label.");
	setText(tr("Plot y label...")) ;
	setShortcut(tr("Ctrl+Alt+y"));
}

const std::type_info& specLabelAction::possibleParent()
{
	return typeid(specPlot) ;
}

void specLabelAction::execute()
{
	// TODO refine simple text edit
	QString text(textToEdit()) ;
	QDialog dialog ;
	dialog.setWindowTitle(tr("Edit Label")) ;
	dialog.setLayout(new QVBoxLayout) ;
	specSimpleTextEdit *editor = new specSimpleTextEdit(&dialog) ;
	editor->setText(text) ;
	dialog.layout()->addWidget(editor) ;
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel) ;
	dialog.layout()->addWidget(buttons);
	connect(buttons,SIGNAL(accepted()),&dialog,SLOT(accept())) ;
	connect(buttons,SIGNAL(rejected()),&dialog,SLOT(reject())) ;

	if (QDialog::Accepted != dialog.exec() || editor->getText() == text) return ;

	text = editor->getText() ;
	specPlotLabelCommand* command = generatePlotLabelCommand(commandId()) ;
	command->setParentObject(parent()) ;
	command->setLabelText(text) ;
	command->setText(tr("Edited ") + labelText());
	library->push(command) ;
}

QString specTitleAction::textToEdit()
{
	return ((specPlot*) parent())->title().text() ;
}

QString specXLabelAction::textToEdit()
{
	return ((specPlot*) parent())->axisTitle(QwtPlot::xBottom).text() ;
}

QString specYLabelAction::textToEdit()
{
	return ((specPlot*) parent())->axisTitle(QwtPlot::yLeft).text() ;
}

specStreamable::type specTitleAction::commandId()
{
	return specStreamable::plotTitleCommandId ;
}

specStreamable::type specXLabelAction::commandId()
{
	return specStreamable::plotXLabelCommandId ;
}

specStreamable::type specYLabelAction::commandId()
{
	return specStreamable::plotYLabelCommandId ;
}

QString specTitleAction::labelText() const
{
	return tr("title") ;
}

QString specXLabelAction::labelText() const
{
	return tr("x axis label") ;
}

QString specYLabelAction::labelText() const
{
	return tr("y axis label") ;
}
