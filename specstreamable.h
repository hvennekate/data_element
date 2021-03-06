#ifndef SPECSTREAMABLE_H
#define SPECSTREAMABLE_H

#include <QDataStream>

// TODO Pimpl this class.

class specStreamable
{
public:
	friend QDataStream& operator<< (QDataStream& out, const specStreamable&) ;
	friend QDataStream& operator>> (QDataStream& in, specStreamable&) ;
	virtual ~specStreamable() {}
	void readDirectly(QDataStream& in) { readFromStream(in) ; }   // TODO improve; make factory available broadly
	typedef quint16 type;
	enum streamableType
	{
	    none = 0,
	    folder = 1,
	    logEntry = 2,
	    sysEntry = 3,
	    kineticItem = 4,
	    svgItem = 5,
	    legacyDataItem = 6,
	    mainWidget = 7,
	    mainPlot = 8,
	    mainView = 9,
	    viewState = 10,
	    logWidget = 11,
	    kineticWidget = 12,
	    actionLibrary = 13,
	    undoCommand = 14,
	    deleteCommandId = 15,
	    newFolderCommandId = 16,
	    moveItemsCommandId = 17,
	    movePlotCommandId = 19,
	    exchangeDataCommandId = 23,
	    resizeSVGCommandId    = 24,
	    newConnectionsCommandId = 31,
	    deleteConnectionsCommandId = 32,
	    editDescriptorCommandId = 33,
	    penColorCommandId         = 34,
	    symbolStyleCommandId      = 35,
	    symbolSizeCommandId       = 36,
	    symbolPenColorCommandId   = 37,
	    symbolBrushColorCommandId = 38,
	    manageConnectionsCommandId = 39,
	    manageItemsCommandId = 40,
	    dataView = 41,
	    multiCommandId = 42,
	    genealogyId = 43,
	    model = 44,
	    legacyDescriptor = 45,
	    plotStyle = 46,
	    metaItem = 47,
	    metaModel = 48,
	    metaView = 49,
	    metaWidget = 50,
	    logModel = 51,
	    logView = 52,
	    range = 53,
	    metaRange = 54,
	    metaRangeCommand = 55,
	    lineWidthCommandId = 58,
	    plotTitleCommandId = 59,
	    plotYLabelCommandId = 60,
	    plotXLabelCommandId = 61,
	    fitCurve = 62,
	    exchangeFitCommand = 63,
	    toggleFitStyleCommand = 64,
	    penStyleCommandId = 65,
	    descriptorFlagsCommand = 66,
	    dataItem = 67,
	    legacyDeleteDescriptorCommandId = 68,
	    renameDescriptorCommandId = 69,
	    exchangeFilterCommandId = 70,
	    descriptor = 71,
	    multilineCommandId = 72,
	    deleteDescriptorCommandId = 73,
	};
	virtual type typeId() const = 0;
	virtual bool alternativeType(type t) const {Q_UNUSED(t) ; return false ; }
protected:
	virtual void writeToStream(QDataStream& out) const = 0;
	virtual void readFromStream(QDataStream& in) = 0;
	virtual void readAlternative(QDataStream& in, type t) {Q_UNUSED(in) ; Q_UNUSED(t) }
	virtual specStreamable* factory(const type& t) const {Q_UNUSED(t) ; return 0; }   // to be implemented in parent!
	specStreamable* produceItem(QDataStream& in) const;
private:
	type effectiveId() const ;
};

QDataStream& operator<< (QDataStream& out, const specStreamable&) ;
QDataStream& operator>> (QDataStream& in, specStreamable&) ;

#endif // SPECSTREAMABLE_H
