#include "asciiexporter.h"
#include "specspectrumplot.h"
#include "specdataview.h"
#include "speclogwidget.h"
#include "speckineticwidget.h"
#include <QMimeData>

asciiExporter::asciiExporter(modelType m)
    : modelPointer(0),
      kineticWidget("")
{
    view.setModel(&itemModel) ;
    kineticWidget.view()->assignDataView(&view);
    qDebug() << "model type:" << m ;
    switch (m)
    {
    case data: modelPointer = &itemModel ; break ;
    case log:  modelPointer = logWidget.view()->model() ; break ;
    case meta: modelPointer = kineticWidget.view()->model()  ; break ;
    }
}

void asciiExporter::readFromStream(QDataStream &in)
{
    in >> plot
       >> view
       >> logWidget
       >> kineticWidget ;
}



asciiExporter::~asciiExporter()
{
}

QString asciiExporter::content(QVector<int> h) const
{
    if (!modelPointer) return QString() ;
    QVector<int> hierarchy(h.size()) ;
    for (int i = 0 ; i < h.size() ; ++i)
        hierarchy[i] = h[h.size()-1-i] ;
    QMimeData *data = modelPointer->mimeData(QModelIndexList() << modelPointer->index(hierarchy)) ;
    QString result = data->text() ;
    delete data ;
    return result ;
}
