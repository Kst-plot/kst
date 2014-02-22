/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2014 Barth Netterfield                                *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ARROWSCRIPTINTERFACE_H
#define ARROWSCRIPTINTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>

#include "viewitemscriptinterface.h"
#include "arrowitem.h"

namespace Kst {

struct ArrowTabSI;

class ArrowSI : public ScriptInterface
{
    Q_OBJECT
  public:
    explicit ArrowSI(ArrowItem* it);
    QByteArrayList commands();
    QString doCommand(QString);
    bool isValid();
    QByteArray getHandle();
    void endEditUpdate() {if (_dim->item) _dim->item->update();}

    static ScriptInterface* newArrow();

  private:
    DimensionTabSI *_dim;
    FillTabSI *_fill;
    StrokeTabSI *_stroke;
    ArrowTabSI *_arrow;
};


}

#endif // ARROWSCRIPTINTERFACE_H
