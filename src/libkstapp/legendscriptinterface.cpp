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

#include "legendscriptinterface.h"

#include <QStringBuilder>

namespace Kst {

struct LegendTabSI {
    LegendItem* item;
    QByteArrayList commands() {
        QByteArrayList ba;
        ba<<"setLegendColor("<<"checkLegendItalic()"<<"uncheckLegendItalic()"<<
             "checkLegendBold()"<<"checkLegendBold()"<<"setFontSize("<<"setFontFamily(";
        return ba;
    }
    QString doCommand(QString x) {
        if(x.startsWith("setLegendColor(")) {
            item->setLegendColor(QColor(x.remove("setLegendColor(").remove(')')));
        } else if(x.contains("Italic")) {
            QFont f=item->font();
            f.setItalic(!x.contains("un"));
            item->setLegendFont(f);
            return "Done";
        } else if(x.contains("Bold")) {
            QFont f=item->font();
            f.setBold(!x.contains("un"));
            item->setLegendFont(f);
            return "Done";
        } else if(x.contains("setFont")) {
            QFont f=item->font();
            if(x.contains("Size")) {
                item->setFontScale(x.remove("setFontSize(").remove(')').toInt());
            } else if(x.contains("Family")) {
                f.setFamily(x.remove("setFontFamily(").remove(')'));
            } else {
                return "";
            }
            item->setLegendFont(f);
            return "Done";
        }
        return "";
    }
};

LegendSI::LegendSI(LegendItem *it) : layout(new LayoutTabSI), dim(new DimensionTabSI), lab(new LegendTabSI) {
    layout->vi=it;
    dim->item=it;
    lab->item=it;
}

QString LegendSI::doCommand(QString x) {

  QString v=doNamedObjectCommand(x, dim->item);

  if (v.isEmpty()) {
    v=layout->doCommand(x);
  }
  if (v.isEmpty()) {
    v=dim->doCommand(x);
  }
  if (v.isEmpty()) {
    v=lab->doCommand(x);
  }
  return v.isEmpty()?"No command":v;
}

bool LegendSI::isValid() {
    return dim->item;
}

ScriptInterface* LegendSI::newLegend(QString plotname) {
    PlotItem *plot = ViewItem::retrieveItem<PlotItem>(plotname);
    LegendItem* li = plot->legend();

    plot->setShowLegend(true, true);

    return new LegendSI(li);
}



}
