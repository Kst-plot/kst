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

#include "arrowscriptinterface.h"

#include <QStringBuilder>

namespace Kst {

struct ArrowTabSI {
    ArrowItem* item;
    QByteArrayList commands() {
      QByteArrayList ba;
      ba<<"arrowAtStart("<<"arrowAtEnd("<<"arrowHeadScale(";
      return ba;
    }
    QString doCommand(QString x) {
      if (x.startsWith("arrowAtStart(")) {
        x.remove("arrowAtStart(").chop(1);
        item->setStartArrowHead(x.toLower()=="true");
        return "Done";
      } else if (x.startsWith("arrowAtEnd(")) {
        x.remove("arrowAtEnd(").chop(1);
        item->setEndArrowHead(x.toLower()=="true");
        return "Done";
      } else if (x.startsWith("arrowHeadScale(")) {
        x.remove("arrowHeadScale(").chop(1);
        double scale = qMax(3.0, x.toDouble());
        item->setEndArrowScale(scale);
        item->setStartArrowScale(scale);
        return "Done";
      }
      return "";
    }
};

ArrowSI::ArrowSI(ArrowItem *it) : _dim(new DimensionTabSI), _fill(new FillTabSI), _stroke(new StrokeTabSI), _arrow(new ArrowTabSI) {
  _dim->item=it;
  _fill->item=it;
  _stroke->item=it;
  _arrow->item=it;
}

QString ArrowSI::doCommand(QString x) {

  QString v=doNamedObjectCommand(x, _dim->item);

  if(v.isEmpty()) {
    v =_dim->doCommand(x);
  }
  if(v.isEmpty()) {
    v=_fill->doCommand(x);
  }
  if(v.isEmpty()) {
    v=_stroke->doCommand(x);
  }
  if(v.isEmpty()) {
    v=_arrow->doCommand(x);
  }
  return v.isEmpty()?"No command":v;
}

bool ArrowSI::isValid() {
  return _dim->item;
}

ScriptInterface* ArrowSI::newArrow() {
  ArrowItem* bi=new ArrowItem(kstApp->mainWindow()->tabWidget()->currentView());
  kstApp->mainWindow()->tabWidget()->currentView()->scene()->addItem(bi);
  return new ArrowSI(bi);
}


}
