/***************************************************************************
                          ksthistogram.cpp: Histogram for KST
                             -------------------
    begin                : July 2002
    copyright            : (C) 2002 by C. Barth Netterfield
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

#include <assert.h>
#include <math.h>
#include <stdlib.h>

// include files for Qt
#include <qstylesheet.h>

// include files for KDE
#include <kglobal.h>
#include <klocale.h>

// application specific includes
#include "dialoglauncher.h"
#include "kstdatacollection.h"
#include "kstdefaultnames.h"
#include "ksthistogram.h"

static const QString& RAWVECTOR  = KGlobal::staticQString("I");
static const QString& BINS = KGlobal::staticQString("B");
static const QString& HIST = KGlobal::staticQString("H");

KstHistogram::KstHistogram(const QString &in_tag, KstVectorPtr in_V,
                           double xmin_in, double xmax_in,
                           int in_n_bins,
                           KstHsNormType in_norm_mode)
: KstDataObject() {
  setRealTimeAutoBin(false);

  commonConstructor(in_tag, in_V, xmin_in, xmax_in, in_n_bins, in_norm_mode);
}

KstHistogram::KstHistogram(const QDomElement &e)
: KstDataObject(e) {
  KstHsNormType in_norm_mode;
  KstVectorPtr in_V;
  QString rawName;
  QString in_tag;
  double xmax_in =  1.0;
  double xmin_in = -1.0;
  int in_n_bins = 10;

  setRealTimeAutoBin(false);

  in_norm_mode = KST_HS_NUMBER;

  QDomNode n = e.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if( !e.isNull() ) { // the node was really an element.
      if (e.tagName() == "tag") {
        in_tag = e.text();
      } else if (e.tagName() == "vectag") {
        rawName = e.text();
      } else if (e.tagName() == "NormMode") {
        if (e.text()=="NUMBER") {
          in_norm_mode = KST_HS_NUMBER;
        } else if (e.text()=="PERCENT") {
          in_norm_mode = KST_HS_PERCENT;
        } else if (e.text()=="FRACTION") {
          in_norm_mode = KST_HS_FRACTION;
        } else if (e.text()=="MAX_ONE") {
          in_norm_mode = KST_HS_MAX_ONE;
        }
      } else if (e.tagName() == "minX") {
        xmin_in = e.text().toDouble();
      } else if (e.tagName() == "maxX") {
        xmax_in = e.text().toDouble();
      } else if (e.tagName() == "numBins") {
        in_n_bins = e.text().toInt();
      } else if (e.tagName() == "realtimeautobin") {
        _realTimeAutoBin = (e.text() != "0");
      }
    }
    n = n.nextSibling();
  }

  _inputVectorLoadQueue.append(qMakePair(RAWVECTOR, rawName));
  commonConstructor(in_tag, in_V, xmin_in, xmax_in, in_n_bins, in_norm_mode);
}


void KstHistogram::commonConstructor(const QString &in_tag, KstVectorPtr in_V,
                                     double xmin_in,
                                     double xmax_in,
                                     int in_n_bins,
                                     KstHsNormType in_norm_mode) {
  _typeString = i18n("Histogram");
  _type = "Histogram";
  _NormMode = in_norm_mode;
  _Bins = 0L;
  _NBins = 0;
  
  setTagName(KstObjectTag::fromString(in_tag));
  _inputVectors[RAWVECTOR] = in_V;

  if (xmax_in>xmin_in) {
    _MaxX = xmax_in;
    _MinX = xmin_in;
  } else {
    _MinX = xmax_in;
    _MaxX = xmin_in;
  }
  if (_MaxX == _MinX) {
    _MaxX += 1.0;
    _MinX -= 1.0;
  }

  _NBins = in_n_bins;
  if (_NBins < 2) {
    _NBins = 2;
  }
  _Bins = new unsigned long[_NBins];
  _NS = 3 * _NBins + 1;

  KstVectorPtr v = new KstVector(KstObjectTag("bins", tag()), _NBins, this);
  _bVector = _outputVectors.insert(BINS, v);

  v = new KstVector(KstObjectTag("sv", tag()), _NBins, this);
  _hVector = _outputVectors.insert(HIST, v);

  setDirty();
}


KstHistogram::~KstHistogram() {
  _bVector = _outputVectors.end();
  _hVector = _outputVectors.end();
  KST::vectorList.lock().writeLock();
  KST::vectorList.remove(_outputVectors[BINS]);
  KST::vectorList.remove(_outputVectors[HIST]);
  KST::vectorList.lock().unlock();

  delete[] _Bins;
  _Bins = 0L;
}


KstObject::UpdateType KstHistogram::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  writeLockInputsAndOutputs();

  if (update_counter <= 0) {
    assert(update_counter == 0);
    force = true;
  }

  bool xUpdated = KstObject::UPDATE == _inputVectors[RAWVECTOR]->update(update_counter);
  if (!xUpdated && !force) {
    unlockInputsAndOutputs();
    return setLastUpdateResult(KstObject::NO_CHANGE);
  }

  int i_bin, i_pt, ns;
  double y = 0.0;
  double MaxY = 0.0;
  // do auto-binning if necessary
  if (_realTimeAutoBin) {
    int temp_NBins;
    double temp_xMin, temp_xMax;
    KstHistogram::AutoBin(_inputVectors[RAWVECTOR], &temp_NBins, &temp_xMax, &temp_xMin);
    internalSetNBins(temp_NBins);
    setXRange(temp_xMin, temp_xMax);
  }

  _NS = 3 * _NBins + 1;
  _W = (_MaxX - _MinX)/double(_NBins);

  memset(_Bins, 0, _NBins*sizeof(*_Bins));

  ns = _inputVectors[RAWVECTOR]->length();
  for (i_pt = 0; i_pt < ns ; i_pt++) {
    y = _inputVectors[RAWVECTOR]->interpolate(i_pt, ns);
    i_bin = (int)floor((y-_MinX)/_W);
    if (i_bin >= 0 && i_bin < _NBins) {
      _Bins[i_bin]++;
    } else {
      // the top boundry of the top bin is included in the top bin.
      // for all other bins, the top boundry is included in the next bin
      if (y == _MaxX) {
        _Bins[_NBins-1]++;
      }
    }
  }

  for (i_bin=0; i_bin<_NBins; i_bin++) {
    y = _Bins[i_bin];
    if (y > MaxY) {
      MaxY = y;
    }
  }

  switch (_NormMode) {
    case KST_HS_NUMBER:
      _Normalization = 1.0;
      (*_hVector)->setLabel(i18n("Number in bin"));
      break;
    case KST_HS_PERCENT:
      if (ns > 0) {
        _Normalization = 100.0/(double)ns;
      } else {
        _Normalization = 1.0;
      }
      (*_hVector)->setLabel(i18n("Percent in bin"));
      break;
    case KST_HS_FRACTION:
      if (ns > 0) {
        _Normalization = 1.0/(double)ns;
      } else {
        _Normalization = 1.0;
      }
      (*_hVector)->setLabel(i18n("Fraction in bin"));
      break;
    case KST_HS_MAX_ONE:
      if (MaxY > 0) {
        _Normalization = 1.0/MaxY;
      } else {
        _Normalization = 1.0;
      }
      (*_hVector)->setLabel("");
      break;
    default:
      _Normalization = 1.0;
      break;
  }

  (*_bVector)->setLabel(_inputVectors[RAWVECTOR]->tagName());

  double *bins = (*_bVector)->value();
  double *hist = (*_hVector)->value();

  for ( i_bin = 0; i_bin<_NBins; i_bin++ ) {
    bins[i_bin] = ( double( i_bin ) + 0.5 )*_W + _MinX;
    hist[i_bin] = _Bins[i_bin]*_Normalization;
  }
  
  (*_bVector)->setDirty();
  (*_bVector)->update(update_counter);
  (*_hVector)->setDirty();
  (*_hVector)->update(update_counter);

  unlockInputsAndOutputs();

  return setLastUpdateResult(UPDATE);
}


int KstHistogram::nBins() const {
  return _NBins;
}


void KstHistogram::setXRange(double xmin_in, double xmax_in) {
  if (xmax_in > xmin_in) {
    _MaxX = xmax_in;
    _MinX = xmin_in;
  } else if (xmax_in < xmin_in) {
    _MinX = xmax_in;
    _MaxX = xmin_in;
  } else {
    _MinX = xmax_in - 1.0;
    _MaxX = xmax_in + 1.0;
  }
  _W = (_MaxX - _MinX)/double(_NBins);
}


void KstHistogram::internalSetNBins(int in_n_bins) {
  if (in_n_bins < 2) {
    in_n_bins = 2;
  }
  if (_NBins != in_n_bins) {
    _NBins = in_n_bins;
    delete[] _Bins;
    _Bins = new unsigned long[_NBins];
    memset(_Bins, 0, _NBins*sizeof(*_Bins));
    (*_bVector)->resize(_NBins);
    (*_hVector)->resize(_NBins);
  }  
  _W = (_MaxX - _MinX)/double(_NBins);
  _NS = 3 * _NBins + 1;
}


void KstHistogram::setNBins(int in_n_bins) {
  _realTimeAutoBin = false;
  internalSetNBins(in_n_bins);
  setDirty();
}


QString KstHistogram::vTag() const {
  return _inputVectors[RAWVECTOR]->tag().displayString();
}


void KstHistogram::setVector(KstVectorPtr new_v) {
  _inputVectors[RAWVECTOR] = new_v;
}


QString KstHistogram::yLabel() const {
  switch (_NormMode) {
    case KST_HS_NUMBER:
      return i18n("Number in Bin");
      break;
    case KST_HS_PERCENT:
      return i18n("Percent in Bin");
      break;
    case KST_HS_FRACTION:
      return i18n("Fraction in Bin");
      break;
    case KST_HS_MAX_ONE:
      return i18n("Histogram");
      break;
  }
  return i18n("Histogram");
}


QString KstHistogram::xLabel() const {
  return _inputVectors[RAWVECTOR]->label();
}


void KstHistogram::save(QTextStream &ts, const QString& indent) {
  // FIXME: clean this up - all lower case nodes, maybe save points in the
  // point class itself, etc
  QString l2 = indent + "  ";
  ts << indent << "<histogram>" << endl;
  ts << l2 << "<tag>" << QStyleSheet::escape(tagName()) << "</tag>" << endl;
  ts << l2 << "<vectag>" << QStyleSheet::escape(_inputVectors[RAWVECTOR]->tag().tagString()) << "</vectag>" << endl;
  ts << l2 << "<numBins>"  << _NBins << "</numBins>" << endl;
  ts << l2 << "<realtimeautobin>" << _realTimeAutoBin << "</realtimeautobin>" << endl;
  ts << l2 << "<minX>" << _MinX << "</minX>" << endl;
  ts << l2 << "<maxX>" << _MaxX << "</maxX>" << endl;
  switch (_NormMode) {
    case KST_HS_NUMBER:
      ts << l2 << "<NormMode>NUMBER</NormMode>" << endl;
      break;
    case KST_HS_PERCENT:
      ts << l2 << "<NormMode>PERCENT</NormMode>" << endl;
      break;
    case KST_HS_FRACTION:
      ts << l2 << "<NormMode>FRACTION</NormMode>" << endl;
      break;
    case KST_HS_MAX_ONE:
      ts << l2 << "<NormMode>MAX_ONE</NormMode>" << endl;
      break;
  }
  ts << indent << "</histogram>" << endl;
}


QString KstHistogram::propertyString() const {
  return i18n("Histogram: %1").arg(vTag());
}


void KstHistogram::showNewDialog() {
  KstDialogs::self()->showHistogramDialog();
}


void KstHistogram::showEditDialog() {
  KstDialogs::self()->showHistogramDialog(tagName(), true);
}


void KstHistogram::AutoBin(KstVectorPtr V, int *n, double *max, double *min) {
  double m;

  *max = V->max();
  *min = V->min();
  *n = V->length();

  if (*max < *min) {
    m = *max;
    *max = *min;
    *min = m;
  }
  
  if (*max == *min) {
    *max += 1.0;
    *min -= 1.0;
  }

  // we can do a better job auto-ranging using the tick rules from plot...
  // this has not been done yet, you will notice...
  *n /= 50;
  if (*n < 6) {
    *n = 6;
  }
  if (*n > 60) {
    *n = 60;
  }

  m = (*max - *min)/(100.0*double(*n));
  *max += m;
  *min -= m;
}


bool KstHistogram::slaveVectorsUsed() const {
  return true;
}


void KstHistogram::setRealTimeAutoBin(bool autoBin) {
  _realTimeAutoBin = autoBin;
}


bool KstHistogram::realTimeAutoBin() const {
  return _realTimeAutoBin;
}


double KstHistogram::vMax() const {
  return _inputVectors[RAWVECTOR]->max();
}


double KstHistogram::vMin() const {
  return _inputVectors[RAWVECTOR]->min();
}


int KstHistogram::vNumSamples() const {
  return _inputVectors[RAWVECTOR]->length();
}


KstDataObjectPtr KstHistogram::makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap) {
  QString name(tagName() + '\'');
  while (KstData::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  KstHistogramPtr histogram = new KstHistogram(name, _inputVectors[RAWVECTOR],
                                               _MinX, _MaxX, _NBins, _NormMode);
  duplicatedMap.insert(this, KstDataObjectPtr(histogram));
  return KstDataObjectPtr(histogram);
}


// vim: ts=2 sw=2 et
