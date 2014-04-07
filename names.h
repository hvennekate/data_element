#ifndef NAMES_H
#define NAMES_H

#define STRINGIFYMACRO(x) MAKESTRINGMACRO(x)
#define MAKESTRINGMACRO(x) #x

class specModelItem ;
class QFile ;

/*! Namespace (mainly for enums) */
namespace spec
{
	/*! Enum for descriptors.*/
	enum desc {def = 0, numeric = 1, editable = 2, multiline = 4} ;
	Q_DECLARE_FLAGS(descriptorFlags, desc)
	Q_DECLARE_OPERATORS_FOR_FLAGS(descriptorFlags)

	// originial dataItem enum value: 0

	/*! Enums for export formats */
	enum separator { nosep = 0, space = 1, tab = 2, newline = 3 } ;
	enum value { wavenumber = 0, signal = 1, maxInt = 2 } ;

	enum undoActionIds { } ;
	enum itemRoles { descriptorPropertyRole = 34 } ;

	enum rtti {canvasItem = 1001, spectrum = 1010, zeroRange = 1020, metaItem = 1030, kineticRange = 1040, SVGItem = 1050, metaRange = 1060 } ;
	enum correctionMode { noCorrection = -1, offset = 0, offsetAndSlope = 1 } ;
}

typedef QList<specModelItem*> (* specFileImportFunction)(QFile&)  ;

#endif
