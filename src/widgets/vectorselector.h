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

#ifndef VECTORSELECTOR_H
#define VECTORSELECTOR_H

#include <QWidget>
#include "ui_vectorselector.h"

#include <vector.h>

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT VectorSelector : public QWidget, public Ui::VectorSelector {
  Q_OBJECT
  public:
    VectorSelector(QWidget *parent = 0, ObjectStore *store = 0);
    virtual ~VectorSelector();

    void setObjectStore(ObjectStore *store);

    VectorPtr selectedVector() const;
    bool selectedVectorDirty() const;
    void setSelectedVector(VectorPtr selectedVector);

    bool allowEmptySelection() const;
    void setAllowEmptySelection(bool allowEmptySelection);

    void clearSelection();

  Q_SIGNALS:
    void selectionChanged(const QString&);

  private Q_SLOTS:
    void newVector();
    void editVector();
    void emitSelectionChanged();
    void updateDescriptionTip();

  private:
    void fillVectors();

  private:
    bool _allowEmptySelection;

    ObjectStore *_store;
};

}

#endif

// vim: ts=2 sw=2 et
