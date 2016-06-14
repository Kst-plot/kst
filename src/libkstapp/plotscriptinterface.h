/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Barth Netterfield                                *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PLOTSCRIPTINTERFACE_H
#define PLOTSCRIPTINTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>
#include <QMap>
#include "viewitemscriptinterface.h"
#include "plotitem.h"
typedef QList<QByteArray> QByteArrayList;

namespace Kst {

class PlotItem;

class PlotSI;
typedef QString (PlotSI::*InterfaceMemberFn)(QString& command);


class PlotSI : public ScriptInterface
{
    Q_OBJECT
public:
    explicit PlotSI(PlotItem* it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate() {if (_item) _item->update();return ("Finished editing "+_item->Name()).toLatin1();}

    static ScriptInterface* newPlot();

protected:
   QString noSuchFn(QString&) {return ""; }

private:
    LayoutTabSI *_layout;
    DimensionTabSI *_dim;
    FillTabSI *_fill;
    StrokeTabSI *_stroke;
    PlotItem *_item;
    QMap<QString,InterfaceMemberFn> _fnMap;

    QString addToCurrentView(QString& command);

    QString addRelation(QString& command);

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
    QString normalizeXtoY(QString& command);
    QString setLogX(QString& command);
    QString setLogY(QString& command);

    QString setXAxisReversed(QString& command);
    QString setYAxisReversed(QString& command);
    QString setXAxisNotReversed(QString& command);
    QString setYAxisNotReversed(QString& command);

    QString setGlobalFont(QString& command);

    QString setTopLabel(QString& command);
    QString setBottomLabel(QString& command);
    QString setLeftLabel(QString& command);
    QString setRightLabel(QString& command);

    QString setTopLabelAuto(QString& command);
    QString setBottomLabelAuto(QString& command);
    QString setLeftLabelAuto(QString& command);
    QString setRightLabelAuto(QString& command);

    QString setXAxisInterpretation(QString &command);
    QString clearXAxisInterpretation(QString &command);
    QString setXAxisDisplay(QString &command);

    //QString setYAxisInterpretation(QString &command);
    //QString clearYAxisInterpretation(QString &command);
    //QString setYAxisDisplay(QString &command);


};


}

#endif // PLOTSCRIPTINTERFACE_H
