#include <utility-functions.h>
#include <QTextCodec>
#include <QFileInfo>
#include <QPair>
#include <math.h>
#include <names.h>
#include "specdescriptor.h"
#include "speclogentryitem.h"
#include "speclogmessage.h"
#include <QQueue>
#include <QtDebug>
#include <QPointF>
#include "specdatapoint.h"
#include <QMessageBox>
#include <QObject>
using std::max ;
using std::min ;

// bool roundedEqual(double a, double b)
// {
// 	int e = min(log10(a),log10(b)) ;
// 	double ma = a /pow(10.,e) ;
// 	double mb = b /pow(10.,e) ;
// 	int d = 0 ;
// 	while ( (int) (ma*pow(10.,d)) != ma*pow(10.,d) && (int) (mb*pow(10.,d)) != mb*pow(10.,d) )
// 	return (round(ma*pow(10.,d)) == round(mb*pow(10.,d))) ;
// }

bool comparePoints(const QPointF& a, const QPointF& b)
{
	return a.x() < b.x() ;
}

QList<specModelItem*> readJCAMPFile(QFile& file)
{
	// Preparing stream
	QTextStream in(&file) ;
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;
	// Reading title of first item in order to verify JCAMP format
	QString title = in.readLine() ;
	QPair<QString,specDescriptor> titleLine = readJCAMPldr(title,in) ;
	if (titleLine.first != "TITLE") return QList<specModelItem*>() ;
	specModelItem *item = readJCAMPBlock(in) ;
	// setting previously read title to item
	item->changeDescriptor("",titleLine.second.content()) ;
	// TODO make description writable --> add setDescriptorFlags(...) function
	return (QList<specModelItem*>() << item) ;
}

QPair<QString,specDescriptor> readJCAMPldr(QString &first,QTextStream &in)
{
	QQueue<QString> toInterpret ;
	// get all lines neccessary
	do
	{
		toInterpret.enqueue(first) ;
		first = in.readLine() ;
	} while (first.left(2) != "##" && !in.atEnd()) ;
	
	// extract label
	int pos = toInterpret[0].indexOf("=") ;
	QString label = toInterpret[0].mid(2,pos-2) ;
	toInterpret[0].remove(0,pos+1) ;
	
	// process lines
	QString data, nl ;
	do
	{
		data += nl ;
#ifdef STRICT_JCAMP
		data += (toInterpret.dequeue()).left(80) ; // TODO test
#else
		data += (toInterpret.dequeue());
#endif
		int commentBegins = data.indexOf("$$") ;
		if (commentBegins >= 0)
			data.truncate(commentBegins) ;
		nl = '\n' ; // TODO ???
		
	} while (!toInterpret.isEmpty()) ;
	
	
	return QPair<QString,specDescriptor>(label,specDescriptor(data)) ;
}

specModelItem* readJCAMPBlock(QTextStream& in)
{
	QString first = in.readLine() ;
	QHash<QString,specDescriptor> descriptors ;
	QList<specModelItem*> children ;
	QPair<QString,specDescriptor> ldr = readJCAMPldr(first,in) ; // process title
	descriptors[ldr.first] = ldr.second ;
	QVector<specDataPoint> data;
	while (!in.atEnd())
	{
		QPair<QString,specDescriptor> ldr = readJCAMPldr(first,in) ;
		QString parsedLabel = ldr.first.toUpper().remove(QRegExp("[\\s\\-\\/\\_]")) ;
		if (parsedLabel == "END") break ;
		else if (parsedLabel == "TITLE")
		{
			children << readJCAMPBlock(in) ;
			children.last()->changeDescriptor("",ldr.second.content()) ;
		}
		else if (parsedLabel == "XYDATA") // TODO keep this and interpret after total block has been read (in case any important keywords follow later)
		{
			QString content = ldr.second.content() ;
			QTextStream dataStream(&content) ;
			QString typeOfData = dataStream.readLine() ;
			if (typeOfData=="(X++(Y..Y))")
				readJCAMPdata(dataStream,data,
					      (descriptors["LASTX"].content().toDouble()-descriptors["FIRSTX"].content().toDouble())/(descriptors["NPOINTS"].content().toDouble()-1), // TODO use parsed labels
					       descriptors["XFACTOR"].content().toDouble(),
					       descriptors["YFACTOR"].content().toDouble()
					     ) ;
		}
		else
			descriptors[ldr.first] = ldr.second ;
	}
	specModelItem *item = 0 ;
	
	if(descriptors["DATA TYPE"].content() == "LINK")
	{
		item = new specFolderItem() ;
		// pass description down to children
		for(QHash<QString,specDescriptor>::iterator i = descriptors.begin() ; i != descriptors.end() ; i++)
			foreach (specModelItem *child, children)
				child->changeDescriptor(i.key(),i.value().content()) ;
		item->addChildren(children,0) ;
	}
	else
	{
		item = new specDataItem(data,descriptors) ;
		foreach(specModelItem *child, children)
			delete child ;
	}
	item->invalidate();
	return item ;
}

void readJCAMPdata(QTextStream& in, QVector<specDataPoint>& data, double step, double xfactor, double yfactor) // TODO verify final x-value
{
	QRegExp specialCharacters("[?@%A-DF-Za-df-s+\\-\\s]|[Ee](?![+\\-\\s])"),
		posSQZdigits("[A-DF-I@]|E(?![+\\-\\s])"),
		negSQZdigits("[a-df-i]|e(?![+\\-\\s])"),
		posDIFdigits("[J-R%]"),
		negDIFdigits("[j-r]"),
		posDUPdigits("[S-Zs]") ;
	bool wasDiff = false ;
	QVector<double> yvals, xvals ;
	while(!in.atEnd())
	{
		QString line = in.readLine() ;
		QString x = line.left(line.indexOf(specialCharacters)) ;
		line.remove(0,line.indexOf(specialCharacters)) ;
		if (!xvals.isEmpty() && fabs((x.toDouble()-((xvals.last()+step)/xfactor))/ x.toDouble()) > 1e-5 ) // TODO arbitrary limit!!
			// TODO check first and last X and Y val.
		xvals << (xvals.isEmpty() ? x.toDouble()*xfactor : xvals.last()+step) ;
		// yvalue check -> (5.8.2) in paper Appl. Spec. 42(1), 151
		if(wasDiff)
		{
			xvals.remove(xvals.size()-1) ;
			// identify, retrieve and remove item from head of string.
			int itemLength = line.indexOf(specialCharacters,1) ;
			if (itemLength == -1) itemLength = line.size() ;
			QString yInput = line.left(itemLength) ;
			line.remove(0,itemLength) ;
			yInput.remove(QRegExp("[\\s]")) ;
			// discard if empty
			if (yInput.isEmpty()) break ;
			
			// identify first character and discard if NAN
			QString firstChar = yInput.left(1) ;
			yInput.remove(0,1) ;
			if (firstChar == "?") break ;
			
			// interpret and discard if not equal
			if (posSQZdigits.exactMatch(firstChar))
				yInput.prepend(firstChar == "@" ? "+0" :
						QString("%1").arg(firstChar.data()->toAscii()-'A'+1)) ;
			else if (negSQZdigits.exactMatch(firstChar))
				yInput.prepend(QString("-%1").arg(firstChar.data()->toAscii()-'a'+1)) ;
			else
				yInput.prepend(firstChar) ;
			
			if (yInput.toDouble()*yfactor != yvals.last()) // here no deviation ought to be allowed...
				break ; // TODO error handling
		}
		// Interpret y values
		while(!line.isEmpty())
		{
			// identify, retrieve and remove item from head of string.
			int itemLength = line.indexOf(specialCharacters,1) ;
			if (itemLength == -1) itemLength = line.size() ;
			QString yInput = line.left(itemLength) ;
			line.remove(0,itemLength) ;
			yInput.remove(QRegExp("[\\s]")) ;
			if (yInput.isEmpty()) break ;
			
			
			// identify first character and treat according to JCAMP rules
			QString firstChar = yInput.left(1) ;
			yInput.remove(0,1) ;
			if (firstChar == "?")
			{
				xvals << NAN ;
				line.prepend(yInput) ;
				wasDiff = false ;
				break ;
			}
			
			// there might really be something to interpret:
			if (posSQZdigits.exactMatch(firstChar))
			{
				yInput.prepend(firstChar == "@" ? "+0" :
					QString("%1").arg(firstChar.data()->toAscii()-'A'+1)) ;
				yvals << yInput.toDouble()*yfactor ;
				wasDiff = false ;
			}
			else if (negSQZdigits.exactMatch(firstChar))
			{
				yInput.prepend(QString("-%1").arg(firstChar.data()->toAscii()-'a'+1)) ;
				yvals << yInput.toDouble()*yfactor ;
				wasDiff = false ;
			}
			else if (posDIFdigits.exactMatch(firstChar))
			{
				yInput.prepend(firstChar == "%" ? "+0" :
						QString("%1").arg(firstChar.data()->toAscii()-'I')) ;
				yvals << yInput.toDouble()*yfactor + (yvals.isEmpty() ? NAN : yvals.last()) ;
				wasDiff = true ;
			}
			else if (negDIFdigits.exactMatch(firstChar))
			{
				yInput.prepend(QString("-%1").arg(firstChar.data()->toAscii()-'i')) ;
				yvals << yInput.toDouble()*yfactor + (yvals.isEmpty() ? NAN : yvals.last()) ;
				wasDiff = true ;
			}
			else if (posDUPdigits.exactMatch(firstChar))
			{
				yInput.prepend(firstChar == QString('s') ? "9" : QString("%1").arg(firstChar.data()->toAscii()-'R')) ;
				int count = yInput.toInt()-1 ;
				if (wasDiff)
				{
					if(yvals.size() >= 2)
						for (int i = 0 ; i < count ; i++)
							yvals << 2.*yvals.last() - yvals[yvals.size()-2] ; // TODO speed opt
					else
						for (int i = 0 ; i < count ; i++)
							yvals << NAN ;
				}
				else
				{
					if(yvals.size() >= 1)
						for (int i = 0 ; i < count ; i++)
							yvals << yvals.last() ; // TODO speed opt
					else
						for (int i = 0 ; i < count ; i++)
							yvals << NAN ;
				}
			}
			// maybe just a regular number...
			else
			{
				yvals << QString("%1%2").arg(firstChar,yInput).toDouble()*yfactor ;
			}
			
			// fill up xvals
			while(xvals.size() < yvals.size())
				xvals << xvals.last() + step ;
		}
	}
	
	// turn vectors into data
	if (yvals.size() != xvals.size()) return ; // should not be necessary
	
	for (int i = 0 ; i < xvals.size() ; i++)
		data << specDataPoint(0,xvals[i],yvals[i],0) ;
}

QHash<QString,specDescriptor> fileHeader(QTextStream& in) {
	QStringList headerContent = in.readLine().split(QRegExp(", (?=(\\w+\\s)*\\w+:)")) ;
	QHash<QString,specDescriptor> headerItems ;
	for (QStringList::size_type i=0 ; i < headerContent.size() ; i++)
	{
		QString content = QStringList(headerContent[i].split(": ").mid(1)).join(": ").remove(QRegExp("^\"")).remove(QRegExp("\"$")) ;
		headerItems[headerContent[i].split(": ")[0]] = (specDescriptor(content,
				(content.contains(QRegExp("\\D"))) ? spec::editable : spec::numeric)) ;
	}
	if (headerItems["Kommentar"].content().contains("@"))
		headerItems["Pump"] = specDescriptor(headerItems["Kommentar"].content().split("@ ")[1].split(" nm")[0],spec::numeric) ; // Pumpwellenlaenge
	return headerItems ;
}

QVector<double> waveNumbers(const QStringList& wns) {
	QVector<double> wavenumbers ;
	foreach(QString wn, wns) wavenumbers += wn.toDouble() ;
	return wavenumbers ;
}

QList<specModelItem*> readHVFile(QFile& file)
{
	QTextStream in(&file) ;
	
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ; // File produced on windows system
	
	QHash<QString,specDescriptor> headerItems = fileHeader(in) ;
	headerItems["Datei"] = specDescriptor(QFileInfo(file.fileName()).fileName(),spec::def) ;
	QStringList wns = in.readLine().split(" ") ;
	bool polarisatorMessung = wns.takeFirst().toInt() ; // First value before wavenumbers denotes pol measurement
	QVector<double> wavenumbers = waveNumbers(wns) ;
	
	QList<specModelItem*> specData, otherPolarisation ;
	while(!in.atEnd())
	{
		QStringList templist = in.readLine().split(" ").replaceInStrings("(","").replaceInStrings(")","") ;
		headerItems["Zeit"] = specDescriptor(templist.takeFirst().toDouble()) ;
		QVector<double> data ;
		data << headerItems["Zeit"].numericValue() << 0. << 0. << 0. ;

		QVector<specDataPoint> dataPoints ;
		for(QStringList::size_type i = 0 ; 2*i+1 < templist.size() ; i++)
		{
			data[1] = wavenumbers[polarisatorMessung ? 32*((i/32)/2)+i%32 : i] ; // Funny formula for doing the wavenumbers once for each polarisation
			data[2] = templist[2*i].toDouble() ;
			data[3] = templist[2*i+1].toDouble() ;
			dataPoints += specDataPoint(data) ;
		}
		for (QVector<double>::size_type i = 0 ; i < dataPoints.size() ; i += 32)
		{
			headerItems["nu"] = specDescriptor((dataPoints[i].nu+dataPoints[i+31].nu)/2.) ;
			if (!polarisatorMessung)
				specData += new specDataItem(dataPoints.mid(i,32),headerItems) ;
			else
			{
				bool polarisation = (i/32)%2 ;
				headerItems["polarisation"] = specDescriptor(polarisation) ;
//				(polarisation ? otherPolarisation : specData) << new specDataItem(dataPoints.mid(i,32),headerItems) ;
				specData << new specDataItem(dataPoints.mid(i,32),headerItems) ;
			}
			specData.last()->mergePlotData = false ;
			specData.last()->invalidate(); ;
		}
	}
	return specData + otherPolarisation ;
}

QPair<QString,QString> interpretString(QString& string)
{
	if(string.left(1) == "\"")
	{
		string.remove(0,1) ;
		int final = string.indexOf("\"") ;
		QString fileName = string.mid(0,final) ;
		string.remove(0,2+final) ;
		return QPair<QString,QString>("Datei",fileName) ;
	}
	
	if (string.left(7) == "Messung")
	{
		bool success = (string.mid(8,1) =="e") ;
		string.remove(0,string.indexOf(".")+2) ;
		return QPair<QString,QString>("Erfolg",success ? "Ja" : "Nein") ;
	}
	QString descriptor = string.mid(0,string.indexOf(": ")), content ;
	string.remove(0,string.indexOf(QRegExp("[^:\\s]"),descriptor.size())) ;
	if(string.left(1) == "\"")
	{
		content = string.mid(0,string.remove(0,1).indexOf("\"")) ;
		string.remove(0,max(content.size(),string.indexOf(QRegExp("[^,.\\s\\\"]"),content.size()))) ;
	}
	else
	{
		content = string.mid(0,string.indexOf(QRegExp("[,.\\D]{4,}"))) ;
		string.remove(0,max(content.size(),string.indexOf(QRegExp("[^,.\\s\\\"]"),content.size()))) ;
	}
	return QPair<QString,QString>(descriptor,content) ;
}

QString takeDateOrTime(QString& string)
{
	QString returnString(string.left(string.indexOf(" "))) ;
	string.remove(0,1+string.indexOf(" ")) ;
	string.remove(QRegExp("^: ")) ;
	return returnString ;
}

QList<specModelItem*> readLogFile(QFile& file) // TODO revise when logentry classes are ready
{
	QTextStream in(&file) ;
	QList<specModelItem*> list ;
	
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ; // File produced on windows system
	
	QList<specModelItem*> logData ;
	while(!in.atEnd())
	{
		QHash<QString,specDescriptor> descriptors ;
		QString firstLine = in.readLine(), secondLine ;
		for (QString::iterator i = firstLine.begin() ; i != firstLine.end() ; ++i)
		{
			if (!(QChar(*i).isPrint()))
			{
				QMessageBox::critical(0,QObject::tr("Binary File"),
						      QObject::tr("File ") +
						      file.fileName() +
						      QObject::tr("seems to be binary.  Aborting Import (found non-printable character at position ") +
						      QString::number(in.pos()) +
						      QObject::tr(").")) ;
				for (QList<specModelItem*>::iterator i = logData.begin() ; i != logData.end() ; ++i)
					delete *i ;
				return QList<specModelItem*>() ;
			}
		}
		if (!in.atEnd()) secondLine = in.readLine() ;
		descriptors["Tag"] = specDescriptor(takeDateOrTime(secondLine)) ;
		descriptors["Uhrzeit"] = specDescriptor(takeDateOrTime(secondLine)) ;
		if (firstLine == "")
		{
			QPair<QString,QString> newDescriptor = interpretString(secondLine) ;
			descriptors["Wert"] = newDescriptor.second ;
			logData += new specLogMessage(descriptors,0, newDescriptor.first) ;
		}
		else
		{
			descriptors["A-Tag"] = specDescriptor(takeDateOrTime(firstLine)) ;
			descriptors["A-Zeit"] = specDescriptor(takeDateOrTime(firstLine)) ;
			// set general log entry attribute
			while(firstLine.size())
			{
				QPair<QString,QString> newDescriptor = interpretString(firstLine) ;
				descriptors[newDescriptor.first] = newDescriptor.second ;
			}
			while(secondLine.size())
			{
				QPair<QString,QString> newDescriptor = interpretString(secondLine) ;
				descriptors[newDescriptor.first] = newDescriptor.second ;
			}
			logData += new specLogEntryItem(descriptors) ;
		}
	}
	return logData ;
}

QList<specModelItem*> (*fileFilter(const QString& fileName)) (QFile&)
{
	QFile file(fileName) ;
	file.open(QFile::ReadOnly | QFile::Text) ;
	QTextStream in(&file) ;
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;
	QString sample = in.readLine(5) ;
	QList<specModelItem*> (*pointer)(QFile&) = 0 ;
	if (sample == "PE IR") pointer = readPEFile ;
	else if (sample == "Solve") pointer = readHVFile ;
	else if (sample == "##") pointer = readJCAMPFile ;
	else pointer = readLogFile ;
	file.close() ;
	return pointer ;
}

QList<specModelItem*> readPEFile(QFile& file)
{
	QTextStream in(&file) ;
	
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ; // File produced on windows system
	QHash<QString,specDescriptor> headerItems ;
	headerItems["Datei"] = specDescriptor(QFileInfo(file.fileName()).fileName(),spec::def) ;
	while(in.readLine() != "#DATA") ; // skip header
	
	QList<specModelItem*> specData ;
	QStringList buffer ;
	QVector<specDataPoint> dataPoints ;
	while(!in.atEnd() && (buffer = in.readLine().split(QRegExp("\\s+"))).size() >1)
		dataPoints += specDataPoint(0,buffer[0].toDouble(),buffer[1].toDouble(),0) ;
	specData += new specDataItem(dataPoints,headerItems) ;
	specData.last()->invalidate();
	return specData ;
}

QList<double> gaussjinv(QList<QList<double> >& A, QList<double>& b)
{
	QVector<bool> ipiv(A.size(),false) ;
	QList<QList<QList<double> >::size_type> indxr ;
	QList<QList<double>::size_type> indxc ;
	for (QList<QList<double> >::size_type i = 0 ; i < A.size() ; i++)
	{
		QList<QList<double> >::size_type irow ;
		QList<double>::size_type icol ;
		double max = 0 ;
		for (QList<QList<double> >::size_type j = 0 ; j < A.size() ; j++)
		{
			if (!ipiv[j])
			{
				for(QList<double>::size_type k = 0 ; k < A[j].size() ; k++)
				{
					if (!ipiv[k] && (max = std::max(fabs(A[j][k]),max)) == fabs(A[j][k]))
					{
						irow = j ; icol = k ;
					}
				}
			}
		}
		ipiv[icol] = true ;
		if (irow != icol)
			A.swap(irow,icol) ;
		indxr << irow ;
		indxc << icol ;
		double pivinv = A[icol][icol] ;
		A[icol][icol] = 1 ;
		for (QList<double>::size_type j = 0 ; j < A[icol].size() ; j ++)
			A[icol][j] /= pivinv ;
		for (QList<QList<double> >::size_type j = 0 ; j < A.size() ; j++)
		{
			if (j != icol)
			{
				double dummy = A[j][icol] ;
				A[j][icol] = 0 ;
				for (QList<double>::size_type k = 0 ; k < A[j].size() ; k++)
					A[j][k] -= A[icol][k]*dummy ;
			}
		}
		
	}
// rearranging the matrix ;	
	for(QList<QList<double>::size_type>::size_type i = indxc.size() -1 ; i >= 0 ; i--)
		if (indxr[i] != indxc[i])
			for(QList<QList<double> >::size_type j = 0 ; j < A.size() ; j++)
				A[j].swap(indxr[i],indxc[i]) ;
// applying the inverted matrix to the vector
	QList<double> retval ;
	for (QList<QList<double> >::size_type i = 0 ; i < A.size() ; i++)
	{
		double dummy = 0 ;
		for (QList<double>::size_type j = 0 ; j < A[i].size() ; j++)
			dummy += A[i][j]*b[j] ;
		retval << dummy ;
	}
	return retval ;
}
