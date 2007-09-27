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

#include "matrixselector.h"

namespace Kst {

MatrixSelector::MatrixSelector(QWidget *parent)
  : QWidget(parent) {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newMatrix->setIcon(QPixmap(":kst_matrixnew.png"));
  _editMatrix->setIcon(QPixmap(":kst_matrixedit.png"));

  _newMatrix->setFixedSize(size + 8, size + 8);
  _editMatrix->setFixedSize(size + 8, size + 8);
}


MatrixSelector::~MatrixSelector() {
}


KstMatrixPtr MatrixSelector::selectedMatrix() const {
  return 0;
}


void MatrixSelector::setSelectedMatrix(KstMatrixPtr selectedMatrix) {
  Q_UNUSED(selectedMatrix);
}

}

// vim: ts=2 sw=2 et
