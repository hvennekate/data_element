#ifndef NAMES_H
#define NAMES_H
#include "specstream.h"


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
	enum desc {def = 0, numeric = 1, editable=2} ;
	Q_DECLARE_FLAGS(descriptorFlags,desc)
	Q_DECLARE_OPERATORS_FOR_FLAGS(descriptorFlags)
	
	/*! Enum for list item types.  Primarily for save/load purposes.*/
	enum specItemType : specStream::Type {
		folder = 1,
		logEntry = 2,
		sysEntry = 3,
		kinetic = 4,
		svg = 5,
		data = 6,
		mainWidget = 7,
		mainPlot = 8,
		mainView = 9,
		viewState = 10,
		logWidget = 11,
		kineticWidget = 12,
		actionLibrary = 13,
		undoCommand = 14,

		};
	// originial data: 0
	
	/*! Enums for export formats */
	enum separator{ nosep=0, space=1, tab=2, newline=3 } ;
	enum value{ time=0, wavenumber=1, signal=2, maxInt=3 } ;

	enum undoActionIds{ deleteId = 10,
			    newFolderId = 11,
			    moveItemsId = 12,
			    modifyDataId = 13,
			    movePlotId = 14,
			    multiMoveId = 15,
			    removeDataId   = 20,
			    insertDataId   = 21,
			    exchangeDataId = 22,
			    resizeSVGId    = 23,
			    newConnectionsId = 30,
			    deleteConnectionsId = 31,
			    editDescriptorId = 32,
			    penColorId         = 5001,
			    symbolStyleId      = 5002,
			    symbolSizeId       = 5003,
			    symbolPenColorId   = 5004,
			    symbolBrushColorId = 5005,
			    multiCommandId        =10000} ;
	enum itemRoles { activeLineRole = 50, fullContentRole = 51 } ;

	enum rtti {canvasItem = 1001, spectrum = 1010, zeroRange = 1020, kineticCurve = 1030, kineticRange = 1040, SVGItem = 1050, metaRange = 1060 } ;
}

#endif
