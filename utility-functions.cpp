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
#include <QDoubleValidator>
#include "specprofiler.h"
#include "specpefile.h"
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
	QPair<QString, specDescriptor> titleLine = readJCAMPldr(title, in) ;
	if(titleLine.first != "TITLE") return QList<specModelItem*>() ;
	specModelItem* item = readJCAMPBlock(in) ;
	// setting previously read title to item
	item->changeDescriptor("", titleLine.second.content()) ;
	// TODO make description writable --> add setDescriptorFlags(...) function
	return (QList<specModelItem*>() << item) ;
}

QPair<QString, specDescriptor> readJCAMPldr(QString& first, QTextStream& in)
{
	QQueue<QString> toInterpret ;
	// get all lines neccessary
	do
	{
		toInterpret.enqueue(first) ;
		first = in.readLine() ;
	}
	while(first.left(2) != "##" && !in.atEnd()) ;

	// extract label
	int pos = toInterpret[0].indexOf("=") ;
	QString label = toInterpret[0].mid(2, pos - 2) ;
	toInterpret[0].remove(0, pos + 1) ;

	// process lines
	QString data, nl ;
	do
	{
		data += nl ;
#ifdef STRICT_JCAMP
		data += (toInterpret.dequeue()).left(80) ;  // TODO test
#else
		data += (toInterpret.dequeue());
#endif
		int commentBegins = data.indexOf("$$") ;
		if(commentBegins >= 0)
			data.truncate(commentBegins) ;
		nl = '\n' ; // TODO ???

	}
	while(!toInterpret.isEmpty()) ;


	return QPair<QString, specDescriptor> (label, specDescriptor(data)) ;
}

specModelItem* readJCAMPBlock(QTextStream& in)
{
	QString first = in.readLine() ;
	QHash<QString, specDescriptor> descriptors ;
	QList<specModelItem*> children ;
	QPair<QString, specDescriptor> ldr = readJCAMPldr(first, in) ;  // process title
	descriptors[ldr.first] = ldr.second ;
	QVector<specDataPoint> data;
	while(!in.atEnd())
	{
		QPair<QString, specDescriptor> ldr = readJCAMPldr(first, in) ;
		QString parsedLabel = ldr.first.toUpper().remove(QRegExp("[\\s\\-\\/\\_]")) ;
		if(parsedLabel == "END") break ;
		else if(parsedLabel == "TITLE")
		{
			children << readJCAMPBlock(in) ;
			children.last()->changeDescriptor("", ldr.second.content()) ;
		}
		else if(parsedLabel == "XYDATA")  // TODO keep this and interpret after total block has been read (in case any important keywords follow later)
		{
			QString content = ldr.second.content(true) ;
			QTextStream dataStream(&content) ;
			QString typeOfData = dataStream.readLine() ;
			if(typeOfData == "(X++(Y..Y))")
				readJCAMPdata(dataStream, data,
					      (descriptors["LASTX"].content().toDouble() - descriptors["FIRSTX"].content().toDouble()) / (descriptors["NPOINTS"].content().toDouble() - 1), // TODO use parsed labels
					      descriptors["XFACTOR"].content().toDouble(),
					      descriptors["YFACTOR"].content().toDouble()
					     ) ;
		}
		else
			descriptors[ldr.first] = ldr.second ;
	}
	specModelItem* item = 0 ;

	if(descriptors["DATA TYPE"].content() == "LINK")
	{
		item = new specFolderItem() ;
		// pass description down to children
		for(QHash<QString, specDescriptor>::iterator i = descriptors.begin() ; i != descriptors.end() ; i++)
			foreach(specModelItem * child, children)
			child->changeDescriptor(i.key(), i.value().content()) ;
		item->addChildren(children, 0) ;
	}
	else
	{
		item = new specDataItem(data, descriptors) ;
		foreach(specModelItem * child, children)
		delete child ;
	}
	item->invalidate();
	return item ;
}

void readJCAMPdata(QTextStream& in, QVector<specDataPoint>& data, double step, double xfactor, double yfactor)  // TODO verify final x-value
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
		line.remove(0, line.indexOf(specialCharacters)) ;
		//    	if (!xvals.isEmpty() && fabs((x.toDouble()-((xvals.last()+step)/xfactor))/ x.toDouble()) > 1e-5 ) // TODO arbitrary limit!!
		// TODO check first and last X and Y val.
		xvals << (xvals.isEmpty() ? x.toDouble() *xfactor : xvals.last() + step) ;
		// yvalue check -> (5.8.2) in paper Appl. Spec. 42(1), 151
		if(wasDiff)
		{
			xvals.remove(xvals.size() - 1) ;
			// identify, retrieve and remove item from head of string.
			int itemLength = line.indexOf(specialCharacters, 1) ;
			if(itemLength == -1) itemLength = line.size() ;
			QString yInput = line.left(itemLength) ;
			line.remove(0, itemLength) ;
			yInput.remove(QRegExp("[\\s]")) ;
			// discard if empty
			if(yInput.isEmpty()) break ;

			// identify first character and discard if NAN
			QString firstChar = yInput.left(1) ;
			yInput.remove(0, 1) ;
			if(firstChar == "?") break ;

			// interpret and discard if not equal
			if(posSQZdigits.exactMatch(firstChar))
				yInput.prepend(firstChar == "@" ? "+0" :
					       QString("%1").arg(firstChar.data()->toAscii() - 'A' + 1)) ;
			else if(negSQZdigits.exactMatch(firstChar))
				yInput.prepend(QString("-%1").arg(firstChar.data()->toAscii() - 'a' + 1)) ;
			else
				yInput.prepend(firstChar) ;

			if(yInput.toDouble() *yfactor != yvals.last())  // here no deviation ought to be allowed...
				break ; // TODO error handling
		}
		// Interpret y values
		while(!line.isEmpty())
		{
			// identify, retrieve and remove item from head of string.
			int itemLength = line.indexOf(specialCharacters, 1) ;
			if(itemLength == -1) itemLength = line.size() ;
			QString yInput = line.left(itemLength) ;
			line.remove(0, itemLength) ;
			yInput.remove(QRegExp("[\\s]")) ;
			if(yInput.isEmpty()) break ;


			// identify first character and treat according to JCAMP rules
			QString firstChar = yInput.left(1) ;
			yInput.remove(0, 1) ;
			if(firstChar == "?")
			{
				xvals << NAN ;
				line.prepend(yInput) ;
				wasDiff = false ;
				break ;
			}

			// there might really be something to interpret:
			if(posSQZdigits.exactMatch(firstChar))
			{
				yInput.prepend(firstChar == "@" ? "+0" :
					       QString("%1").arg(firstChar.data()->toAscii() - 'A' + 1)) ;
				yvals << yInput.toDouble() *yfactor ;
				wasDiff = false ;
			}
			else if(negSQZdigits.exactMatch(firstChar))
			{
				yInput.prepend(QString("-%1").arg(firstChar.data()->toAscii() - 'a' + 1)) ;
				yvals << yInput.toDouble() *yfactor ;
				wasDiff = false ;
			}
			else if(posDIFdigits.exactMatch(firstChar))
			{
				yInput.prepend(firstChar == "%" ? "+0" :
					       QString("%1").arg(firstChar.data()->toAscii() - 'I')) ;
				yvals << yInput.toDouble() *yfactor + (yvals.isEmpty() ? NAN : yvals.last()) ;
				wasDiff = true ;
			}
			else if(negDIFdigits.exactMatch(firstChar))
			{
				yInput.prepend(QString("-%1").arg(firstChar.data()->toAscii() - 'i')) ;
				yvals << yInput.toDouble() *yfactor + (yvals.isEmpty() ? NAN : yvals.last()) ;
				wasDiff = true ;
			}
			else if(posDUPdigits.exactMatch(firstChar))
			{
				yInput.prepend(firstChar == QString('s') ? "9" : QString("%1").arg(firstChar.data()->toAscii() - 'R')) ;
				int count = yInput.toInt() - 1 ;
				if(wasDiff)
				{
					if(yvals.size() >= 2)
						for(int i = 0 ; i < count ; i++)
							yvals << 2.*yvals.last() - yvals[yvals.size() - 2] ; // TODO speed opt
					else
						for(int i = 0 ; i < count ; i++)
							yvals << NAN ;
				}
				else
				{
					if(yvals.size() >= 1)
						for(int i = 0 ; i < count ; i++)
							yvals << yvals.last() ; // TODO speed opt
					else
						for(int i = 0 ; i < count ; i++)
							yvals << NAN ;
				}
			}
			// maybe just a regular number...
			else
			{
				yvals << QString("%1%2").arg(firstChar, yInput).toDouble() *yfactor ;
			}

			// fill up xvals
			while(xvals.size() < yvals.size())
				xvals << xvals.last() + step ;
		}
	}

	// turn vectors into data
	if(yvals.size() != xvals.size()) return ;  // should not be necessary

	for(int i = 0 ; i < xvals.size() ; i++)
		data << specDataPoint(xvals[i], yvals[i], 0) ;
}

QHash<QString, specDescriptor> fileHeader(QTextStream& in)
{
	QStringList headerContent = in.readLine().split(QRegExp(", (?=(\\w+\\s)*\\w+:)")) ;
	QHash<QString, specDescriptor> headerItems ;
	for(QStringList::size_type i = 0 ; i < headerContent.size() ; i++)
	{
		QString content = QStringList(headerContent[i].split(": ").mid(1)).join(": ").remove(QRegExp("^\"")).remove(QRegExp("\"$")) ;
		headerItems[headerContent[i].split(": ") [0]] = (specDescriptor(content,
			(content.contains(QRegExp("\\D"))) ? spec::editable : spec::numeric)) ;
	}
	if(headerItems["Kommentar"].content().contains("@"))
		headerItems["Pump"] = specDescriptor(headerItems["Kommentar"].content().split("@ ") [1].split(" nm") [0], spec::numeric) ;    // Pumpwellenlaenge
	return headerItems ;
}

void fileHeader(QString header, QHash<QString, specDescriptor>& description)
{
	QStringList headerContent = header.split(QRegExp(", (?=(\\w+\\s)*\\w+:)")) ;
	QHash<QString, specDescriptor> headerItems ;
	QDoubleValidator validator ;
	int a = 0 ;
	foreach(QString headerEntry, headerContent)
	{
		a = 0 ;
		QString name = headerEntry.section(": ", 0, 0),
			content = headerEntry.section(": ", 1) ;
		content.remove(QRegExp("^\"")).remove(QRegExp("\"$")) ;
		description[name] = specDescriptor(content,
						   validator.validate(content, a) == QValidator::Acceptable ?
						   spec::numeric : spec::editable) ;

	}

	if(description["Kommentar"].content().contains("@"))
		description["Pump"] = specDescriptor(description["Kommentar"].content().section("@ ", 1, -1).section(" nm", 0, 0), spec::numeric) ;    // Pumpwellenlaenge
}

QVector<double> waveNumbers(const QStringList& wns)
{
	QVector<double> wavenumbers ;
	foreach(QString wn, wns) wavenumbers += wn.toDouble() ;
	return wavenumbers ;
}

QList<specModelItem*> readHVMeasurement(const QString& measurement, QString filename)
{
	QStringList lines = measurement.split("\n") ;
	QList<specModelItem*> newItems ;
	QHash<QString, specDescriptor> headerItems ;
	headerItems["Datei"] = filename ;
	fileHeader(lines.takeFirst(), headerItems) ;
	QStringList wns = lines.takeFirst().split(" ") ;
	bool polarisatorMessung = wns.takeFirst().toInt() ;
	QVector<QVector<double> > wavenumbers ;

	QStringList::iterator wnsIt = wns.begin() ;
	for(int i = 0 ; i + 32 <= wns.size() ; i += 32)
	{
		wavenumbers << QVector<double>() ;
		for(int j = 0 ; j < 32 ; ++j)
			wavenumbers.last() << (wnsIt++)->toDouble() ;
	}

	int counter = 0 ;
	for(QStringList::iterator it = lines.begin() ; it != lines.end() ; ++it)
	{
		QStringList dataEntries = it->split(QRegExp("[ ()]"), QString::SkipEmptyParts) ;
		if(dataEntries.size() < 1) continue ;
		headerItems["Zeit"] = dataEntries.takeFirst().toDouble() ;

		QStringList::iterator entry = dataEntries.begin() ;
		for(int i = 0 ; (i + 1) * 32 * 2 <= dataEntries.size() ; ++i)
		{
			QVector<QVector<double> >::iterator currentWns = wavenumbers.begin()
				+ i / (polarisatorMessung ? 2 : 1) ;
			QVector<specDataPoint> dataPoints ;
			for(int j = 0 ; j < 32 ; ++j)
			{
				double value = (entry++)->toDouble() ;
				double mintv = (entry++)->toDouble() ;
				dataPoints << specDataPoint(currentWns->at(j), value, mintv) ;
			}
			headerItems["nu"] = (dataPoints.size() > 1) ?
					    (dataPoints.first().nu + dataPoints.last().nu) / 2. :
					    ((dataPoints.size() == 1) ? dataPoints.first().nu : NAN) ;
			if(polarisatorMessung) headerItems["Polarisation"] = i % 2 ;
			headerItems["Index"] = counter ++ ;
			newItems << new specDataItem(dataPoints, headerItems) ;
		}
	}
	return newItems ;
}



QList<specModelItem*> readHVFile(QFile& file)
{
	specProfiler profiler("Datei einlesen") ;
	QTextStream in(&file) ;
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;   // File produced on windows system
	QStringList measurements = in.readAll().split(QRegExp("\n(?=Solvens)"), QString::SkipEmptyParts) ;

	//    QList<specModelItem*> newItems ;
	QString filename = QFileInfo(file.fileName()).fileName() ;
	specFolderItem* top = new specFolderItem(0, filename) ;
	if(measurements.size() == 1) top->addChildren(readHVMeasurement(measurements.first(), filename)) ;
	else
	{
		int counter = 0 ;
		foreach(const QString & measurement, measurements)
		{
			specFolderItem* folder = new specFolderItem(0, QString::number(counter++)) ;
			folder->addChildren(readHVMeasurement(measurement, filename)) ;
			top->addChild(folder, top->children()) ;
		}
	}
	return QList<specModelItem*>() << top ;
}

QPair<QString, QString> interpretString(QString& string)
{
	if(string.left(1) == "\"")
	{
		string.remove(0, 1) ;
		int final = string.indexOf("\"") ;
		QString fileName = string.mid(0, final) ;
		string.remove(0, 2 + final) ;
		return QPair<QString, QString> ("Datei", fileName) ;
	}

	if(string.left(7) == "Messung")
	{
		bool success = (string.mid(8, 1) == "e") ;
		string.remove(0, string.indexOf(".") + 2) ;
		return QPair<QString, QString> ("Erfolg", success ? "Ja" : "Nein") ;
	}
	QString descriptor = string.mid(0, string.indexOf(": ")), content ;
	string.remove(0, qMax(string.indexOf(QRegExp("[^:\\s]"), descriptor.size()), descriptor.size())) ;
	if(string.left(1) == "\"")
	{
		content = string.mid(0, string.remove(0, 1).indexOf("\"")) ;
		string.remove(0, max(content.size() + 1, string.indexOf(QRegExp("[^,.\\s\\\"]"), content.size()))) ;
	}
	else
	{
		content = string.mid(0, string.indexOf(QRegExp("[,.\\D]{4,}"))) ;
		string.remove(0, max(content.size(), string.indexOf(QRegExp("[^,.\\s\\\"]"), content.size()))) ;
	}
	return QPair<QString, QString> (descriptor, content) ;
}

QString takeDateOrTime(QString& string)
{
	QString returnString(string.left(string.indexOf(" "))) ;
	string.remove(0, 1 + string.indexOf(" ")) ;
	string.remove(QRegExp("^: ")) ;
	return returnString ;
}

QList<specModelItem*> readLogFile(QFile& file)  // TODO revise when logentry classes are ready
{
	QTextStream in(&file) ;
	QList<specModelItem*> list ;

	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;   // File produced on windows system

	QList<specModelItem*> logData ;
	QString firstLine, secondLine ;
	while(!in.atEnd())
	{
		QHash<QString, specDescriptor> descriptors ;
		if(firstLine.isEmpty()) firstLine = in.readLine() ;
		// protection agains binary files
		for(QString::iterator i = firstLine.begin() ; i != firstLine.end() ; ++i)
		{
			if(!(QChar(*i).isPrint()))
			{
				QMessageBox::critical(0, QObject::tr("Binary File"),
						      QObject::tr("File ") +
						      file.fileName() +
						      QObject::tr(" seems to be binary.  Aborting import (found non-printable character at position ") +
						      QString::number(in.pos()) +
						      QObject::tr(").  Context:\n") + firstLine) ;
				for(QList<specModelItem*>::iterator j = logData.begin() ; j != logData.end() ; ++j)
					delete *j ;
				return QList<specModelItem*>() ;
			}
		}
		// verification of date/time string:
		QRegExp dateTimeString("^\\d\\d\\.\\d\\d\\.\\d\\d \\d\\d:\\d\\d:\\d\\d : ") ;
		if(!firstLine.isEmpty() && !dateTimeString.exactMatch(firstLine.left(20)))
		{
			QMessageBox::critical(0, QObject::tr("Not a log file"),
					      QObject::tr("File ") +
					      file.fileName() +
					      QObject::tr(" does not conform with the log file format (no date and time at beginning of a line at position ") +
					      QString::number(in.pos()) +
					      QObject::tr("). Offending line:\n")
					      + firstLine) ;
			for(QList<specModelItem*>::iterator j = logData.begin() ; j != logData.end() ; ++j)
				delete *j ;
			return QList<specModelItem*>() ;
		}
		if(!in.atEnd()) secondLine = in.readLine() ;
		descriptors["Tag"] = specDescriptor(takeDateOrTime(secondLine)) ;
		descriptors["Uhrzeit"] = specDescriptor(takeDateOrTime(secondLine)) ;
		if(firstLine == "")
		{
			while(secondLine.count("\"") % 2)
				secondLine += "\n" + in.readLine() ;
			QPair<QString, QString> newDescriptor = interpretString(secondLine) ;
			descriptors["Wert"] = newDescriptor.second ;
			QString mainDescriptor = newDescriptor.first ;
			while(!secondLine.isEmpty())
			{
				newDescriptor = interpretString(secondLine) ;
				descriptors[newDescriptor.first] = newDescriptor.second ;
			}
			logData += new specLogMessage(descriptors, 0, mainDescriptor) ;
		}
		else
		{
			descriptors["A-Tag"] = specDescriptor(takeDateOrTime(firstLine)) ;
			descriptors["A-Zeit"] = specDescriptor(takeDateOrTime(firstLine)) ;
			// set general log entry attribute
			while(firstLine.size())
			{
				QPair<QString, QString> newDescriptor = interpretString(firstLine) ;
				descriptors[newDescriptor.first] = newDescriptor.second ;
			}
			if(secondLine.left(7) == "Messung")
			{
				while(secondLine.size())
				{
					QPair<QString, QString> newDescriptor = interpretString(secondLine) ;
					descriptors[newDescriptor.first] = newDescriptor.second ;
				}
			}
			else
			{
				firstLine = descriptors["Tag"].content() + " " + descriptors["Uhrzeit"].content() + " : " + secondLine ;
				descriptors["Tag"] = descriptors["A-Tag"] ;
				descriptors["Uhrzeit"] = descriptors["A-Zeit"] ;
			}
			logData += new specLogEntryItem(descriptors) ;
		}
	}
	return logData ;
}

specFileImportFunction fileFilter(const QString& fileName)
{
	QFile file(fileName) ;
	if(!file.open(QFile::ReadOnly | QFile::Text)) return 0 ;
	QTextStream in(&file) ;
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;
	QString sample = in.readLine(5) ;
	QList<specModelItem*> (*pointer)(QFile&) = 0 ;
	if(sample == "PE IR") pointer = readPEFile ;
	else if(sample.left(4) == "PEPE") pointer = readPEBinary ;
	else if(sample == "Solve") pointer = readHVFile ;
	else if(sample == "Time ") pointer = readSKHIFile ;
	else if(sample.left(2) == "##") pointer = readJCAMPFile ;   // ???? TODO
	else
	{
		for(int i = 0 ; i < 10 ; ++i)
			sample += in.readLine() ;
		if(sample.contains(QRegExp("^\\d\\d\\.\\d\\d\\.\\d\\d \\d\\d:\\d\\d:\\d\\d : "))) pointer = readLogFile ;
		else pointer = readXYFILE ;
	}
	file.close() ;
	return pointer ;
}

QList<specModelItem*> readPEBinary(QFile& file)
{
	file.close();
	if(!file.open(QFile::ReadOnly)) return QList<specModelItem*>() ;
	specPEFile peData(file.readAll()) ;
	specDataItem dataItem(peData) ;  // TODO interface is clumsy...
	dataItem.changeDescriptor("Datei", QFileInfo(file.fileName()).fileName()) ;
	dataItem.setDescriptorProperties("Datei", spec::def);
	return QList<specModelItem*>() << new specDataItem(dataItem) ;
}

QList<specModelItem*> readPEFile(QFile& file)
{
	QTextStream in(&file) ;

	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;   // File produced on windows system
	QHash<QString, specDescriptor> headerItems ;
	headerItems["Datei"] = specDescriptor(QFileInfo(file.fileName()).fileName(), spec::def) ;
	while(in.readLine() != "#DATA") ;  // skip header

	QList<specModelItem*> specData ;
	QStringList buffer ;
	QVector<specDataPoint> dataPoints ;
	while(!in.atEnd() && (buffer = in.readLine().split(QRegExp("\\s+"))).size() > 1)
		dataPoints += specDataPoint(buffer[0].toDouble(), buffer[1].toDouble(), 0) ;
	specData += new specDataItem(dataPoints, headerItems) ;
	specData.last()->invalidate();
	return specData ;
}

QVector<double> gaussjinv(QVector<QVector<double> >& A, QVector<double>& b)
{
	QVector<bool> ipiv(A.size(), false) ;
	QVector<QVector<QVector<double> >::size_type> indxr ;
	QVector<QVector<double>::size_type> indxc ;
	for(QVector<QVector<double> >::size_type i = 0 ; i < A.size() ; i++)
	{
		QVector<QVector<double> >::size_type irow ;
		QVector<double>::size_type icol ;
		double max = 0 ;
		for(QVector<QVector<double> >::size_type j = 0 ; j < A.size() ; j++)
		{
			if(!ipiv[j])
			{
				for(QVector<double>::size_type k = 0 ; k < A[j].size() ; k++)
				{
					if(!ipiv[k] && (max = std::max(fabs(A[j][k]), max)) == fabs(A[j][k]))
					{
						irow = j ; icol = k ;
					}
				}
			}
		}
		ipiv[icol] = true ;
		if(irow != icol)
			A[irow].swap(A[icol]) ;
		indxr << irow ;
		indxc << icol ;
		double pivinv = A[icol][icol] ;
		A[icol][icol] = 1 ;
		for(QVector<double>::size_type j = 0 ; j < A[icol].size() ; j ++)
			A[icol][j] /= pivinv ;
		for(QVector<QVector<double> >::size_type j = 0 ; j < A.size() ; j++)
		{
			if(j != icol)
			{
				double dummy = A[j][icol] ;
				A[j][icol] = 0 ;
				for(QVector<double>::size_type k = 0 ; k < A[j].size() ; k++)
					A[j][k] -= A[icol][k] * dummy ;
			}
		}

	}
	// rearranging the matrix ;
	for(QVector<QVector<double>::size_type>::size_type i = indxc.size() - 1 ; i >= 0 ; i--)
		if(indxr[i] != indxc[i])
			for(QVector<QVector<double> >::size_type j = 0 ; j < A.size() ; j++)
				qSwap(A[j][indxr[i]], A[j][indxc[i]]) ;
	// applying the inverted matrix to the vector
	QVector<double> retval ;
	for(QVector<QVector<double> >::size_type i = 0 ; i < A.size() ; i++)
	{
		double dummy = 0 ;
		for(QVector<double>::size_type j = 0 ; j < A[i].size() ; j++)
			dummy += A[i][j] * b[j] ;
		retval << dummy ;
	}
	return retval ;
}

QList<specModelItem*> readSKHIFile(QFile& file)
{
	QTextStream in(&file) ;
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;   // File produced on windows system
	QVector < QPair < QPair<double, double>,
		QPair<QVector<double>, QVector<double> > > > integrale ;
	QStringList integralNamen = in.readLine().split("\t") ;
	QVector<double> zeiten ;
	integralNamen.takeFirst() ;
	integralNamen.takeFirst() ;
	foreach(QString integralName, integralNamen)
	integrale << qMakePair(qMakePair(integralName.section(" - ", 0, 0).toDouble(),
					 integralName.section(" - ", 1, 1).toDouble()),
			       qMakePair(QVector<double>(), QVector<double>()));


	QHash<QString, specDescriptor> headerItems ;
	foreach(QString descriptor, in.readLine().split(", "))
	headerItems[descriptor.section(": ", 0, 0)] = specDescriptor(descriptor.section(": ", 1, 1), spec::numeric | spec::editable) ;
	headerItems["raw"] = 1 ;
	headerItems["file"] = QFileInfo(file.fileName()).fileName() ;

	while(!in.atEnd())
	{
		QStringList firstLine = in.readLine().split("\t") ;
		QStringList secondLine = in.readLine().split("\t") ;
		if(firstLine.size() < 2 || secondLine.size() < 2) break ;
		zeiten << firstLine.takeFirst().toDouble() ;
		secondLine.takeFirst() ;
		firstLine.takeFirst() ;
		secondLine.takeFirst() ;
		for(int i = 0 ; i < integrale.size() ; ++i)
		{
			double a = firstLine.isEmpty() ? NAN : firstLine.takeFirst().toDouble() ;
			double b = secondLine.isEmpty() ? NAN : secondLine.takeFirst().toDouble() ;
			integrale[i].second.first << a ;
			integrale[i].second.second << b ;
		}
	}

	QList<specModelItem*> newItems ;
	for(int i = 0 ; i < integrale.size() ; ++i)
	{
		headerItems["begin"] = integrale[i].first.first ;
		headerItems["end"]   = integrale[i].first.second ;
		QVector<specDataPoint> rawOne, rawTwo, diff ;
		for(int j = 0 ; j < integrale[i].second.first.size() ; ++j)
		{
			double a =  integrale[i].second.first[j],
			       b = integrale[i].second.second[j],
			       t = zeiten[j] ;
			rawOne << specDataPoint(t, a, NAN) ;
			rawTwo << specDataPoint(t, b, NAN) ;
			diff   << specDataPoint(t, a - b, NAN) ;
		}
		headerItems["raw"] = 1 ;
		newItems << new specDataItem(rawOne, headerItems)
			 << new specDataItem(rawTwo, headerItems) ;
		headerItems["raw"] = 0 ;
		newItems << new specDataItem(diff, headerItems) ;
	}
	return newItems ;
}

QList<specModelItem*> readXYFILE(QFile& file)
{
	QTextStream in(&file) ;
	in.setCodec(QTextCodec::codecForName("ISO 8859-1")) ;   // File produced on windows system
	QList<specModelItem*> newItems ;

	// determine separator
	QChar separator ;
	QString content = in.readAll() ;
	if(content.contains('\t')) separator = '\t' ;
	else if(content.contains(' ')) separator = ' ' ;
	else if(content.contains(',')) separator = ',' ;
	else
	{
		QMessageBox::critical(0, QObject::tr("XY import failed"), QObject::tr("Could not find valid data field separators (tab, space, comma)")) ;
		return newItems ;
	}

	// split entries up
	QList<QStringList> entries ;
	foreach(QString line, content.split("\n"))
	entries << line.split(separator, QString::SkipEmptyParts) ;

	// determine if first line can be used as header
	QDoubleValidator validator ;
	int pos = 0 ;
	QStringList headers ;
	if(!entries.isEmpty() && !entries.first().isEmpty() && validator.validate(entries.first().first(), pos) != QValidator::Acceptable)
		headers = entries.takeFirst() ;

	// determine number of columns
	if(entries.isEmpty() || entries.first().isEmpty()) return newItems ;
	int numberOfColums = entries.first().size() ;
	int i = 0 ;
	foreach(QStringList line, entries)
	{
		++i ;
		if(line.size() != numberOfColums && line.size() > 1)
		{
			QMessageBox::critical(0, QObject::tr("XY import failed"),
					      QObject::tr("The number of columns differs in at least one line"
							  " differs from that determined for the first line. "
							  "(Error occured in line %1. Pre-determined number of"
							  " columns: %2. The line in question has %3 columns.)").
					      arg(i).arg(numberOfColums).arg(line.size())) ;
			return newItems ;
		}
	}

	// generate data
	QVector<double> xValues ;
	QVector<QVector<double> > yValues(numberOfColums - 1) ;
	foreach(QStringList line, entries)
	{
		if(!(line.size() > 1)) continue ;
		xValues << line.first().toDouble() ;
		for(int i = 0 ; i < numberOfColums - 1 ; ++i)
			yValues[i] << line[i + 1].toDouble() ;
	}

	// generate items
	QHash<QString, specDescriptor> description ;
	description["File"] = QFileInfo(file.fileName()).fileName() ;
	for(int i = 0 ; i < numberOfColums - 1 ; ++i)
	{
		if(headers.size() > i)
			description["Column"] = headers[i + (headers.size() == numberOfColums ? 1 : 0)] ;
		else if(description.contains("Column"))
			description.remove("Column") ;
		QVector<specDataPoint> points ;
		for(int j = 0 ; j < xValues.size() ; ++j)
			points << specDataPoint(xValues[j], yValues[i][j], NAN) ;
		newItems << new specDataItem(points, description) ;
	}
	return newItems ;
}

QwtSymbol* cloneSymbol(const QwtSymbol* original)
{
	if(!original) return 0 ;
	return new QwtSymbol(original->style(),
			     original->brush(),
			     original->pen(),
			     original->size()) ;
}
