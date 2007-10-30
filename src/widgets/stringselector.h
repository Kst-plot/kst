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

#ifndef STRINGSELECTOR_H
#define STRINGSELECTOR_H

#include <QWidget>
#include "ui_stringselector.h"

#include <string_kst.h>

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT StringSelector : public QWidget, public Ui::StringSelector {
  Q_OBJECT
  public:
    StringSelector(QWidget *parent = 0, ObjectStore *store = 0);
    virtual ~StringSelector();

    void setObjectStore(ObjectStore *store);

    StringPtr selectedString() const;
    void setSelectedString(StringPtr selectedString);

  Q_SIGNALS:
    void selectionChanged();

  private:
    ObjectStore *_store;
};

}

#endif

// vim: ts=2 sw=2 et
