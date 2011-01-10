/***************************************************************************
                    dmc.cpp  -  data source - planck DMC
                             -------------------
    begin                : Wed July 4 2007
    copyright            : (C) 2007 The University of Toronto
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

#include "dmcsource.h"
#include "debug.h"
#include "kst_i18n.h"

#include <assert.h>
#include <math.h>

#include <QDir>
#include <QFile>

using namespace Kst;



static QString dmcTypeString = I18N_NOOP("PLANCK DMC I/O");

const QString& DmcSource::typeString() const
{
  return dmcTypeString;
}


const QString DmcSource::dmcTypeKey()
{
  return ::dmcTypeString;
}



DmcSource::DmcSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e) :
    Kst::DataSource(store, cfg, filename, type)
{
  _filename = filename;

  _dmcObject = new DMC::Object;
  if (!_dmcObject->setGroup(filename)) {
    _dmcObject = 0L; // deref
  }

  if (_dmcObject && _dmcObject->isValid()) {
    _fieldList = _dmcObject->fields();
    _fieldList.prepend("INDEX");
    update();
    _valid = true;
  }
  //qDebug() << "Planck source " << (void*)this << " created" << endl;
}


DmcSource::~DmcSource() {
  //qDebug() << "Planck source " << (void*)this << " destroyed" << endl;
}


void DmcSource::reset() {
  _valid = false;
  _dmcObject = new DMC::Object; // kshared - deletes the old one
  if (!_dmcObject->setGroup(_filename)) {
    _dmcObject = 0L; // deref
  }

  _fieldList.clear();
  if (_dmcObject && _dmcObject->isValid()) {
    _fieldList = _dmcObject->fields();
    _fieldList.prepend("INDEX");
    update();
    _valid = true;
  }
}


Object::UpdateType DmcSource::internalDataSourceUpdate() {
#warning port
  /*
  if (_valid && _dmcObject && _dmcObject->updated()) {
    updateNumFramesScalar();
    return setLastUpdateResult(Object::Updated);
  }
  return setLastUpdateResult(Object::NoChange);
  */
  return Object::UpdateType();
}


int DmcSource::readField(double *v, const QString& field, int s, int n) {
  //qDebug() << "Planck read field " << field << " - " << n << " samples from " << s << endl;
  if (field.toLower() == "index") {
    if (n < 0) {
      v[0] = double(s);
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i);
    }
    return n > 0 ? n : 0;
  }

  if (!_valid || !_dmcObject || !_dmcObject->isValid()) {
    qDebug() << "tried to read from an invalid planck dmc source" << endl;
    qDebug() << "plugin is valid? " << _valid << endl;
    qDebug() << "Object object is non-null? " << (_dmcObject.data() != 0) << endl;
    return -1;
  }

  QSize sz = _dmcObject->range(field);
  long start = sz.width(), end = sz.height();
  long count = end - start + 1;

  if (s >= count) { // start past the end
    return 0;
  }

  if (n < 0) { // reading less than 0 -> read only one sample!
    n = 1;
  }

  if (s + n > count) { // trying to read past the end
    n = count - s;
  }

  if (s + start > end) {
    qDebug() << "Nothing to read: (" << start << "," << end << ") " << s << endl;
    return 0;
  }

  assert(n != 0);
  return _dmcObject->readObject(field, v, start + s, start + s + n - 1);
}


int DmcSource::readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead) {
  if (*lastFrameRead) {
    *lastFrameRead = -1;
  }

  if (field.toLower() == "index") {
    if (n < 0) {
      v[0] = double(s);
      if (*lastFrameRead) {
        *lastFrameRead = -1; // FIXME
      }
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i*skip);
    }
    if (*lastFrameRead) {
      *lastFrameRead = -1; // FIXME
    }
    return n > 0 ? n : 0;
  }

  if (!_valid || !_dmcObject || !_dmcObject->isValid()) {
    return -1;
  }

  QSize sz = _dmcObject->range(field);
  long start = sz.width(), end = sz.height();
  long count = end - start + 1;

  if (s >= count) { // start past the end
    return 0;
  }

  if (n < 0) { // reading less than 0 -> read only one sample!
    n = 1;
  }

  if (s + (n-1)*skip >= count) { // trying to read past the end
    qDebug() << "TRYING TO READ PAST END.  n=" << n << endl;
    // n = ceil((count-s)/skip)
    if ((count - s) % skip == 0) {
      n = (count - s) / skip;
    } else {
      n = (count - s) / skip + 1;
    }
    qDebug() << "N IS NOW=" << n << endl;
  }

  if (s + start > end) {
    qDebug() << "Nothing to read: (" << start << "," << end << ") " << s << endl;
    return 0;
  }

  // PIOLib doesn't have a mechanism to read with skip.  This needs to be added
  // later.  For now, we read into a temp buffer, then extract what we need.
  double *tmp = new double[(n - 1) * skip + 1];
  int rc = _dmcObject->readObject(field, tmp, start + s, start + s + (n - 1) * skip);
  //qDebug() << "readObject rc=" << rc << " from=" << start+s << " to=" << start + s + (n - 1) * skip << endl;
  int i = 0;
  while (i < n && i*skip < rc) {
    v[i] = tmp[i * skip];
    ++i;
  }
  delete[] tmp;

  if (*lastFrameRead) {
    *lastFrameRead = -1; // FIXME
  }

  return i;
}


bool DmcSource::isValidField(const QString& field) const {
  return field.toLower() == "index" || _fieldList.contains(field);
}


int DmcSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int DmcSource::frameCount(const QString& field) const {
  if (!_valid) {
    return 0;
  }

  if (field.isEmpty() || field.toLower() == "index" || !_fieldList.contains(field)) {
    int maxLen = 0;
    for (QStringList::ConstIterator i = _fieldList.begin(); i != _fieldList.end(); ++i) {
      assert(!(*i).isEmpty());
      QSize sz = _dmcObject->range(*i);
      maxLen = qMax(sz.height() - sz.width() + 1, maxLen);
    }
    return maxLen;
  }

  QSize sz = _dmcObject->range(field);
  return sz.height() - sz.width() + 1;
}


QString DmcSource::fileType() const {
  return "PLANCK DMC I/O";
}


void DmcSource::save(QTextStream &ts, const QString& indent) {
#warning port
  //DataSource::save(ts, indent);
}


bool DmcSource::isEmpty() const {
  if (!_valid) {
    return true;
  }

  for (QStringList::ConstIterator i = _fieldList.begin(); i != _fieldList.end(); ++i) {
    QSize sz = _dmcObject->range(*i);
    if (sz.height() - sz.width() + 1 > 0) {
      return false;
    }
  }

  return true;
}



bool DmcSource::supportsTimeConversions() const {
  return _fieldList.contains("TIMES_OF_SAMPLES") || _fieldList.contains("TIMESEC");
}


int DmcSource::sampleForTime(const QDateTime& time, bool *ok) {
  if (!_valid) {
    if (ok) {
      *ok = false;
    }
    return -1;
  }

  int tai = time.toTime_t() + 86400 * (365 * 12 + 3);
  return _dmcObject->sampleForTime(tai, ok);
}


int DmcSource::sampleForTime(double ms, bool *ok) {
  if (!_valid) {
    if (ok) {
      *ok = false;
    }
    return -1;
  }

  return _dmcObject->sampleForTimeRelative(ms, ok);
}


QDateTime DmcSource::timeForSample(int sample, bool *ok) {
  if (!_valid) {
    if (ok) {
      *ok = false;
    }
    return QDateTime();
  }

  double t = _dmcObject->timeForSample(sample);
  if (t != t) {
    if (ok) {
      *ok = false;
    }
    return QDateTime();
  }

  QDateTime dt;
  dt.setTime_t(int(floor(t + 0.5)) - (86400 * (365 * 12 + 3)));
  return dt;
}


double DmcSource::relativeTimeForSample(int sample, bool *ok) {
  if (!_valid) {
    if (ok) {
      *ok = false;
    }
    return -1;
  }

  double i = _dmcObject->timeForSample(0);
  double t = _dmcObject->timeForSample(sample);
  if (t != t || i != i) {
    if (ok) {
      *ok = false;
    }
    return -1;
  }

  return floor(t - i + 0.5) * 1000.0;
}


