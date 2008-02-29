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

#ifndef CURVESELECTOR_H
#define CURVESELECTOR_H

#include <QWidget>
#include "ui_curveselector.h"

#include <curve.h>

#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT CurveSelector : public QWidget, public Ui::CurveSelector {
  Q_OBJECT
  public:
    CurveSelector(QWidget *parent = 0, ObjectStore *store = 0);
    virtual ~CurveSelector();

    void setObjectStore(ObjectStore *store);

    CurvePtr selectedCurve() const;
    bool selectedCurveDirty() const;
    void setSelectedCurve(CurvePtr selectedCurve);

    bool allowEmptySelection() const;
    void setAllowEmptySelection(bool allowEmptySelection);

    void clearSelection();

  Q_SIGNALS:
    void selectionChanged(const QString&);

  private Q_SLOTS:
    void emitSelectionChanged();

  private:
    void fillCurves();

  private:
    bool _allowEmptySelection;

    ObjectStore *_store;
};

}

#endif

// vim: ts=2 sw=2 et
