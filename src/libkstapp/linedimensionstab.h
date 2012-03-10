/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2012 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LINEDIMENSIONSTAB_H
#define LINEDIMENSIONSTAB_H

#include "dialogtab.h"
#include "viewitem.h"
#include "ui_linedimensionstab.h"

#include "kst_export.h"

namespace Kst {

class LineDimensionsTab : public DialogTab, Ui::LineDimensionsTab {
    Q_OBJECT
  public:
    explicit LineDimensionsTab(ViewItem* viewItem, QWidget *parent);

    void setupDimensions();

    double x1() {return _p1X->value();}
    double x2() {return _p2X->value();}
    double y1() {return _p1Y->value();}
    double y2() {return _p2Y->value();}

    bool lockPosToData() const {return _lockPosToData->isChecked();}
    bool lockPosToDataDirty() const;

    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  public Q_SLOTS:
    void fillDimensions(bool lock_pos_to_data);

  private:
    ViewItem *_viewItem;

  private Q_SLOTS:
    void modified();

  Q_SIGNALS:
    void tabModified();
};

}

#endif // LINEDIMENSIONSTAB_H
