#ifndef SPECPEFILE_H
#define SPECPEFILE_H

#include <QMap>
#include <QHash>
#include <QString>

class QByteArray ;
class specDataItem ;
class QVariant ;
class QString ;

class specPEFile
{
private:
	typedef QMap<int, QString> labelMapType ;
	typedef qint32 blockSize ;
	typedef qint16 innerCode ;
	typedef qint16 innerLength ;

    enum blockID
    {
        // Block IDs
        DSet2DC1DIBlock               =  120,
        HistoryRecordBlock            =  121,
        InstrHdrHistoryRecordBlock    =  122,
        InstrumentHeaderBlock         =  123,
        IRInstrumentHeaderBlock       =  124,
        UVInstrumentHeaderBlock       =  125,
        FLInstrumentHeaderBlock       =  126
    } ;

    enum typeCodeID
    {
        //     Type code IDs
        ShortType               = 29999,
        UShortType              = 29998,
        IntType                 = 29997,
        UIntType                = 29996,
        LongType                = 29995,
        BoolType                = 29988,
        CharType                = 29987,
        CvCoOrdPointType        = 29986,
        StdFontType             = 29985,
        CvCoOrdDimensionType    = 29984,
        CvCoOrdRectangleType    = 29983,
        RGBColorType            = 29982,
        CvCoOrdRangeType        = 29981,
        DoubleType              = 29980,
        CvCoOrdType             = 29979,
        ULongType               = 29978,
        PeakType                = 29977,
        CoOrdType               = 29976,
        RangeType               = 29975,
        CvCoOrdArrayType        = 29974,
        EnumType                = 29973,
        LogFontType             = 29972
    } ;

    enum dataMemberID
    {
        //     Data member IDs
        DataSetDataTypeMember              =  -29839,
        DataSetAbscissaRangeMember         =  -29838,
        DataSetOrdinateRangeMember         =  -29837,
        DataSetIntervalMember              =  -29836,
        DataSetNumPointsMember             =  -29835,
        DataSetSamplingMethodMember        =  -29834,
        DataSetXAxisLabelMember            =  -29833,
        DataSetYAxisLabelMember            =  -29832,
        DataSetXAxisUnitTypeMember         =  -29831,
        DataSetYAxisUnitTypeMember         =  -29830,
        DataSetFileTypeMember              =  -29829,
        DataSetDataMember                  =  -29828,
        DataSetNameMember                  =  -29827,
        DataSetChecksumMember              =  -29826,
        DataSetHistoryRecordMember         =  -29825,
        DataSetInvalidRegionMember         =  -29824,
        DataSetAliasMember                 =  -29823,
        DataSetVXIRAccyHdrMember           =  -29822,
        DataSetVXIRQualHdrMember           =  -29821,
        DataSetEventMarkersMember          =  -29820
    } ;

    enum historyBlockID
    {
        HistoryNumberMember              =  -29839,
        HistoryUserMember                =  -29838,
        HistoryActionTypeMember          =  -29837,
        HistoryTimestampMember           =  -29836,
        HistoryParameterMember           =  -29835,
        HistoryDescriptionMember         =  -29834,
        HistoryPreviousMember            =  -29833
    };

	template<typename t> static t readInt(const QByteArray& ba, int pos = 0) ;
	static double readDouble(const QByteArray& ba, int pos = 0) ;

	labelMapType* labelMap ;

	QMap<QString, QVariant> data ;
	QString Description ;
	static QVariant readEntry(const QByteArray&, innerCode code) ;

	QByteArray readItem(const QByteArray&, QString label = QString()) ;
	void readFromArray(const QByteArray&) ;
public:
	specPEFile(const QByteArray&);
	operator specDataItem() const;
};

#endif // SPECPEFILE_H
