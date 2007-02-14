/***************************************************************************
                          bind_matrixcollection.cpp
                             -------------------
    begin                : Jul 27 2005
    copyright            : (C) 2005 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bind_matrixcollection.h"
#include "bind_matrix.h"

#include <kst.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindMatrixCollection::KstBindMatrixCollection(KJS::ExecState *exec, const KstMatrixList& matrices)
: KstBindCollection(exec, "MatrixCollection", true) {
  _isGlobal = false;
  _matrices = matrices.tagNames();
}


KstBindMatrixCollection::KstBindMatrixCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "MatrixCollection", true) {
  _isGlobal = true;
}


KstBindMatrixCollection::~KstBindMatrixCollection() {
}


KJS::Value KstBindMatrixCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::matrixList.lock());
    return KJS::Number(KST::matrixList.count());
  }

  return KJS::Number(_matrices.count());
}


QStringList KstBindMatrixCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::matrixList.lock());
    return KST::matrixList.tagNames();
  }

  return _matrices;
}


KJS::Value KstBindMatrixCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstReadLocker rl(&KST::matrixList.lock());
  KstMatrixPtr vp = *KST::matrixList.findTag(item.qstring());
  if (!vp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindMatrix(exec, vp));
}


KJS::Value KstBindMatrixCollection::extract(KJS::ExecState *exec, unsigned item) const {
  if (_isGlobal) {
    KstReadLocker rl(&KST::matrixList.lock());
    if (item >= KST::matrixList.count()) {
      return KJS::Undefined();
    }

    KstMatrixPtr vp = KST::matrixList[item];
    if (!vp) {
      return KJS::Undefined();
    }

    return KJS::Object(new KstBindMatrix(exec, vp));
  }

  if (item >= _matrices.count()) {
    return KJS::Undefined();
  }

  KstReadLocker rl(&KST::matrixList.lock());
  KstMatrixPtr vp = *KST::matrixList.findTag(_matrices[item]);
  if (!vp) {
    return KJS::Undefined();
  }
  return KJS::Object(new KstBindMatrix(exec, vp));
}


// vim: ts=2 sw=2 et
