//
// C++ Interface: dimentionstab
//
// Description: 
//
//
// Author: Barth Netterfield <netterfield@physics.utoronto.ca>, (C) 2008

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DIMENSIONSTAB_H
#define DIMENSIONSTAB_H

#include "dialogtab.h"
#include "viewitem.h"
#include "ui_dimensionstab.h"

#include "kst_export.h"

namespace Kst {

class DimensionsTab : public DialogTab, Ui::DimensionsTab {
  Q_OBJECT
  public:
    DimensionsTab(ViewItem* viewItem, QWidget *parent = 0);
    virtual ~DimensionsTab();

    void setupDimensions();

    double x() {return _x->value();}  
    double y() {return _y->value();}

    double width() {return _width->value();}
    bool widthDirty() const;

    double height() {return _height->value();}
    bool heightDirty() const;

    double rotation() {return _rotation->value();}
    bool rotationDirty() const;

    bool fixedAspect() {return _fixAspectRatio->isChecked();}
    bool fixedAspectDirty() const;

    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  private:
    ViewItem *_viewItem;

  private Q_SLOTS:
    void modified();
    void updateButtons();

  Q_SIGNALS:
    void tabModified();
};

}


#endif

// vim: ts=2 sw=2 et
