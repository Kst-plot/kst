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

#include "labelscriptinterface.h"
#include "labelitem.h"

namespace Kst {

struct LabelTabSI {
    LabelItem* item;
    QByteArrayList commands() {
        QByteArrayList ba;
        ba<<"setLabel("<<"setLabelColor("<<"setLabelColor("<<"checkLabelItalic()"<<"uncheckLabelItalic()"<<
             "checkLabelBold()"<<"checkLabelBold()"<<"setFontSize("<<"setFontFamily(";
        return ba;
    }
    QString doCommand(QString x) {
        if(x.startsWith("setLabel(")) {
            x.remove("setLabel(").chop(1);
            item->setLabelText(x);
            return "Done";
        } else if(x.startsWith("setLabelColor(")) {
            item->setLabelColor(QColor(x.remove("setLabelColor(").remove(")")));
        } else if(x.contains("Italic")) {
            QFont f=item->labelFont();
            f.setItalic(x.contains("un"));
            item->setLabelFont(f);
            return "Done";
        } else if(x.contains("Bold")) {
            QFont f=item->labelFont();
            f.setBold(x.contains("un"));
            item->setLabelFont(f);
            return "Done";
        } else if(x.contains("setFont")) {
            QFont f=item->labelFont();
            if(x.contains("Size")) {
                item->setLabelScale(x.remove("setFontSize(").remove(")").toInt());
            } else if(x.contains("Family")) {
                f.setFamily(x.remove("setFontFamily(").remove(")"));
            } else {
                return "";
            }
            item->setLabelFont(f);
            return "Done";
        }
        return "";
    }
};

LabelSI::LabelSI(LabelItem *it) : layout(new LayoutTabSI), dim(new DimensionTabSI), lab(new LabelTabSI) {
    layout->vi=it;
    dim->item=it;
    lab->item=it;
}

QByteArrayList LabelSI::commands() {
    return layout->commands()<<dim->commands()<<lab->commands();
}

QString LabelSI::doCommand(QString x) {
    QString v=layout->doCommand(x);
    if(v.isEmpty()) {
        v=dim->doCommand(x);
    }
    if(v.isEmpty()) {
        v=lab->doCommand(x);
    }
    return v.isEmpty()?"No command":v;
}

bool LabelSI::isValid() {
    return dim->item;
}

QByteArray LabelSI::getHandle() {
    return ((QString)("Finished editing "%dim->item->Name())).toAscii();
}

}
