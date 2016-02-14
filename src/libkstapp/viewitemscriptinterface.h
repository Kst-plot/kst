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
#include "boxitem.h"
#include "circleitem.h"
#include "lineitem.h"
#include "ellipseitem.h"
#include "labelitem.h"
#include "pictureitem.h"
#include "svgitem.h"
#include "legenditem.h"
#include "plotitem.h"

#ifndef VIEWITEMSCRIPTINTERFACE_H
#define VIEWITEMSCRIPTINTERFACE_H

typedef QList<QByteArray> QByteArrayList;

namespace Kst {

class ViewItem;

struct LayoutTabSI {
    ViewItem* vi;
    QString doCommand(QString x);
};

struct FillTabSI {
    ViewItem* item;
    QString doCommand(QString x);
};

struct StrokeTabSI {
    ViewItem* item;
    QString doCommand(QString x);
};

struct DimensionTabSI {
    ViewItem* item;
    QString doCommand(QString x);
};

class ViewItemSI : public ScriptInterface
{
    Q_OBJECT
public:
    explicit ViewItemSI(ViewItem* it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate() {if (dim->item) dim->item->update();return ("Finished editing "+dim->item->Name()).toLatin1();}

    static ScriptInterface* newBox();
    static ScriptInterface* newButton();
    static ScriptInterface* newLineEdit();
    static ScriptInterface* newCircle();
    static ScriptInterface* newEllipse();
    static ScriptInterface* newLine();
    static ScriptInterface* newPicture(QByteArray file);
    static ScriptInterface* newSvgItem(QByteArray path);
  private:
    LayoutTabSI* layout;
    DimensionTabSI* dim;
    FillTabSI* fill;
    StrokeTabSI* stroke;
};

}

#endif // VIEWITEMSCRIPTINTERFACE_H
