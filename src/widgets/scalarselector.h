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

#ifndef SCALARSELECTOR_H
#define SCALARSELECTOR_H

#include <QWidget>
#include "ui_scalarselector.h"

#include <scalar.h>

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT ScalarSelector : public QWidget, public Ui::ScalarSelector {
  Q_OBJECT
  public:
    ScalarSelector(QWidget *parent = 0, ObjectStore *store = 0);
    virtual ~ScalarSelector();

    void setObjectStore(ObjectStore *store);

    ScalarPtr selectedScalar() const;
    void setSelectedScalar(ScalarPtr selectedScalar);

  Q_SIGNALS:
    void selectionChanged();

  private Q_SLOTS:
    void newScalar();
    void editScalar();

  private:
    void fillScalars();

    ObjectStore *_store;
};

}

#endif

// vim: ts=2 sw=2 et
