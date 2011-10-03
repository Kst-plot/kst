#ifndef PLOTSCRIPTINTERFACE_H
#define PLOTSCRIPTINTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>
#include "viewitemscriptinterface.h"
typedef QList<QByteArray> QByteArrayList;

namespace Kst {

class PlotItem;

class PlotSI : public ScriptInterface
{
    Q_OBJECT
public:
    PlotSI(PlotItem* it);
    QByteArrayList commands();
    QString doCommand(QString);
    bool isValid();
    QByteArray getHandle();
private:
    LayoutTabSI *_layout;
    DimensionTabSI *_dim;
    FillTabSI *_fill;
    StrokeTabSI *_stroke;

};


}

#endif // PLOTSCRIPTINTERFACE_H
