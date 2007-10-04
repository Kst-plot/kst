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

#ifndef MATRIXSELECTOR_H
#define MATRIXSELECTOR_H

#include <QWidget>
#include "ui_matrixselector.h"

#include <matrix.h>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT MatrixSelector : public QWidget, public Ui::MatrixSelector {
  Q_OBJECT
  public:
    MatrixSelector(QWidget *parent = 0);
    virtual ~MatrixSelector();

    MatrixPtr selectedMatrix() const;
    void setSelectedMatrix(MatrixPtr selectedMatrix);

  Q_SIGNALS:
    void selectionChanged();
};

}

#endif

// vim: ts=2 sw=2 et
