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

#include <QByteArray>
#include <QString>
#include <QSizeF>
#include <QList>
#include "scriptinterface.h"
#include "viewitem.h"

#ifndef VIEWITEMSCRIPTINTERFACE_H
#define VIEWITEMSCRIPTINTERFACE_H

typedef QList<QByteArray> QByteArrayList;

namespace Kst {

class ViewItem;

struct LayoutTabSI {
    ViewItem* vi;
    QByteArrayList commands();
    QString doCommand(QString x);
};

struct FillTabSI {
    ViewItem* item;
    QByteArrayList commands();
    QString doCommand(QString x);
};

struct StrokeTabSI {
    ViewItem* item;
    QByteArrayList commands();
    QString doCommand(QString x);
};

struct DimensionTabSI {
    ViewItem* item;
    QByteArrayList commands();
    QString doCommand(QString x);
};

class ViewItemSI : public ScriptInterface
{
    Q_OBJECT
public:
    ViewItemSI(ViewItem* it);
    QByteArrayList commands();
    QString doCommand(QString);
    bool isValid();
    QByteArray getHandle();
    void endEditUpdate() {if (dim->item) dim->item->update();}
  private:
    LayoutTabSI* layout;
    DimensionTabSI* dim;
    FillTabSI* fill;
    StrokeTabSI* stroke;
};

}

#endif // VIEWITEMSCRIPTINTERFACE_H
