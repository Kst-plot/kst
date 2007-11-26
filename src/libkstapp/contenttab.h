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

#ifndef CONTENTTAB_H
#define CONTENTTAB_H

#include "dialogtab.h"
#include "ui_contenttab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT ContentTab : public DialogTab, Ui::ContentTab {
  Q_OBJECT
  public:
    ContentTab(QWidget *parent = 0);
    virtual ~ContentTab();

    void setDisplayedCurves(QStringList displayedCurves);
    void setAvailableCurves(QStringList availableCurves);

    QStringList displayedCurves();

  private Q_SLOTS:
    void updateButtons();
    void addButtonClicked();
    void removeButtonClicked();
    void upButtonClicked();
    void downButtonClicked();
};

}

#endif

// vim: ts=2 sw=2 et
