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

#include "vectorselector.h"

#include "kstdatacollection.h"

namespace Kst {

VectorSelector::VectorSelector(QWidget *parent)
  : QWidget(parent), _allowEmptySelection(true) {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newVector->setIcon(QPixmap(":kst_vectornew.png"));
  _editVector->setIcon(QPixmap(":kst_vectoredit.png"));

  _newVector->setFixedSize(size + 8, size + 8);
  _editVector->setFixedSize(size + 8, size + 8);

//   fillVectors();
}


VectorSelector::~VectorSelector() {
}


KstVectorPtr VectorSelector::selectedVector() const {
  return 0;
}


void VectorSelector::setSelectedVector(KstVectorPtr selectedVector) {
  Q_UNUSED(selectedVector);
}


// void VectorSelector::fillVectors() {
//   QStringList vectors;
// 
//   KST::vectorList.lock().readLock();
// 
//   KstVectorList::ConstIterator it = KST::vectorList.begin();
//   for (; it != KST::vectorList.end(); ++it) {
//     KstVectorPtr vector = (*it);
//     if (vector->isScalarList())
//       continue;
// 
//     vector->readLock();
//     vectors << vector->tag().displayString();
//     vector->unlock();
//   }
// 
//   KST::vectorList.lock().unlock();
// 
//   qSort(vectors);
// 
//   if (allowEmptySelection()) {
//     vectors.prepend(tr("<None>"));
//   }
// 
//   _vector->clear();
//   _vector->addItems(vectors);
// }

}

// vim: ts=2 sw=2 et
