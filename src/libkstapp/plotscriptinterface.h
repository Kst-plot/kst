#ifndef PLOTSCRIPTINTERFACE_H
#define PLOTSCRIPTINTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>
#include <QMap>
#include "viewitemscriptinterface.h"
typedef QList<QByteArray> QByteArrayList;

namespace Kst {

class PlotItem;

class PlotSI;
typedef QString (PlotSI::*InterfaceMemberFn)(QString& command);


class PlotSI : public ScriptInterface
{
    Q_OBJECT
public:
    PlotSI(PlotItem* it);
    QByteArrayList commands();
    QString doCommand(QString);
    bool isValid();
    QByteArray getHandle();
protected:
   QString noSuchFn(QString&) {return ""; }

private:
    LayoutTabSI *_layout;
    DimensionTabSI *_dim;
    FillTabSI *_fill;
    StrokeTabSI *_stroke;
    PlotItem *_item;
    QMap<QString,InterfaceMemberFn> _fnMap;

    QStringList getArgs(const QString &command);
    QString getArg(const QString &command);

    QString setXRange(QString& command);
    QString setYRange(QString& command);
    QString setXAuto(QString& command);
    QString setYAuto(QString& command);
    QString setXAutoBorder(QString& command);
    QString setYAutoBorder(QString& command);
    QString setXNoSpike(QString& command);
    QString setYNoSpike(QString& command);
    QString setXAC(QString& command);
    QString setYAC(QString& command);

    QString setGlobalFont(QString& command);

    QString setTopLabel(QString& command);
    QString setBottomLabel(QString& command);
    QString setLeftLabel(QString& command);
    QString setRightLabel(QString& command);

    QString setTopLabelAuto(QString& command);
    QString setBottomLabelAuto(QString& command);
    QString setLeftLabelAuto(QString& command);
    QString setRightLabelAuto(QString& command);


};


}

#endif // PLOTSCRIPTINTERFACE_H
