/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LEGENDSCRIPTINTERFACE_H
#define LEGENDSCRIPTINTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>
#include "viewitemscriptinterface.h"
#include "legenditem.h"
typedef QList<QByteArray> QByteArrayList;

namespace Kst {

struct LegendTabSI;
class LegendItem;

class LegendSI : public ScriptInterface
{
    Q_OBJECT
public:
    explicit LegendSI(LegendItem* it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate() {if (dim->item) dim->item->update();return ("Finished editing "+dim->item->Name()).toLatin1();}
    static ScriptInterface* newLegend(QString plotname);

private:
    LayoutTabSI *layout;
    DimensionTabSI *dim;
    LegendTabSI *lab;
};


}

#endif // LEGENDSCRIPTINTERFACE_H
