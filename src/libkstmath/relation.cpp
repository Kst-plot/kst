/***************************************************************************
                   relation.cpp: base class for a curve
                             -------------------
    begin                : June 2003
    copyright            : (C) 2003 University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/

#include "relation.h"
#include "datacollection.h"
#include "debug.h"
#include "kst_i18n.h"

#include "objectstore.h"

#include <QXmlStreamWriter>

namespace Kst {

const QString Relation::staticTypeString = I18N_NOOP("Relation");

Relation::Relation(ObjectStore *store) : Object() {
  Q_UNUSED(store);
  commonConstructor();
}


void Relation::commonConstructor() {
  _ns_maxx = _ns_minx = _ns_maxy = _ns_miny = MaxX = MinX = MinPosX = MeanX = MaxY = MinY = MinPosY = 0.0;
  NS = 0;

  _redrawRequired = true;
  _ignoreAutoScale = false;

  _contextDetails.Lx = 0.0;
  _contextDetails.Hx = 0.0;
  _contextDetails.Ly = 0.0;
  _contextDetails.Hy = 0.0;
  _contextDetails.m_X = 0.0;
  _contextDetails.m_Y = 0.0;
  _contextDetails.b_X = 0.0;
  _contextDetails.b_Y = 0.0;
  _contextDetails.XMin = 0.0;
  _contextDetails.XMax = 0.0;
  _contextDetails.xLog = false;
  _contextDetails.yLog = false;
  _contextDetails.xLogBase = 0.0;
  _contextDetails.yLogBase = 0.0;
  _contextDetails.penWidth = 0;
}


Relation::~Relation() {
}


void Relation::save(QXmlStreamWriter &s) {
  Q_UNUSED(s)
}


void Relation::deleteDependents() {
  Data::self()->removeCurveFromPlots(this);
}


void Relation::paint(const CurveRenderContext& context) {
  if (redrawRequired(context) || _redrawRequired) {
    updatePaintObjects(context);
    _redrawRequired = false;
  }

  paintObjects(context);
}


bool Relation::redrawRequired(const CurveRenderContext& context) {
  if ((_contextDetails.Lx == context.Lx) &&
      (_contextDetails.Hx == context.Hx) &&  
      (_contextDetails.Ly == context.Ly) &&  
      (_contextDetails.Hy == context.Hy) &&  
      (_contextDetails.m_X == context.m_X) &&  
      (_contextDetails.m_Y == context.m_Y) &&  
      (_contextDetails.b_X == context.b_X) &&  
      (_contextDetails.b_Y == context.b_Y) &&  
      (_contextDetails.XMin == context.XMin) &&  
      (_contextDetails.XMax == context.XMax) &&  
      (_contextDetails.xLog == context.xLog) &&  
      (_contextDetails.yLog == context.yLog) &&  
      (_contextDetails.xLogBase == context.xLogBase) &&  
      (_contextDetails.yLogBase == context.yLogBase) &&  
      (_contextDetails.penWidth == context.penWidth) ) {
    return false;
  } else {
    _contextDetails.Lx = context.Lx;
    _contextDetails.Hx = context.Hx;
    _contextDetails.Ly = context.Ly;
    _contextDetails.Hy = context.Hy;
    _contextDetails.m_X = context.m_X;
    _contextDetails.m_Y = context.m_Y;
    _contextDetails.b_X = context.b_X;
    _contextDetails.b_Y = context.b_Y;
    _contextDetails.XMin = context.XMin;
    _contextDetails.XMax = context.XMax;
    _contextDetails.xLog = context.xLog;
    _contextDetails.yLog = context.yLog;
    _contextDetails.xLogBase = context.xLogBase;
    _contextDetails.yLogBase = context.yLogBase;
    _contextDetails.penWidth = context.penWidth;
    return true;
  }
}


void Relation::setIgnoreAutoScale(bool ignoreAutoScale) {
  _ignoreAutoScale = ignoreAutoScale;
}

qint64 Relation::minInputSerial() const{
  qint64 minSerial = LLONG_MAX;

  foreach (VectorPtr P, _inputVectors) {
    minSerial = qMin(minSerial, P->serial());
  }
  foreach (ScalarPtr P, _inputScalars) {
    minSerial = qMin(minSerial, P->serial());
  }
  foreach (MatrixPtr P, _inputMatrices) {
    minSerial = qMin(minSerial, P->serial());
  }
  foreach (StringPtr P, _inputStrings) {
    minSerial = qMin(minSerial, P->serial());
  }
  return minSerial;
}

qint64 Relation::maxInputSerialOfLastChange() const {
  qint64 maxSerial = NoInputs;

  foreach (VectorPtr P, _inputVectors) {
    maxSerial = qMax(maxSerial, P->serialOfLastChange());
  }
  foreach (ScalarPtr P, _inputScalars) {
    maxSerial = qMax(maxSerial, P->serialOfLastChange());
  }
  foreach (MatrixPtr P, _inputMatrices) {
    maxSerial = qMax(maxSerial, P->serialOfLastChange());
  }
  foreach (StringPtr P, _inputStrings) {
    maxSerial = qMax(maxSerial, P->serialOfLastChange());
  }
  return maxSerial;
}

void Relation::writeLockInputsAndOutputs() const {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  #ifdef LOCKTRACE
  qDebug() << (void*)this << this->Name() << ") Relation::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << endl;
  #endif

  QList<PrimitivePtr> inputs;
  QList<PrimitivePtr> outputs;

  QList<StringPtr> sl = _inputStrings.values();
  for (QList<StringPtr>::Iterator i = sl.begin(); i != sl.end(); ++i) {
    inputs += (*i).data();
  }
  sl = _outputStrings.values();
  for (QList<StringPtr>::Iterator i = sl.begin(); i != sl.end(); ++i) {
    outputs += (*i).data();
  }

  QList<ScalarPtr> sc = _inputScalars.values();
  for (QList<ScalarPtr>::Iterator i = sc.begin(); i != sc.end(); ++i) {
    inputs += (*i).data();
  }
  sc = _outputScalars.values();
  for (QList<ScalarPtr>::Iterator i = sc.begin(); i != sc.end(); ++i) {
    outputs += (*i).data();
  }

  QList<VectorPtr> vl = _inputVectors.values();
  for (QList<VectorPtr>::Iterator i = vl.begin(); i != vl.end(); ++i) {
    inputs += (*i).data();
  }
  vl = _outputVectors.values();
  for (QList<VectorPtr>::Iterator i = vl.begin(); i != vl.end(); ++i) {
    outputs += (*i).data();
  }

  QList<MatrixPtr> ml = _inputMatrices.values();
  for (QList<MatrixPtr>::Iterator i = ml.begin(); i != ml.end(); ++i) {
    inputs += (*i).data();
  }
  ml = _outputMatrices.values();
  for (QList<MatrixPtr>::Iterator i = ml.begin(); i != ml.end(); ++i) {
    outputs += (*i).data();
  }

  qSort(inputs);
  qSort(outputs);

  QList<PrimitivePtr>::ConstIterator inputIt = inputs.begin();
  QList<PrimitivePtr>::ConstIterator outputIt = outputs.begin();

  while (inputIt != inputs.end() || outputIt != outputs.end()) {
    if (inputIt != inputs.end() && (outputIt == outputs.end() || (void*)(*inputIt) < (void*)(*outputIt))) {
      // do input
      if (!(*inputIt)) {
        qWarning() << "Input for data object " << this->Name() << " is invalid." << endl;
      }
#ifdef LOCKTRACE
      qDebug() << (void*)this << this->Name() << ") KstDataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": write locking input \"" << (*inputIt)->Name() << "\" (" << (void*)((KstRWLock*)*inputIt) << ")" << endl;
#endif
      (*inputIt)->writeLock();
      ++inputIt;
    } else {
      // do output
      if (!(*outputIt)) {
        qWarning() << "Output for data object " << this->Name() << " is invalid." << endl;
      }
#ifdef LOCKTRACE
      qDebug() << (void*)this << this->Name() << ") KstDataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": write locking output \"" << (*outputIt)->Name() << "\" (" << (void*)((KstRWLock*)*outputIt) << ")" << endl;
#endif
      if ((*outputIt)->provider() != this) {
      }
      (*outputIt)->writeLock();
      ++outputIt;
    }
  }
}


void Relation::unlockInputsAndOutputs() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << endl;
  #endif

  for (MatrixMap::ConstIterator i = _outputMatrices.begin(); i != _outputMatrices.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output matrix for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output matrix \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (MatrixMap::ConstIterator i = _inputMatrices.begin(); i != _inputMatrices.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input matrix for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input matrix \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (VectorMap::ConstIterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output vector for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output vector \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (VectorMap::ConstIterator i = _inputVectors.begin(); i != _inputVectors.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input vector for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input vector \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (ScalarMap::ConstIterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output scalar for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output scalar \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (ScalarMap::ConstIterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input scalar for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input scalar \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (StringMap::ConstIterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output string for data object " << this->Name() << " is invalid." << endl;
    }
   #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output string \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (StringMap::ConstIterator i = _inputStrings.begin(); i != _inputStrings.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input string for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << this->Name() << ") Relation::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input string \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }
}


bool Relation::uses(ObjectPtr p) const {
  VectorPtr v = kst_cast<Vector>(p);
  if (v) {
    for (VectorMap::ConstIterator j = _inputVectors.begin(); j != _inputVectors.end(); ++j) {
      if (j.value() == v) {
        return true;
      }
    }
    QHashIterator<QString, ScalarPtr> scalarDictIter(v->scalars());
    for (ScalarMap::ConstIterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == j.value()) {
          return true;
        }
      }
    }
    QHashIterator<QString, StringPtr> stringDictIter(v->strings());
    for (StringMap::ConstIterator j = _inputStrings.begin(); j != _inputStrings.end(); ++j) {
      while (stringDictIter.hasNext()) {
        stringDictIter.next();
        if (stringDictIter.value() == j.value()) {
          return true;
        }
      }
    }
  } else if (MatrixPtr matrix = kst_cast<Matrix>(p)) {
    for (MatrixMap::ConstIterator j = _inputMatrices.begin(); j != _inputMatrices.end(); ++j) {
      if (j.value() == matrix) {
        return true;
      }
    }
    QHashIterator<QString, ScalarPtr> scalarDictIter(matrix->scalars());
    for (ScalarMap::ConstIterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == j.value()) {
          return true;
        }
      }
    }
  } else if (DataObjectPtr obj = kst_cast<DataObject>(p) ) {
    // check all connections from this object to p
    for (VectorMap::Iterator j = obj->outputVectors().begin(); j != obj->outputVectors().end(); ++j) {
      for (VectorMap::ConstIterator k = _inputVectors.begin(); k != _inputVectors.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
      // also check dependencies on vector stats
      QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
      for (ScalarMap::ConstIterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
        while (scalarDictIter.hasNext()) {
          scalarDictIter.next();
          if (scalarDictIter.value() == k.value()) {
            return true;
          }
        }
      }
      // also check dependencies on vector strings
      QHashIterator<QString, StringPtr> stringDictIter(j.value()->strings());
      for (StringMap::ConstIterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
        while (stringDictIter.hasNext()) {
          stringDictIter.next();
          if (stringDictIter.value() == k.value()) {
            return true;
          }
        }
      }
    }

    for (MatrixMap::Iterator j = obj->outputMatrices().begin(); j != obj->outputMatrices().end(); ++j) {
      for (MatrixMap::ConstIterator k = _inputMatrices.begin(); k != _inputMatrices.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
      // also check dependencies on vector stats
      QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
      for (ScalarMap::ConstIterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
        while (scalarDictIter.hasNext()) {
          scalarDictIter.next();
          if (scalarDictIter.value() == k.value()) {
            return true;
          }
        }
      }
    }

    for (ScalarMap::Iterator j = obj->outputScalars().begin(); j != obj->outputScalars().end(); ++j) {
      for (ScalarMap::ConstIterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
    }

    for (StringMap::Iterator j = obj->outputStrings().begin(); j != obj->outputStrings().end(); ++j) {
      for (StringMap::ConstIterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
    }
  }
  return false;
}


PrimitiveList Relation::inputPrimitives() const {
  PrimitiveList primitive_list;

  int n = _inputMatrices.count();
  for (int i = 0; i< n; i++) {
      primitive_list.append(kst_cast<Primitive>(_inputMatrices.values().at(i)));
  }

  n = _inputStrings.count();
  for (int i = 0; i< n; i++) {
      primitive_list.append(kst_cast<Primitive>(_inputStrings.values().at(i)));
  }

  n = _inputScalars.count();
  for (int i = 0; i< n; i++) {
      primitive_list.append(kst_cast<Primitive>(_inputScalars.values().at(i)));
  }

  n = _inputVectors.count();
  for (int i = 0; i< n; i++) {
      primitive_list.append(kst_cast<Primitive>(_inputVectors.values().at(i)));
  }

  return primitive_list;
}

void Relation::replaceInput(PrimitivePtr p, PrimitivePtr new_p) {
  if (VectorPtr v = kst_cast<Vector>(p) ) {
    if (VectorPtr new_v = kst_cast<Vector>(new_p)) {
      for (VectorMap::Iterator j = _inputVectors.begin(); j != _inputVectors.end(); ++j) {
        if (j.value() == v) {
          _inputVectors[j.key()] = new_v;
        }
      }
    }
  } else if (MatrixPtr m = kst_cast<Matrix>(p) ) {
    if (MatrixPtr new_m = kst_cast<Matrix>(new_p)) {
      for (MatrixMap::Iterator j = _inputMatrices.begin(); j != _inputMatrices.end(); ++j) {
        if (j.value() == m) {
          _inputMatrices[j.key()] = new_m;
        }
      }
    }
  } else if (StringPtr s = kst_cast<String>(p) ) {
    if (StringPtr new_s = kst_cast<String>(new_p)) {
      for (StringMap::Iterator j = _inputStrings.begin(); j != _inputStrings.end(); ++j) {
        if (j.value() == s) {
          _inputStrings[j.key()] = new_s;
        }
      }
    }
  } else if (ScalarPtr s = kst_cast<Scalar>(p) ) {
    if (ScalarPtr new_s = kst_cast<Scalar>(new_p)) {
      for (ScalarMap::Iterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
        if (j.value() == s) {
          _inputScalars[j.key()] = new_s;
        }
      }
    }
  }
}


}
// vim: ts=2 sw=2 et
