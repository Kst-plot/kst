/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYOUTTAB_H
#define LAYOUTTAB_H

#include "dialogtab.h"
#include "ui_layouttab.h"

#include "kst_export.h"

namespace Kst {

class LayoutTab : public DialogTab, Ui::LayoutTab {
  Q_OBJECT
  public:
    LayoutTab(QWidget *parent = 0);
    virtual ~LayoutTab();

    double horizontalMargin() const;
    bool horizontalMarginDirty() const;
    void setHorizontalMargin(double horizontalMargin);

    double verticalMargin() const;
    bool verticalMarginDirty() const;
    void setVerticalMargin(double verticalMargin);

    double horizontalSpacing() const;
    bool horizontalSpacingDirty() const;
    void setHorizontalSpacing(double horizontalSpacing);

    double verticalSpacing() const;
    bool verticalSpacingDirty() const;
    void setVerticalSpacing(double verticalSpacing);

    void clearTabValues();
};

}

#endif

// vim: ts=2 sw=2 et
