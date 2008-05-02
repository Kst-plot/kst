/***************************************************************************
                          histogram.cpp: Histogram for KST
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

#include <QTextDocument>
#include <QXmlStreamWriter>
#include "kst_i18n.h"

#include "dialoglauncher.h"
#include "datacollection.h"
#include "defaultnames.h"
#include "histogram.h"
#include "objectstore.h"

namespace Kst {

const QString Histogram::staticTypeString = I18N_NOOP("Histogram");
const QString Histogram::staticTypeTag = I18N_NOOP("histogram");

static const QLatin1String& RAWVECTOR  = QLatin1String("I");
static const QLatin1String& BINS = QLatin1String("B");
static const QLatin1String& HIST = QLatin1String("H");

static int _hnum = 1;

Histogram::Histogram(ObjectStore *store, const ObjectTag &in_tag)
  : DataObject(store, in_tag) {
  setRealTimeAutoBin(false);
  commonConstructor(store, 0L, 0, 0, 0, Number);
}

Histogram::Histogram(ObjectStore *store, const ObjectTag &in_tag, VectorPtr in_V,
                           double xmin_in, double xmax_in,
                           int in_n_bins,
                           NormalizationType in_norm_mode)
: DataObject(store, in_tag) {
  setRealTimeAutoBin(false);

  commonConstructor(store, in_V, xmin_in, xmax_in, in_n_bins, in_norm_mode);

}

Histogram::Histogram(ObjectStore *store, const QDomElement &e)
: DataObject(store, e) {
  NormalizationType in_norm_mode;
  VectorPtr in_V;
  QString rawName;
  QString in_tag;
  double xmax_in =  1.0;
  double xmin_in = -1.0;
  int in_n_bins = 10;

  setRealTimeAutoBin(false);

  in_norm_mode = Number;

  QDomNode n = e.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if( !e.isNull() ) { // the node was really an element.
      if (e.tagName() == "tag") {
        in_tag = e.text();
      } else if (e.tagName() == "vectag") {
        rawName = e.text();
      } else if (e.tagName() == "normmode") {
        if (e.text()=="NUMBER") {
          in_norm_mode = Number;
        } else if (e.text()=="PERCENT") {
          in_norm_mode = Percent;
        } else if (e.text()=="FRACTION") {
          in_norm_mode = Fraction;
        } else if (e.text()=="MAX_ONE") {
          in_norm_mode = MaximumOne;
        }
      } else if (e.tagName() == "min") {
        xmin_in = e.text().toDouble();
      } else if (e.tagName() == "max") {
        xmax_in = e.text().toDouble();
      } else if (e.tagName() == "numbins") {
        in_n_bins = e.text().toInt();
      } else if (e.tagName() == "realtimeautobin") {
        _realTimeAutoBin = (e.text() != "0");
      }
    }
    n = n.nextSibling();
  }

  _inputVectorLoadQueue.append(qMakePair(QString(RAWVECTOR), rawName));
  commonConstructor(store, in_V, xmin_in, xmax_in, in_n_bins, in_norm_mode);
}


void Histogram::commonConstructor(ObjectStore *store,
                                  VectorPtr in_V,
                                  double xmin_in,
                                  double xmax_in,
                                  int in_n_bins,
                                  NormalizationType in_norm_mode) {
  _typeString = staticTypeString;
  _type = "Histogram";
  _NormalizationMode = in_norm_mode;
  _Bins = 0L;
  _NumberOfBins = 0;

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

  _NumberOfBins = in_n_bins;
  if (_NumberOfBins < 2) {
    _NumberOfBins = 2;
  }
  _Bins = new unsigned long[_NumberOfBins];
  _NS = 3 * _NumberOfBins + 1;

  Q_ASSERT(store);
  VectorPtr v = store->createObject<Vector>(ObjectTag("bins", tag()));
  v->setProvider(this);
  v->setSlaveName("bin");
  v->resize(_NumberOfBins);
  _bVector = _outputVectors.insert(BINS, v).value();

  v = store->createObject<Vector>(ObjectTag("sv", tag()));
  v->setProvider(this);
  v->setSlaveName("num");
  v->resize(_NumberOfBins);
  _hVector = _outputVectors.insert(HIST, v).value();

  _shortName = "H"+QString::number(_hnum++);

  setDirty();
}


Histogram::~Histogram() {
  _bVector = 0L;
  _hVector = 0L;

  delete[] _Bins;
  _Bins = 0L;
}


Object::UpdateType Histogram::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (Object::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  writeLockInputsAndOutputs();

  if (update_counter <= 0) {
    assert(update_counter == 0);
    force = true;
  }

  bool xUpdated = Object::UPDATE == _inputVectors[RAWVECTOR]->update(update_counter);
  if (!xUpdated && !force) {
    unlockInputsAndOutputs();
    return setLastUpdateResult(Object::NO_CHANGE);
  }

  int i_bin, i_pt, ns;
  double y = 0.0;
  double MaxY = 0.0;
  // do auto-binning if necessary
  if (_realTimeAutoBin) {
    int temp_NumberOfBins;
    double temp_xMin, temp_xMax;
    Histogram::AutoBin(_inputVectors[RAWVECTOR], &temp_NumberOfBins, &temp_xMax, &temp_xMin);
    internalSetNumberOfBins(temp_NumberOfBins);
    setXRange(temp_xMin, temp_xMax);
  }

  _NS = 3 * _NumberOfBins + 1;
  _W = (_MaxX - _MinX)/double(_NumberOfBins);

  memset(_Bins, 0, _NumberOfBins*sizeof(*_Bins));

  ns = _inputVectors[RAWVECTOR]->length();
  for (i_pt = 0; i_pt < ns ; i_pt++) {
    y = _inputVectors[RAWVECTOR]->interpolate(i_pt, ns);
    i_bin = (int)floor((y-_MinX)/_W);
    if (i_bin >= 0 && i_bin < _NumberOfBins) {
      _Bins[i_bin]++;
    } else {
      // the top boundry of the top bin is included in the top bin.
      // for all other bins, the top boundry is included in the next bin
      if (y == _MaxX) {
        _Bins[_NumberOfBins-1]++;
      }
    }
  }

  for (i_bin=0; i_bin<_NumberOfBins; i_bin++) {
    y = _Bins[i_bin];
    if (y > MaxY) {
      MaxY = y;
    }
  }

  switch (_NormalizationMode) {
    case Number:
      _Normalization = 1.0;
      _hVector->setLabel(i18n("Number in bin"));
      break;
    case Percent:
      if (ns > 0) {
        _Normalization = 100.0/(double)ns;
      } else {
        _Normalization = 1.0;
      }
      _hVector->setLabel(i18n("Percent in bin"));
      break;
    case Fraction:
      if (ns > 0) {
        _Normalization = 1.0/(double)ns;
      } else {
        _Normalization = 1.0;
      }
      _hVector->setLabel(i18n("Fraction in bin"));
      break;
    case MaximumOne:
      if (MaxY > 0) {
        _Normalization = 1.0/MaxY;
      } else {
        _Normalization = 1.0;
      }
      _hVector->setLabel("");
      break;
    default:
      _Normalization = 1.0;
      break;
  }

  _bVector->setLabel(_inputVectors[RAWVECTOR]->tag().displayString());

  double *bins = _bVector->value();
  double *hist = _hVector->value();

  for ( i_bin = 0; i_bin<_NumberOfBins; i_bin++ ) {
    bins[i_bin] = ( double( i_bin ) + 0.5 )*_W + _MinX;
    hist[i_bin] = _Bins[i_bin]*_Normalization;
  }

  _bVector->setDirty();
  _bVector->update(update_counter);
  _hVector->setDirty();
  _hVector->update(update_counter);

  unlockInputsAndOutputs();

  return setLastUpdateResult(UPDATE);
}


int Histogram::numberOfBins() const {
  return _NumberOfBins;
}


void Histogram::setXRange(double xmin_in, double xmax_in) {
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
  _W = (_MaxX - _MinX)/double(_NumberOfBins);
}


void Histogram::internalSetNumberOfBins(int in_n_bins) {
  if (in_n_bins < 2) {
    in_n_bins = 2;
  }
  if (_NumberOfBins != in_n_bins) {
    _NumberOfBins = in_n_bins;
    delete[] _Bins;
    _Bins = new unsigned long[_NumberOfBins];
    memset(_Bins, 0, _NumberOfBins*sizeof(*_Bins));
    _bVector->resize(_NumberOfBins);
    _hVector->resize(_NumberOfBins);
  }
  _W = (_MaxX - _MinX)/double(_NumberOfBins);
  _NS = 3 * _NumberOfBins + 1;
}


void Histogram::setNumberOfBins(int in_n_bins) {
  _realTimeAutoBin = false;
  internalSetNumberOfBins(in_n_bins);
  setDirty();
}


QString Histogram::vTag() const {
  return _inputVectors[RAWVECTOR]->tag().displayString();
}


void Histogram::setVector(VectorPtr new_v) {
  if (new_v) {
    connect(new_v, SIGNAL(vectorUpdated(ObjectPtr, int)), this, SLOT(vectorUpdated(ObjectPtr, int)));
    _inputVectors[RAWVECTOR] = new_v;
  }
}


VectorPtr Histogram::vector() const {
  return _inputVectors[RAWVECTOR];
}


QString Histogram::yLabel() const {
  switch (_NormalizationMode) {
    case Number:
      return i18n("Number in Bin");
      break;
    case Percent:
      return i18n("Percent in Bin");
      break;
    case Fraction:
      return i18n("Fraction in Bin");
      break;
    case MaximumOne:
      return i18n("Histogram");
      break;
  }
  return i18n("Histogram");
}


QString Histogram::xLabel() const {
  return _inputVectors[RAWVECTOR]->label();
}

void Histogram::save(QXmlStreamWriter &xml) {
  xml.writeStartElement(staticTypeTag);
  xml.writeAttribute("tag", tag().tagString());
  xml.writeAttribute("vector", _inputVectors[RAWVECTOR]->tag().tagString());
  xml.writeAttribute("numberofbins", QString::number(_NumberOfBins));
  xml.writeAttribute("realtimeautobin", QVariant(_realTimeAutoBin).toString());
  xml.writeAttribute("min", QString::number(_MinX));
  xml.writeAttribute("max", QString::number(_MaxX));
  xml.writeAttribute("normalizationmode", QString::number(_NormalizationMode));
  xml.writeEndElement();
}


QString Histogram::propertyString() const {
  return i18n("Histogram: %1").arg(vTag());
}


void Histogram::showNewDialog() {
  DialogLauncher::self()->showHistogramDialog();
}


void Histogram::showEditDialog() {
  DialogLauncher::self()->showHistogramDialog(this);
}


void Histogram::AutoBin(VectorPtr V, int *n, double *max, double *min) {
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


bool Histogram::slaveVectorsUsed() const {
  return true;
}


void Histogram::setRealTimeAutoBin(bool autoBin) {
  _realTimeAutoBin = autoBin;
}


bool Histogram::realTimeAutoBin() const {
  return _realTimeAutoBin;
}


double Histogram::vMax() const {
  return _inputVectors[RAWVECTOR]->max();
}


double Histogram::vMin() const {
  return _inputVectors[RAWVECTOR]->min();
}


int Histogram::vNumSamples() const {
  return _inputVectors[RAWVECTOR]->length();
}


DataObjectPtr Histogram::makeDuplicate() {
  QString newTag = tag().name() + "'";

  HistogramPtr histogram = store()->createObject<Histogram>(ObjectTag::fromString(newTag));

  histogram->setVector(_inputVectors[RAWVECTOR]);
  histogram->setXRange(_MinX, _MaxX);
  histogram->setNumberOfBins(_NumberOfBins);
  histogram->setNormalizationType(_NormalizationMode);
  histogram->setRealTimeAutoBin(_realTimeAutoBin);

  histogram->writeLock();
  histogram->update(0);
  histogram->unlock();

  return DataObjectPtr(histogram);
}

QString Histogram::_automaticDescriptiveName() {
  return (_inputVectors[RAWVECTOR]->descriptiveName());
}

}
// vim: ts=2 sw=2 et
