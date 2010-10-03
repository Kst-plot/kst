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

#ifndef VECTORSELECTOR_H
#define VECTORSELECTOR_H

#include <QWidget>
#include "ui_vectorselector.h"

#include <vector.h>

#include "kstwidgets_export.h"

namespace Kst {

class ObjectStore;

class KSTWIDGETS_EXPORT VectorSelector : public QWidget, public Ui::VectorSelector {
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

    void fillVectors();
    void setToLastX();

    void setIsX(bool is_x) {_isX = is_x;}
    int count() {return _vector->count();}

    virtual bool event(QEvent * event);

  Q_SIGNALS:
    void selectionChanged(const QString&);
    void contentChanged(); // something in the combo changed (new or edit)

  private Q_SLOTS:
    void newVector();
    void editVector();
    void emitSelectionChanged();
    void updateDescriptionTip();

  private:
    bool _allowEmptySelection;
    bool _isX;

    ObjectStore *_store;
};

}

#endif

// vim: ts=2 sw=2 et
