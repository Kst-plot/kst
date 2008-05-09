/***************************************************************************
                          vector.cpp  -  description
                             -------------------
    begin                : Fri Sep 22 2000
    copyright            : (C) 2000-2002 by cbn
    email                :
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

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <QDebug>
#include <QXmlStreamWriter>

#include "kst_i18n.h"

#include "datacollection.h"
#include "math_kst.h"
#include "vector.h"
//#include "scalar.h"
#include "debug.h"
#include "objectstore.h"
#include "updatemanager.h"

namespace Kst {

// "Zero" means set to NAN.
// Use 1 for a simple for() loop
// Use 2 for memset (presently broken)
#define ZERO_MEMORY 1

#define INITSIZE 1

const QString Vector::staticTypeString = I18N_NOOP("Vector");
const QString Vector::staticTypeTag = I18N_NOOP("vector");

/** Create a vector */
Vector::Vector(ObjectStore *store, const ObjectTag& tag, int size, Object *provider, bool isScalarList)
    : Primitive(store, tag, provider), _nsum(0) {
  //qDebug() << "+++ CREATING VECTOR: " << (void*) this;

  _editable = false;
  NumShifted = 0;
  NumNew = 0;
  _saveData = false;
  _isScalarList = isScalarList;

  _saveable = false;

  if (size <= 0) {
    size = INITSIZE;
  }

  _v = static_cast<double*>(malloc(size * sizeof(double)));
  if (!_v) { // Malloc failed
    _v = static_cast<double*>(malloc(sizeof(double)));
    _size = 1;
  } else {
    _size = size;
  }
  _is_rising = false;

  CreateScalars(store);
  blank();

  _shortName = "V"+QString::number(_vnum);
  _vnum++;
}


Vector::Vector(ObjectStore *store, const ObjectTag& tag, const QByteArray& data)
    : Primitive(store, tag), _nsum(0) {
  _v = 0L;
  _size = 0;
  int sz = INITSIZE;

  _editable = false;
  NumShifted = 0;
  NumNew = 0;
  _isScalarList = false;
  _saveable = false;
  _saveData = false;

  sz = qMax((size_t)(INITSIZE), data.size()/sizeof(double));

  CreateScalars(store);
  resize(sz, true);

  if (!data.isEmpty()) {
    _saveable = true;
    _saveData = true;
    QDataStream qds(data);
    for (int i = 0; !qds.atEnd(); ++i) {
      qds >> _v[i];
    }
  }

  _is_rising = false;
  _shortName = "V"+QString::number(_vnum);
  _vnum++;
}


Vector::~Vector() {
  // qDebug() << "+++ DELETING VECTOR: " << (void*) this;
  if (_v) {
    free(_v);
    _v = 0;
  }
}


void Vector::deleteDependents() {
  for (QHash<QString, Scalar*>::Iterator it = _scalars.begin(); it != _scalars.end(); ++it) {
    _store->removeObject(it.value());
  }
}


const QString& Vector::typeString() const {
  return staticTypeString;
}


#define GENERATE_INTERPOLATION              \
  assert(_size > 0);                        \
  /** Limits checks - optional? **/         \
  if (in_i < 0 || _size == 1) {             \
    return _v[0];                           \
  }                                         \
                                            \
  if (in_i >= ns_i - 1) {                   \
    return _v[_size - 1];                   \
  }                                         \
                                            \
  /** speedup check **/                     \
  if (ns_i == _size) { /* no extrapolating or decimating needed */  \
    return _v[in_i];                        \
  }                                         \
                                            \
  double fj = in_i * double(_size - 1) / double(ns_i-1); /* scaled index */ \
                                            \
  int j = int(floor(fj)); /* index of sample one lower */ \
  assert(j+1 < _size && j >= 0);            \
  if (_v[j + 1] != _v[j + 1] || _v[j] != _v[j]) { \
    return NOPOINT;                    \
  }                                         \
                                            \
  double fdj = fj - float(j); /* fdj is fraction between _v[j] and _v[j+1] */ \
                                            \
  return _v[j + 1] * fdj + _v[j] * (1.0 - fdj);


// FIXME: optimize me - possible that floor() (especially) and isnan() are
//        expensive here.
/** Return v[i], i is sample number, interpolated to have ns_i total
    samples in vector */
double Vector::interpolate(int in_i, int ns_i) const {
  GENERATE_INTERPOLATION
}


double kstInterpolate(double *_v, int _size, int in_i, int ns_i) {
  GENERATE_INTERPOLATION
}

#undef GENERATE_INTERPOLATION

#define RETURN_FIRST_NON_HOLE               \
    for (int i = 0; i < _size; ++i) {       \
      if (_v[i] == _v[i]) {                 \
        return _v[i];                       \
      }                                     \
    }                                       \
    return 0.;

#define RETURN_LAST_NON_HOLE                \
    for (int i = _size - 1; i >= 0; --i) {  \
      if (_v[i] == _v[i]) {                 \
        return _v[i];                       \
      }                                     \
    }                                       \
    return 0.;

#define FIND_LEFT(val, idx)                 \
    for (; idx >= 0; --idx) {               \
      if (_v[idx] == _v[idx]) {             \
        val = _v[idx]; break;               \
      }                                     \
    }

#define FIND_RIGHT(val, idx)                \
    for (; idx < _size; ++idx) {            \
      if (_v[idx] == _v[idx]) {             \
        val = _v[idx]; break;               \
      }                                     \
    }


#define GENERATE_INTERPOLATION              \
  assert(_size > 0);                        \
  /** Limits checks - optional? **/         \
  if (in_i <= 0 || _size == 1) {            \
    RETURN_FIRST_NON_HOLE                   \
  }                                         \
                                            \
  if (in_i >= ns_i - 1) {                   \
    RETURN_LAST_NON_HOLE                    \
  }                                         \
                                            \
  /** speedup check **/                     \
  if (ns_i == _size) {                      \
    if (_v[in_i] == _v[in_i]) {             \
      return _v[in_i];                      \
    }                                       \
    double left = 0., right = 0.;           \
    int leftIndex = in_i, rightIndex = in_i;\
    FIND_LEFT(left, leftIndex)              \
    FIND_RIGHT(right, rightIndex)           \
    if (leftIndex == -1) {                  \
      return right;                         \
    }                                       \
    if (rightIndex == _size) {              \
      return left;                          \
    }                                       \
    return left + (right - left) * double(in_i - leftIndex) / double(rightIndex - leftIndex); \
  }                                         \
  abort(); /* FIXME */                      \
  double indexScaleFactor = double(_size - 1) / double(ns_i - 1); \
  double fj = in_i * indexScaleFactor; /* scaled index */ \
                                            \
  int j = int(floor(fj)); /* index of sample one lower */ \
  assert(j+1 < _size && j >= 0);            \
  if (_v[j + 1] != _v[j + 1] || _v[j] != _v[j]) { \
    return NOPOINT;                    \
  }                                         \
                                            \
  double fdj = fj - float(j); /* fdj is fraction between _v[j] and _v[j+1] */ \
                                            \
  return _v[j + 1] * fdj + _v[j] * (1.0 - fdj);


// FIXME: optimize me - possible that floor() (especially) and isnan() are
//        expensive here.
double Vector::interpolateNoHoles(int in_i, int ns_i) const {
  GENERATE_INTERPOLATION
}


double kstInterpolateNoHoles(double *_v, int _size, int in_i, int ns_i) {
  GENERATE_INTERPOLATION
}

#undef FIND_LEFT
#undef FIND_RIGHT
#undef RETURN_LAST_NON_HOLE
#undef RETURN_FIRST_NON_HOLE
#undef GENERATE_INTERPOLATION

double Vector::value(int i) const {
  if (i < 0 || i >= _size) { // can't look before beginning or past end
    return 0.0;
  }
  return _v[i];
}

void Vector::CreateScalars(ObjectStore *store) {
  if (!_isScalarList) {
    _min = _max = _mean = _minPos = 0.0;

    Q_ASSERT(store);
    ScalarPtr sp;
    _scalars.insert("max", sp = store->createObject<Scalar>(ObjectTag("Max", tag())));
    sp->setProvider(this);
    sp->setSlaveName("Max");
    sp->_KShared_ref();
    _scalars.insert("min", sp = store->createObject<Scalar>(ObjectTag("Min", tag())));
    sp->setProvider(this);
    sp->setSlaveName("Min");
    sp->_KShared_ref();
    _scalars.insert("last", sp = store->createObject<Scalar>(ObjectTag("Last", tag())));
    sp->setProvider(this);
    sp->setSlaveName("Last");
    sp->_KShared_ref();
    _scalars.insert("first", sp = store->createObject<Scalar>(ObjectTag("First", tag())));
    sp->setProvider(this);
    sp->setSlaveName("First");
    sp->_KShared_ref();
    _scalars.insert("mean", sp = store->createObject<Scalar>(ObjectTag("Mean", tag())));
    sp->setProvider(this);
    sp->setSlaveName("Mean");
    sp->_KShared_ref();
    _scalars.insert("sigma", sp = store->createObject<Scalar>(ObjectTag("Sigma", tag())));
    sp->setProvider(this);
    sp->setSlaveName("Sigma");
    sp->_KShared_ref();
    _scalars.insert("rms", sp = store->createObject<Scalar>(ObjectTag("Rms", tag())));
    sp->setProvider(this);
    sp->setSlaveName("Rms");
    sp->_KShared_ref();
    _scalars.insert("ns", sp = store->createObject<Scalar>(ObjectTag("NS", tag())));
    sp->setProvider(this);
    sp->setSlaveName("NS");
    sp->_KShared_ref();
    _scalars.insert("sum", sp = store->createObject<Scalar>(ObjectTag("Sum", tag())));
    sp->setProvider(this);
    sp->setSlaveName("Sum");
    sp->_KShared_ref();
    _scalars.insert("sumsquared", sp = store->createObject<Scalar>(ObjectTag("SumSquared", tag())));
    sp->setProvider(this);
    sp->setSlaveName("SumSquared");
    sp->_KShared_ref();
    _scalars.insert("minpos", sp = store->createObject<Scalar>(ObjectTag("MinPos", tag())));
    sp->setProvider(this);
    sp->setSlaveName("MinPos");
    sp->_KShared_ref();
  }
}


void Vector::updateScalars() {
  if (!_isScalarList) {
    _scalars["ns"]->setValue(_size);

    if (_nsum >= 2) {
      double sum = _scalars["sum"]->value();
      double sumsq = _scalars["sumsquared"]->value();
      _scalars["mean"]->setValue(_mean = sum/double(_nsum));
      _scalars["sigma"]->setValue(sqrt((sumsq - sum * sum / double(_nsum)) / double(_nsum-1)));
      _scalars["rms"]->setValue(sqrt(sumsq/double(_nsum)));
    } else {
      _scalars["sigma"]->setValue(_max - _min);
      _scalars["rms"]->setValue(sqrt(_scalars["sumsquared"]->value()));
      _scalars["mean"]->setValue(_mean = NOPOINT);
    }
  }
}


const QHash<QString, Scalar*>& Vector::scalars() const {
  return _scalars;
}


double* Vector::realloced(double *memptr, int newSize) {
  double *old = _v;
  _v = memptr;
  if (newSize < _size) {
    NumNew = newSize; // all new if we shrunk the vector
  } else {
    NumNew = newSize - _size;
  }
  _size = newSize;
  updateScalars();
  return old;
}


void Vector::zero() {
  setDirty();
  _ns_min = _ns_max = 0.0;
  memset(_v, 0, sizeof(double)*_size);
  updateScalars();
}


void Vector::blank() {
  setDirty();
  _ns_min = _ns_max = 0.0;
#if 0
  // FIXME: make this set NANs across, portably
  memset(_v, 0, sizeof(double)*_size);
#else
  for (int i = 0; i < _size; ++i) {
    _v[i] = NOPOINT;
  }
#endif
  updateScalars();
}


bool Vector::resize(int sz, bool init) {
  //qDebug() << "resizing to: " << sz;
  if (sz > 0) {
    _v = static_cast<double*>(realloc(_v, sz*sizeof(double)));
    if (!_v) {
      return false;
    }
#ifdef ZERO_MEMORY
    if (init && _size < sz) {
#if ZERO_MEMORY == 2
      abort(); // must use NAN here
      memset(&_v[_size], 0, (sz - _size)*sizeof(double));
#else
      for (int i = _size; i < sz; i++) {
        _v[i] = NOPOINT;
      }
#endif
    }
#else
    abort();  // avoid unpleasant surprises
#endif
    _size = sz;
    updateScalars();
  }

  setDirty();
  return true;
}


Object::UpdateType Vector::internalUpdate(Object::UpdateType providerRC) {
  int i, i0;
  double sum, sum2, last, first, v;
  double last_v;
  double dv2 = 0.0, dv, no_spike_max_dv;

  _max = _min = sum = sum2 = _minPos = last = first = NOPOINT;
  _nsum = 0;

  if (_size > 0) {
    _is_rising = true;

    // FIXME: expensive!!  Can we somehow cache this at least?
    for (i = 0; i < _size && !finite(_v[i]); i++) {
      // do nothing
    }

    if (i == _size) {
      if (!_isScalarList) {
        _scalars["sum"]->setValue(sum);
        _scalars["sumsquared"]->setValue(sum2);
        _scalars["max"]->setValue(_max);
        _scalars["min"]->setValue(_min);
        _scalars["minpos"]->setValue(_minPos);
        _scalars["last"]->setValue(last);
        _scalars["first"]->setValue(first);
      }
      _ns_max = _ns_min = 0;

      updateScalars();

      return providerRC;
    }

    i0 = i;

    if (i0 > 0) {
      _is_rising = false;
    }

    _max = _min = _v[i0];
    sum = sum2 = 0.0;

    if (_v[i0] > 0.0) {
      _minPos = _v[i0];
    } else {
      _minPos = 1.0E300;
    }

    last_v = _v[i0];

    for (i = i0; i < _size; i++) {
      v = _v[i]; // get rid of redirections

      if (finite(v)) {
        dv = v - last_v;
        dv2 += dv*dv;

        if (v <= last_v) {
          if (i != i0) {
            _is_rising = false;
          }
        }

        last_v = v;

        _nsum++;
        sum += v;
        sum2 += v*v;

        if (v > _max) {
          _max = v;
        } else if (v < _min) {
          _min = v;
        }
        if (v < _minPos && v > 0.0) {
          _minPos = v;
        }
      } else {
        _is_rising = false;
      }
    }

    no_spike_max_dv = 7.0*sqrt(dv2/double(_nsum));

    _ns_max = _ns_min = last_v = _v[i0];

    last = _v[_size-1];
    first = _v[0];

    for (i = i0; i < _size; i++) {
      v = _v[i]; // get rid of redirections
      if (finite(v)) {
        if (fabs(v - last_v) < no_spike_max_dv) {
          if (v > _ns_max) {
            _ns_max = v;
          } else if (v < _ns_min) {
            _ns_min = v;
          }
          last_v = v;
        } else {
          i += 20;
          if (i < _size) {
            last_v = _v[i];
          }
          i++;
        }
      }
    }

    if (_isScalarList) {
      _max = _min = _minPos = 0.0;
    } else {
      _scalars["sum"]->setValue(sum);
      _scalars["sumsquared"]->setValue(sum2);
      _scalars["max"]->setValue(_max);
      _scalars["min"]->setValue(_min);
      _scalars["minpos"]->setValue(_minPos);
      _scalars["last"]->setValue(last);
      _scalars["first"]->setValue(first);
    }

    updateScalars();

    return providerRC;
  }

  return NO_CHANGE;
}


void Vector::triggerUpdateSignal(ObjectPtr object) {
#if DEBUG_UPDATE_CYCLE > 1
  qDebug() << "UP - Vector" << shortName() << "has been updated as part of update of" << object->shortName() << "informing dependents";
#endif
  emit vectorUpdated(object);
}


void Vector::save(QXmlStreamWriter &s) {
  if (provider()) {
    return;
  }
  s.writeStartElement("vector");
  s.writeAttribute("tag", tag().tagString());
  if (_saveData) {
    QByteArray qba(length()*sizeof(double), '\0');
    QDataStream qds(&qba, QIODevice::WriteOnly);

    for (int i = 0; i < length(); i++) {
      qds << _v[i];
    }

    s.writeTextElement("data", qCompress(qba).toBase64());
  }
  saveNameInfo(s);
  s.writeEndElement();
}

void Vector::setNewAndShift(int inNew, int inShift) {
  NumNew = inNew;
  NumShifted = inShift;
}


QString Vector::label() const {
  return _label; // default
}


QString Vector::fileLabel() const {
  return QString::null;
}


double *const Vector::value() const {
  return _v;
}


void Vector::newSync() {
  NumNew = NumShifted = 0;
}


#if 0
VectorPtr Vector::generateVector(ObjectStore *store, double x0, double x1, int n, const ObjectTag& tag) {
  if (n < 2) {
    n = 2;
  }

  if (x0 > x1) {
    double tx;
    tx = x0;
    x0 = x1;
    x1 = tx;
  }

  if (x0 == x1) {
    x1 = x0 + 0.1;
  }

  QString t = tag.name();
  if (t.isEmpty()) {
    t = suggestVectorName("X(" + QString::number(x0) + ".." + QString::number(x1) + ")");
  }

  Q_ASSERT(store);
  VectorPtr xv = kst_cast<Vector>(store->createObject<Vector>(ObjectTag(t, tag.context())));
  xv->resize(n);
  xv->_saveable = false;

  for (int i = 0; i < n; i++) {
    xv->value()[i] = x0 + double(i) * (x1 - x0) / (n - 1);
  }

  xv->_scalars["min"]->setValue(x0);
  xv->_scalars["max"]->setValue(x1);
  xv->updateScalars();

  return xv;
}
#endif


void Vector::setLabel(const QString& label_in) {
  _label = label_in;
}


int Vector::getUsage() const {
  int adj = 0;
  for (QHash<QString, Scalar*>::ConstIterator it = _scalars.begin(); it != _scalars.end(); ++it) {
    adj += it.value()->getUsage() - 1;
  }
  return Object::getUsage() + adj;
}


bool Vector::saveable() const {
  return _saveable;
}


bool Vector::editable() const {
  return _editable;
}


void Vector::setEditable(bool editable) {
  _editable = editable;
}


bool Vector::saveData() const {
  return _saveData;
}


void Vector::setSaveData(bool save) {
  _saveData = save;
}


void Vector::change(QByteArray &data) {
  if (!data.isEmpty()) {
    _saveable = true;
    _saveData = true;

    int sz = qMax((size_t)(INITSIZE), data.size()/sizeof(double));
    resize(sz, true);

    QDataStream qds(data);
    for (int i = 0; !qds.atEnd(); ++i) {
      qds >> _v[i];
    }
  }
}

#undef ZERO_MEMORY
#undef INITSIZE

}
// vim: et sw=2 ts=2
