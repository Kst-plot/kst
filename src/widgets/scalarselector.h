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

#include <kstscalar.h>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT ScalarSelector : public QWidget, public Ui::ScalarSelector {
  Q_OBJECT
  public:
    ScalarSelector(QWidget *parent = 0);
    virtual ~ScalarSelector();

    KstScalarPtr selectedScalar() const;
    void setSelectedScalar(KstScalarPtr selectedScalar);

  Q_SIGNALS:
    void selectionChanged();
};

}

#endif

// vim: ts=2 sw=2 et
