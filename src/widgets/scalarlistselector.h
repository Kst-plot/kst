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

#ifndef SCALARLISTSELECTOR_H
#define SCALARLISTSELECTOR_H

#include <QWidget>
#include "ui_scalarlistselector.h"

#include "kstwidgets_export.h"

namespace Kst {

class KSTWIDGETS_EXPORT ScalarListSelector : public QDialog, public Ui::ScalarListSelector {
  Q_OBJECT
  public:
    ScalarListSelector(QWidget *parent = 0);
    virtual ~ScalarListSelector();

    QString selectedScalar() const;

    void fillScalars(QStringList& scalars);
    void clear();

  public Q_SLOTS:
    void filter(const QString& filter);
};

}

#endif

// vim: ts=2 sw=2 et
