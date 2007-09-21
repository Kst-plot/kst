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

#ifndef GENERALTAB_H
#define GENERALTAB_H

#include "dialogtab.h"
#include "ui_generaltab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT GeneralTab : public DialogTab, Ui::GeneralTab {
  Q_OBJECT
  public:
    GeneralTab(QWidget *parent = 0);
    virtual ~GeneralTab();
};

}

#endif

// vim: ts=2 sw=2 et
