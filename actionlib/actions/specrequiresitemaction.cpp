#include "specrequiresitemaction.h"

specRequiresItemAction::specRequiresItemAction(QObject *parent) :
	specItemAction(parent)
{
}

specRequiresDataItemAction::specRequiresDataItemAction(QObject *parent)
	: specRequiresItemAction(parent)
{}

specRequiresMetaItemAction::specRequiresMetaItemAction(QObject *parent)
	: specRequiresItemAction(parent)
{}

bool specRequiresItemAction::specificRequirements()
{
	return view->selectionModel()->hasSelection() ;
}

bool specRequiresItemAction::postProcessingRequirements() const
{
	return !pointers.isEmpty() ;
}

QList<specStreamable::type> specRequiresItemAction::requiredTypes() const
{
	return QList<specStreamable::type>()
			<< specStreamable::dataItem
			<< specStreamable::folder
			<< specStreamable::logEntry
			<< specStreamable::sysEntry
			<< specStreamable::metaItem
			<< specStreamable::svgItem ;
}

QList<specStreamable::type> specRequiresDataItemAction::requiredTypes() const
{
	return QList<specStreamable::type>()
			<< specStreamable::dataItem ;
}

QList<specStreamable::type> specRequiresMetaItemAction::requiredTypes() const
{
	return QList<specStreamable::type>()
			<< specStreamable::metaItem ;
}
