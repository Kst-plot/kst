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

#ifndef LABELDIMENSIONSTAB_H
#define LABELDIMENSIONSTAB_H

#include "dialogtab.h"
#include "labelitem.h"
#include "ui_labeldimensionstab.h"

#include "kst_export.h"

namespace Kst {

class LabelDimensionsTab :public DialogTab, Ui::LabelDimensionsTab {
    Q_OBJECT
    
  public:
    explicit LabelDimensionsTab(LabelItem* labelItem, QWidget *parent);

    void setupDimensions();

    double x() {return _x->value();}
    double y() {return _y->value();}
    double rotation() {return _rotation->value();}
    bool fixLeft() {return _left->isChecked();}

    bool lockPosToData() const {return _lockPosToData->isChecked();}
    bool lockPosToDataDirty() const;

    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  public Q_SLOTS:
    void lockPosToDataChanged(bool lock_pos_to_data);
    void fixLeftChanged(bool fix_left);
    void fillDimensions(bool lock_pos_to_data, bool fix_left);

  private:
    LabelItem *_labelItem;

  private Q_SLOTS:
    void modified();

  Q_SIGNALS:
    void tabModified();
};

}
#endif // LABELDIMENSIONSTAB_H
