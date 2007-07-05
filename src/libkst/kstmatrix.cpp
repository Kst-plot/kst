/***************************************************************************
                   kstmatrix.cpp: 2D matrix type for kst
                             -------------------
    begin                : July 2004
    copyright            : (C) 2004 University of British Columbia
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

#include <stdlib.h>
#include <math.h>

#include "kst_i18n.h"

#include "defaultprimitivenames.h"
#include "kstdatacollection.h"
#include "kstdebug.h"
#include <qdebug.h>
#include "kstmatrix.h"
#include "kstmath.h"

// used for resizing; set to 1 for loop zeroing, 2 to use memset
#define ZERO_MEMORY 2

static int anonymousMatrixCounter = 1;

KstMatrix::KstMatrix(KstObjectTag in_tag, KstObject *provider, uint nX, uint nY, double minX, double minY, double stepX, double stepY)
: KstPrimitive(provider) {
 
  _nX = nX;
  _nY = nY;
  _NS = _nX * _nY;
  _NRealS = 0;    
  _minX = minX;
  _minY = minY;
  _stepX = stepX;
  _stepY = stepY;
  _z = 0L;
  _zSize = 0;

  _editable = false;
  _saveable = false;

  QString _tag = in_tag.tag();
  if (!in_tag.isValid()) {
    do {
      _tag = i18n("Anonymous Matrix %1", anonymousMatrixCounter++);
    } while (KstData::self()->matrixTagNameNotUnique(_tag, false));
    KstObject::setTagName(KstObjectTag(_tag, in_tag.context()));
  } else {
    KstObject::setTagName(KST::suggestUniqueMatrixTag(in_tag));
  }

  createScalars();
  setDirty();

  KST::matrixList.lock().writeLock();
  KST::matrixList.append(this);
  KST::matrixList.lock().unlock();
}


KstMatrix::~KstMatrix() {
  // get rid of the stat scalars
  KST::scalarList.lock().writeLock();
  KST::scalarList.setUpdateDisplayTags(false);
  for (QHash<QString, KstScalar*>::Iterator iter = _statScalars.begin(); iter != _statScalars.end(); ++iter) {
    KST::scalarList.remove(iter.value());
    iter.value()->_KShared_unref();  
  }
  KST::scalarList.setUpdateDisplayTags(true);
  KST::scalarList.lock().unlock();

  if (_z) {
    free(_z);
    _z = 0L;  
  }  
}


int KstMatrix::sampleCount() const {
  return _nX*_nY;  
}
    

double KstMatrix::value(double x, double y, bool* ok) const {
  int x_index = (int)floor((x - _minX) / (double)_stepX);
  int y_index = (int)floor((y - _minY) / (double)_stepY);

  return valueRaw(x_index, y_index, ok);
}
    

double KstMatrix::valueRaw(int x, int y, bool* ok) const {
  int index = zIndex(x,y);
  if ((index < 0) || !finite(_z[index]) || KST_ISNAN(_z[index])) {
    if (ok) {
      (*ok) = false;
    }
    return 0.0;
  }
  if (ok) {
    (*ok) = true;  
  }
  return _z[index];
}


int KstMatrix::zIndex(int x, int y) const {
  if (x >= _nX || x < 0 || y >= _nY || y < 0) {
    return -1;
  }
  int index = x * _nY + y;
  if (index >= _zSize || index < 0 ) {
    return -1;
  }
  return index;
}


bool KstMatrix::setValue(double x, double y, double z) {
  int x_index = (int)floor((x - _minX) / (double)_stepX);
  int y_index = (int)floor((y - _minY) / (double)_stepY);
  return setValueRaw(x_index, y_index, z);
}


bool KstMatrix::setValueRaw(int x, int y, double z) {
  int index = zIndex(x,y);
  if (index < 0) {
    return false;  
  }
  _z[index] = z;
  return true;
}

double KstMatrix::minValue() const {
  return _statScalars["min"]->value();  
}


double KstMatrix::maxValue() const {
  return _statScalars["max"]->value();  
}

double KstMatrix::minValueNoSpike() const {
  // FIXME: it is expensive to calcNoSpikeRange
  // so we have chosen here to only call it expicitly
  // and no attempt is made to check if it is still up to date...
  // It would be better to have these calls call 
  // calcNoSpikeRange iff the values were obsolete.
  return _minNoSpike;
}

double KstMatrix::maxValueNoSpike() const {
  // FIXME: it is expensive to calcNoSpikeRange
  // so we have chosen here to only call it expicitly
  // and no attempt is made to check if it is still up to date...
  // It would be better to have these calls call 
  // calcNoSpikeRange iff the values were obsolete.

  return _maxNoSpike;
}

void KstMatrix::calcNoSpikeRange(double per) {
  double *min_list, *max_list, min_of_max, max_of_min;
  int n_list;
  int max_n = 50000; // the most samples we will look at...
  double n_skip;
  double x=0;
  int n_notnan;

  int i,j, k;

  // count number of points which aren't nans.
  for (i=n_notnan=0; i<_NS; i++) {
    if (!KST_ISNAN(_z[i])) {
      n_notnan++;
    }
  }

  if (n_notnan==0) {
    _minNoSpike = 0;
    _maxNoSpike = 0;

    return;
  }

  per *= (double)n_notnan/(double)_NS;
  max_n *= int((double)_NS/(double)n_notnan);

  n_skip = (double)_NS/max_n;
  if (n_skip<1.0) n_skip = 1.0;

  n_list = int(double(_NS)*per/n_skip);

  min_list = (double *)malloc(n_list * sizeof(double));
  max_list = (double *)malloc(n_list * sizeof(double));


  // prefill the list
  for (i=0; i<n_list; i++) {
    j = int(i*n_skip);
    min_list[i] = 1E+300;
    max_list[i] = -1E+300;
  }
  min_of_max = -1E+300;
  max_of_min = 1E+300;

  i = n_list;
  for (j=0; j<_NS; j=int(i*n_skip), i++) {
    if (_z[j] < max_of_min) { // member for the min list
      // replace max of min with the new value
      for (k=0; k<n_list; k++) {
        if (min_list[k]==max_of_min) {
          x = min_list[k] = _z[j];
          break;
        }
      }
      max_of_min = x;
      // find the new max_of_min
      for (k=0; k<n_list; k++) {
        if (min_list[k] > max_of_min) {
          max_of_min = min_list[k];
        }
      }
    }
    if (_z[j] > min_of_max) { // member for the max list
      //printf("******** z: %g  min_of_max: %g\n", _z[j], min_of_max);
      // replace min of max with the new value
      for (k=0; k<n_list; k++) {
        if (max_list[k]==min_of_max) {
          x = max_list[k] = _z[j];
          break;
        }
      }
      // find the new min_of_max
      min_of_max = x;
      for (k=0; k<n_list; k++) {
        if (max_list[k] < min_of_max) {
          min_of_max = max_list[k];
        }
      }
    }
  }

  // FIXME: this needs a z spike insensitive algorithm...
  // it should use the update counter to see if it needs
  // to calculate it.  A call to either this or to
  // minValueNoSpike should trigger the calculation
  // which will be slow.
  _minNoSpike = max_of_min;
  _maxNoSpike = min_of_max;

  free(min_list);
  free(max_list);
}

double KstMatrix::meanValue() const {
  return _statScalars["mean"]->value();
}

double KstMatrix::minValuePositive() const {
  return _statScalars["minpos"]->value();  
}

int KstMatrix::numNew() const {
  return _numNew;  
}


void KstMatrix::resetNumNew() {
  _numNew = 0;  
}

    
QString KstMatrix::label() const {
  return _label;
}

    
void KstMatrix::zero() {
  for (int i = 0; i < _zSize; i++) {
    _z[i] = 0.0;  
  }
  setDirty();
  updateScalars();
}
    

void KstMatrix::blank() {
  for (int i = 0; i < _zSize; ++i) {
    _z[i] = KST::NOPOINT;
  }
  setDirty();
  updateScalars();
}
    

int KstMatrix::getUsage() const {
  int scalarUsage = 0;
  for (QHash<QString, KstScalar*>::ConstIterator it = _statScalars.begin(); it != _statScalars.end(); ++it) {
    scalarUsage += it.value()->getUsage() - 1;
  }
  return KstObject::getUsage() + scalarUsage;
}


KstObject::UpdateType KstMatrix::internalUpdate(KstObject::UpdateType providerUpdateType) {
  // calculate stats
  _NS = _nX * _nY;

  if (_zSize > 0) {
    double min = NAN;
    double max = NAN;
    double minpos = NAN;
    double sum = 0.0, sumsquared = 0.0;
    bool initialized = false;

    _NRealS = 0;    

    for (int i = 0; i < _zSize; i++) {
      if (finite(_z[i]) && !KST_ISNAN(_z[i])) {
        if (!initialized) {
          min = _z[i];
          max = _z[i];
          minpos = (_z[0] > 0) ? _z[0] : 1.0E300;
          initialized = true;
          _NRealS++;
        } else {
          if (min > _z[i]) {
            min = _z[i];
          }
          if (max < _z[i]) {
            max = _z[i];
          }
          if (minpos > _z[i] && _z[i] > 0) {
            minpos = _z[i];
          }
          sum += _z[i];
          sumsquared += _z[i] * _z[i];
        
          _NRealS++;
        }
      }
    }
    _statScalars["sum"]->setValue(sum);
    _statScalars["sumsquared"]->setValue(sumsquared);
    _statScalars["max"]->setValue(max);
    _statScalars["min"]->setValue(min);
    _statScalars["minpos"]->setValue(minpos);
    
    updateScalars();
    
    return setLastUpdateResult(providerUpdateType);
  } 
  return setLastUpdateResult(NO_CHANGE);
}
    
    
void KstMatrix::setTagName(const KstObjectTag& tag) {
  if (tag == this->tag()) {
    return;
  }

  KstWriteLocker l(&KST::matrixList.lock());

  KST::matrixList.doRename(this, tag);

  renameScalars();
}


const QHash<QString, KstScalar*>& KstMatrix::scalars() const {
  return _statScalars;
}
    
    
void KstMatrix::setLabel(const QString& newLabel) {
  _label = newLabel;
}


void KstMatrix::setXLabel(const QString& newLabel) {
  _xLabel = newLabel;  
}


void KstMatrix::setYLabel(const QString& newLabel) {
  _yLabel = newLabel;
}


QString KstMatrix::xLabel() const {
  return _xLabel;
}


QString KstMatrix::yLabel() const {
  return _yLabel;
}


bool KstMatrix::editable() const {
  return _editable;  
}


void KstMatrix::setEditable(bool editable) {
  _editable = editable;  
}


void KstMatrix::createScalars() {
  KstWriteLocker sl(&KST::scalarList.lock());
  KST::scalarList.setUpdateDisplayTags(false);

  _statScalars.insert("max", new KstScalar(KstObjectTag("Max", tag()), this));
  _statScalars["max"]->_KShared_ref();
  _statScalars.insert("min", new KstScalar(KstObjectTag("Min", tag()), this));
  _statScalars["min"]->_KShared_ref();
  _statScalars.insert("mean", new KstScalar(KstObjectTag("Mean", tag()), this));
  _statScalars["mean"]->_KShared_ref();
  _statScalars.insert("sigma", new KstScalar(KstObjectTag("Sigma", tag()), this));
  _statScalars["sigma"]->_KShared_ref();
  _statScalars.insert("rms", new KstScalar(KstObjectTag("Rms", tag()), this));
  _statScalars["rms"]->_KShared_ref();
  _statScalars.insert("ns", new KstScalar(KstObjectTag("NS", tag()), this));
  _statScalars["ns"]->_KShared_ref();
  _statScalars.insert("sum", new KstScalar(KstObjectTag("Sum", tag()), this));
  _statScalars["sum"]->_KShared_ref();
  _statScalars.insert("sumsquared", new KstScalar(KstObjectTag("SumSquared", tag()), this));
  _statScalars["sumsquared"]->_KShared_ref();
  _statScalars.insert("minpos", new KstScalar(KstObjectTag("MinPos", tag()), this));
  _statScalars["minpos"]->_KShared_ref();

  KST::scalarList.setUpdateDisplayTags(true);
}


void KstMatrix::renameScalars() {
  KstWriteLocker sl(&KST::scalarList.lock());
  KST::scalarList.setUpdateDisplayTags(false);

  _statScalars["max"]->setTagName(KstObjectTag("Max", tag()));
  _statScalars["min"]->setTagName(KstObjectTag("Min", tag()));
  _statScalars["mean"]->setTagName(KstObjectTag("Mean", tag()));
  _statScalars["sigma"]->setTagName(KstObjectTag("Sigma", tag()));
  _statScalars["rms"]->setTagName(KstObjectTag("Rms", tag()));
  _statScalars["ns"]->setTagName(KstObjectTag("NS", tag()));
  _statScalars["sum"]->setTagName(KstObjectTag("Sum", tag()));
  _statScalars["sumsquared"]->setTagName(KstObjectTag("SumSquared", tag()));
  _statScalars["minpos"]->setTagName(KstObjectTag("MinPos", tag()));

  KST::scalarList.setUpdateDisplayTags(true);
}


void KstMatrix::updateScalars() {  
  _statScalars["ns"]->setValue(_NS);
  if (_NRealS >= 2) {
    _statScalars["mean"]->setValue(_statScalars["sum"]->value()/double(_NRealS));
    _statScalars["sigma"]->setValue( sqrt(
        (_statScalars["sumsquared"]->value() - _statScalars["sum"]->value()*_statScalars["sum"]->value()/double(_NRealS))/ double(_NRealS-1) ) );
    _statScalars["rms"]->setValue(sqrt(_statScalars["sumsquared"]->value()/double(_NRealS)));
  } else {
    _statScalars["sigma"]->setValue(_statScalars["max"]->value() - _statScalars["min"]->value());
    _statScalars["rms"]->setValue(sqrt(_statScalars["sumsquared"]->value()));
    _statScalars["mean"]->setValue(0);
  }
}


bool KstMatrix::resizeZ(int sz, bool reinit) {
  //kdDebug() << "resizing to: " << sz << endl;
  if (sz >= 1) {
    _z = static_cast<double*>(KST::realloc(_z, sz*sizeof(double)));
    if (!_z) {
      return false;
    }
#ifdef ZERO_MEMORY
    if (reinit && _zSize < sz) {
#if ZERO_MEMORY == 2
      memset(&_z[_zSize], 0, (sz - _zSize)*sizeof(double));
      
#else
      for (int i = _zSize; i < sz; i++) {
        _z[i] = 0.0;
      }
#endif
    }
#else
    abort();  // avoid unpleasant surprises
#endif
    _zSize = sz;
    updateScalars();
  }

  setDirty();
  return true;
}


bool KstMatrix::resize(int xSize, int ySize, bool reinit) {
  int oldNX = _nX;
  int oldNY = _nY;
  _nX = xSize;
  _nY = ySize;
  if (resizeZ(xSize*ySize, reinit)) {
    return true;  
  } else {
    _nX = oldNX;
    _nY = oldNY;
    return false;  
  }
}


void KstMatrix::save(QTextStream &ts, const QString& indent) {
  Q_UNUSED(ts);
  Q_UNUSED(indent);

  // no saving
}


bool KstMatrix::saveable() const {
  return _saveable;  
}


void KstMatrix::change(const KstObjectTag& newTag, uint nX, uint nY, double minX, double minY, double stepX, double stepY) {
  if (tag() != newTag) {
    setTagName(newTag);
  }
  _nX = nX;
  _nY = nY;
  _stepX = stepX;
  _stepY = stepY;
  _minX = minX;
  _minY = minY;

  setDirty();
}

// vim: ts=2 sw=2 et
