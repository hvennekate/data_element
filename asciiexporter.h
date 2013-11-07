#ifndef ASCIIEXPORTER_H
#define ASCIIEXPORTER_H
#include <QVector>
#include <QString>
#include "specspectrumplot.h"
#include "specmodel.h"
#include "speclogwidget.h"
#include "speckineticwidget.h"
#include "specdataview.h"

class QMimeData ;

class asciiExporter
{
public:
	enum modelType { log, data, meta } ;
	asciiExporter(modelType model);
	QString content(QVector<int>) const ;
	~asciiExporter() ;
	void readFromStream(QDataStream& in) ;
private:
	specModel* modelPointer ;
	specSpectrumPlot plot ;
	specDataView view ;
	specModel itemModel ;
	specLogWidget logWidget ;
	specKineticWidget kineticWidget ;
};

#endif // ASCIIEXPORTER_H
