#include "specpefile.h"
#include <QVariant>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <cmath>
#include "specdescriptor.h"
#include "specdatapoint.h"
#include "specdataitem.h"
#include <QtEndian>

specPEFile::specPEFile(const QByteArray& ba)
	: labelMap(0)
{
	readFromArray(ba);
}

template<typename t>
t specPEFile::readInt(const QByteArray& ba, int pos)
{
	if((size_t) ba.size() < pos + sizeof(t)) return 0 ;
	return qFromLittleEndian(* ((t*)(ba.data() + pos))) ;
}

double specPEFile::readDouble(const QByteArray& ba, int pos)
{
	if((size_t) ba.size() < pos + sizeof(double)) return NAN ;
	return * ((double*)(ba.data() + pos)) ;
}

QVariant specPEFile::readEntry(const QByteArray& in, innerCode code)
{
	if(CharType == code)
	{
		innerLength length = readInt<innerLength> (in) ;
		return QString(in.mid(sizeof(length), qMin<innerLength> (length, in.size() - sizeof(length)))) ;
	}
	if(CvCoOrdRangeType == code)
		return  QList<QVariant>() << readDouble(in) << readDouble(in, sizeof(double)) ;
	if(CvCoOrdType == code || DoubleType == code)
		return readDouble(in) ;
	if(LongType == code)
		return readInt<qint32> (in) ;
	if(EnumType == code)
		return readInt<qint32> (in) ;
	if(UIntType == code)
		return readInt<quint16> (in) ;
	if(ShortType == code)
		return readInt<qint16> (in) ;
	if(ULongType == code)
		return readInt<quint32> (in) ;
	if(CvCoOrdArrayType == code)
	{
		blockSize size = qMin(readInt<blockSize> (in), in.size()) ;
		QList<QVariant> data ;
		for(int i = sizeof(blockSize) ; i < size ; i += sizeof(double))
			data << * ((double*)(in.data() + i)) ;
		return data;
	}
	return QVariant() ;
}

QByteArray specPEFile::readItem(const QByteArray& in, QString label)
{
	typedef qint16 blockType ;

	blockType type = readInt<blockType> (in) ;

	if(29970 < type)
	{
		if(!label.isEmpty())
		{
			QVariant newEntry = readEntry(in.mid(sizeof(type)), type) ;
			if(data.contains(label))
			{
				QVariant v = data[label] ;
				if(v.canConvert(QVariant::StringList))
					v = (v.toStringList() << newEntry.toString()) ;
				else
					v = (QStringList() << v.toString() << newEntry.toString()) ;
				data[label] = v ;
			}
			else
				data[label] = newEntry ;
		}
		return QByteArray() ;
	}

	blockSize size = readInt<blockSize> (in, sizeof(type)) ;
	QByteArray core(in.mid(sizeof(type) + sizeof(size), qMin((size_t) size, in.size() - sizeof(type) - sizeof(size)))),
		   remainder = in.mid(core.size() + sizeof(type) + sizeof(size)) ;

	labelMapType* oldLabels = labelMap ;

	switch(type)
	{
		case DSet2DC1DIBlock:
		case DataSetHistoryRecordMember:
		case HistoryRecordBlock:
		case InstrHdrHistoryRecordBlock:
		case IRInstrumentHeaderBlock:
			labelMap = new labelMapType ;
		default: ;
	}

	switch(type)
	{
		case DSet2DC1DIBlock:
			// change map content
			labelMap->insert(-29839, "DataSetDataTypeMember") ;
			labelMap->insert(-29838, "DataSetAbscissaRangeMember") ;
			labelMap->insert(-29837, "DataSetOrdinateRangeMember") ;
			labelMap->insert(-29836, "DataSetIntervalMember") ;
			labelMap->insert(-29835, "DataSetNumPointsMember") ;
			labelMap->insert(-29834, "DataSetSamplingMethodMember") ;
			labelMap->insert(-29833, "DataSetXAxisLabelMember") ;
			labelMap->insert(-29832, "DataSetYAxisLabelMember") ;
			labelMap->insert(-29831, "DataSetXAxisUnitTypeMember") ;
			labelMap->insert(-29830, "DataSetYAxisUnitTypeMember") ;
			labelMap->insert(-29829, "DataSetFileTypeMember") ;
			labelMap->insert(-29828, "DataSetDataMember") ;
			labelMap->insert(-29827, "DataSetNameMember") ;
			labelMap->insert(-29826, "DataSetChecksumMember") ;
			labelMap->insert(-29825, "DataSetHistoryRecordMember") ;
			labelMap->insert(-29824, "DataSetInvalidRegionMember") ;
			labelMap->insert(-29823, "DataSetAliasMember") ;
			labelMap->insert(-29822, "DataSetVXIRAccyHdrMember") ;
			labelMap->insert(-29821, "DataSetVXIRQualHdrMember") ;
			labelMap->insert(-29820, "DataSetEventMarkersMember") ;
			break ;
		case HistoryRecordBlock:
			labelMap->insert(-29839, "HistoryNumberMember") ;
			labelMap->insert(-29838, "HistoryUserMember") ;
			labelMap->insert(-29837, "HistoryActionTypeMember") ;
			labelMap->insert(-29836, "HistoryTimestampMember") ;
			labelMap->insert(-29835, "HistoryParameterMember") ;
			labelMap->insert(-29834, "HistoryDescriptionMember") ;
			labelMap->insert(-29833, "HistoryPreviousMember") ;
			break ;
		case IRInstrumentHeaderBlock:
			for(int i = -29700 ; i < -29600 ; ++i)
				labelMap->insert(i, "Instrument") ;
			break ;
		default: ;
	}

	while(!core.isEmpty())
		core = readItem(core, labelMap ? labelMap->value(type) : QString()) ;

	if(labelMap != oldLabels)
	{
		delete labelMap ;
		labelMap = oldLabels ;
	}
	return remainder ;
}

void specPEFile::readFromArray(const QByteArray& in)
{
	if(in.left(4) != "PEPE") return ;
	Description = in.mid(4, 40) ;
	readItem(in.mid(44)) ;
}

specPEFile::operator specDataItem() const
{
	QHash<QString, specDescriptor> description ;
	foreach(const QString & key, data.keys())
	{
		if(key == "DataSetAbscissaRangeMember" ||
			key == "DataSetOrdinateRangeMember" ||
			key == "DataSetIntervalMember" ||
			key == "DataSetDataMember")
			continue ;
		QVariant v = data[key] ;
		if(QVariant::Bool == v.type()
			|| QVariant::Double == v.type()
			|| QVariant::Int == v.type()
			|| QVariant::UInt == v.type()
			|| QVariant::ULongLong == v.type()
			|| QVariant::LongLong == v.type())
			description[key] = v.toDouble() ;
		else
			description[key] = v.toStringList().join("\n") ;
	}

	QVector<specDataPoint> dataPoints ;
	QList<QVariant> limits = data["DataSetAbscissaRangeMember"].toList() ;
	double begin = 0 ;
	if(!limits.isEmpty()) begin = limits.first().toDouble() ;
	double step = data["DataSetIntervalMember"].toDouble() ;
	QList<QVariant> dataList = data["DataSetDataMember"].toList() ;
	foreach(const QVariant & point, dataList)
	{
		dataPoints << specDataPoint(begin, point.toDouble() , 0.) ;
		begin += step ;
	}

	return specDataItem(dataPoints, description, 0, Description) ;
}
