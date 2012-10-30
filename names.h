#ifndef NAMES_H
#define NAMES_H


/*! Namespace (mainly for enums) */
namespace spec{
	/*! Enum for specifying the mode of correction for spectra*/
	enum move { none = 0,
			shift=1,
   			scale=2,
      			slope=4,
	 		shiftX = 8,
			newZero= 16,
			individualZero=32} ;
	Q_DECLARE_FLAGS(moveMode,move)
	Q_DECLARE_OPERATORS_FOR_FLAGS(moveMode)
	
	/*! Enum for descriptors.*/
    enum desc {def = 0, numeric = 1, editable=2, multiline=4} ;
	Q_DECLARE_FLAGS(descriptorFlags,desc)
	Q_DECLARE_OPERATORS_FOR_FLAGS(descriptorFlags)
	
	// originial dataItem enum value: 0
	
	/*! Enums for export formats */
	enum separator{ nosep=0, space=1, tab=2, newline=3 } ;
	enum value{ time=0, wavenumber=1, signal=2, maxInt=3 } ;

	enum undoActionIds{ } ;
	enum itemRoles { activeLineRole = 50, fullContentRole = 51 } ;

	enum rtti {canvasItem = 1001, spectrum = 1010, zeroRange = 1020, metaItem = 1030, kineticRange = 1040, SVGItem = 1050, metaRange = 1060 } ;
}

#endif
