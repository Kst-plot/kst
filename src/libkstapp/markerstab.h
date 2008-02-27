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

#ifndef MARKERSTAB_H
#define MARKERSTAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "ui_markerstab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT MarkersTab : public DialogTab, Ui::MarkersTab {
  Q_OBJECT
  public:
    MarkersTab(QWidget *parent = 0);
    virtual ~MarkersTab();

    void setObjectStore(ObjectStore *store);

    void setObjects(QStringList allObjects);

  public Q_SLOTS:
    void update();
};

}

#endif

// vim: ts=2 sw=2 et
