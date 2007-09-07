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

class KST_EXPORT LayoutTab : public DialogTab, Ui::LayoutTab {
  Q_OBJECT
  public:
    LayoutTab(QWidget *parent = 0);
    virtual ~LayoutTab();

    double horizontalMargin() const;
    void setHorizontalMargin(double horizontalMargin);

    double verticalMargin() const;
    void setVerticalMargin(double verticalMargin);

    double horizontalSpacing() const;
    void setHorizontalSpacing(double horizontalSpacing);

    double verticalSpacing() const;
    void setVerticalSpacing(double verticalSpacing);
};

}

#endif

// vim: ts=2 sw=2 et
