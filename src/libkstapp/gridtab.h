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

#ifndef GRIDTAB_H
#define GRIDTAB_H

#include "dialogtab.h"
#include "ui_gridtab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT GridTab : public DialogTab, Ui::GridTab {
  Q_OBJECT
  public:
    GridTab(QWidget *parent = 0);
    virtual ~GridTab();

    bool showGrid() const;
    void setShowGrid(bool showGrid);

    bool snapToGrid() const;
    void setSnapToGrid(bool snapToGrid);

    qreal gridHorizontalSpacing() const;
    void setGridHorizontalSpacing(qreal spacing);

    qreal gridVerticalSpacing() const;
    void setGridVerticalSpacing(qreal spacing);
};

}

#endif

// vim: ts=2 sw=2 et
