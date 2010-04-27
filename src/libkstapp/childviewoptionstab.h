/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHILDVIEWOPTIONSTAB_H
#define CHILDVIEWOPTIONSTAB_H

#include "dialogtab.h"
#include "ui_childviewoptionstab.h"

#include "kst_export.h"

namespace Kst {

class ChildViewOptionsTab : public DialogTab, Ui_ChildViewOptionsTab {
  Q_OBJECT
  public:
    ChildViewOptionsTab(QWidget *parent = 0);
    virtual ~ChildViewOptionsTab();

    bool shareAxis() const;
    void setShareAxis(bool shareAxis);
};

}

#endif

// vim: ts=2 sw=2 et
