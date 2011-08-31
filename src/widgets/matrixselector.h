/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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

#include "kstwidgets_export.h"

namespace Kst {

class ObjectStore;

class KSTWIDGETS_EXPORT MatrixSelector : public QWidget, public Ui::MatrixSelector {
  Q_OBJECT
  public:
    explicit MatrixSelector(QWidget *parent = 0, ObjectStore *store = 0);
    virtual ~MatrixSelector();

    void setObjectStore(ObjectStore *store);

    MatrixPtr selectedMatrix() const;
    bool selectedMatrixDirty() const;
    void setSelectedMatrix(MatrixPtr selectedMatrix);

    void clearSelection();

    virtual bool event(QEvent * event);

  Q_SIGNALS:
    void selectionChanged();

  public Q_SLOTS:
    void updateMatrices();
    void matrixSelected(int index);
    void matrixSelected(QString text);

  private Q_SLOTS:
    void newMatrix();
    void editMatrix();

  private:
    ObjectStore *_store;

    void fillMatrices();
};

}

#endif

// vim: ts=2 sw=2 et
