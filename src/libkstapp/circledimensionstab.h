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

#ifndef CIRCLEDIMENSIONSTAB_H
#define CIRCLEDIMENSIONSTAB_H

#include "dialogtab.h"
#include "viewitem.h"
#include "ui_circledimensionstab.h"

#include "kst_export.h"

namespace Kst {

class CircleDimensionsTab : public DialogTab, Ui::CircleDimensionsTab {
    Q_OBJECT
  public:
    explicit CircleDimensionsTab(ViewItem* viewItem, QWidget *parent);

    void setupDimensions();

    double x() {return _x->value();}
    double y() {return _y->value();}

    double radius() {return _radius->value();}
    bool radiusDirty() const;

    bool lockPosToData() const {return _lockPosToData->isChecked();}
    bool lockPosToDataDirty() const;

    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  private:
    ViewItem *_viewItem;

  private Q_SLOTS:
    void modified();

  Q_SIGNALS:
    void tabModified();
};

}
#endif // CIRCLEDIMENSIONSTAB_H
