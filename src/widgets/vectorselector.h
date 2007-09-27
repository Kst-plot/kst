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

#include <kstvector.h>

#include "kst_export.h"

namespace Kst {

class KST_EXPORT VectorSelector : public QWidget, public Ui::VectorSelector {
  Q_OBJECT
  public:
    VectorSelector(QWidget *parent = 0);
    virtual ~VectorSelector();

    KstVectorPtr selectedVector() const;
    void setSelectedVector(KstVectorPtr selectedVector);

    bool allowEmptySelection() const { return _allowEmptySelection; }
    void setAllowEmptySelection(bool allowEmptySelection)
    { _allowEmptySelection = allowEmptySelection; }

  Q_SIGNALS:
    void selectionChanged();

  private:
//     void fillVectors();

  private:
    bool _allowEmptySelection;
};

}

#endif

// vim: ts=2 sw=2 et
