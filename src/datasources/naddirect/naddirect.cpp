/***************************************************************************
                    naddirect.cpp  -  Direct NAD plugin
                             -------------------
    begin                : Wed Mar 22 2006
    copyright            : (C) 2006 Staikos Computing Services Inc.
    email                : info@staikos.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "naddirect.h"

#include <qapplication.h>
#include <qevent.h>
#include <qwaitcondition.h>
#include <ksdebug.h>
#include <kurl.h>
 
NADDirectSource::NADDirectSource(KConfig *cfg, const QString& filename, const QString& type)
: KstDataSource(cfg, filename, type) {
  kstdDebug() << "NAD: constructor (filename=" + filename + ")" << endl;

  _conn = new NAD::NADConnection();
  _conn->setUrl(filename);

  if (init()) {
    _valid = true;
  }
}


NADDirectSource::~NADDirectSource() {
  kstdDebug() << "NAD: destructor\n";
  if (_conn) {
    delete _conn;
  }
}


bool NADDirectSource::init() {
  kstdDebug() << "NAD::init()\n";
  if (_conn->isValid()) {
    _fieldList = _conn->getFields();
    if (!_fieldList.empty()) {
      _fieldList.prepend("INDEX");
    }
  }

  return update() == KstObject::UPDATE;
}

KstObject::UpdateType NADDirectSource::update(int u) {
  if (KstObject::checkUpdateCounter(u)) {
    return lastUpdateResult();
  }

  if (_conn && _conn->updated()) {
    updateNumFramesScalar();
    kstdDebug() << "NAD::update(" << QString::number(u) << ") = UPDATE\n";
    return setLastUpdateResult(KstObject::UPDATE);
  } else {
    kstdDebug() << "NAD::update(" << QString::number(u) << ") = NO_CHANGE\n";
    return setLastUpdateResult(KstObject::NO_CHANGE);
  }
}


// if n > 0, read n FRAMES starting at frame s for field into buffer v
// if n < 0, read 1 SAMPLE from frame s for field into buffer v
int NADDirectSource::readField(double *v, const QString& field, int s, int n) {
  kstdDebug() << "NAD::readField(" << field << ", s=" + QString::number(s) + ", n=" + QString::number(n) + ")" << endl;

  if (field == "INDEX") {
    if (n < 0) {
      v[0] = double(s);
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i);
    }
    return n > 0 ? n : 0;
  }

  if (!_valid || !_conn || !_conn->isValid()) {
    kstdDebug() << "tried to read from an invalid NAD source" << endl;
    kstdDebug() << "plugin is valid? " << _valid << endl;
    return -1;
  }

  QSize sz = _conn->range(field);
  long start = sz.width(), end = sz.height();
  long count = end - start + 1;

  if (s + start > end) {
    kstdDebug() << "Nothing to read: (" << start << "," << end << ") " << s << endl;
    return 0;
  }

  if (n < 0) { // reading less than 0 -> read only one sample!
    int spf = _conn->samplesPerFrame(field);
    double *temp = new double[spf];
    _conn->getData(field, temp, start + s, start + s);
    v[0] = temp[0];
    delete[] temp;
    return 1;
  } else {
    if (s + n > count) { // trying to read past the end
      n = count - s;
    }

//    assert(n != 0);
//    kstdDebug() << "NAD: calling getData()" << endl;
    int ret = _conn->getData(field, v, start + s, start + s + n - 1);
//    kstdDebug() << "NAD::readField(" << field << ", s=" + QString::number(s) + ", n=" + QString::number(n) + ") = " << ret << endl;
    return ret;
  }
}


/** Reads a field from the file.  Data is returned in the
 *  double Array v[].  Will skip according to the parameter, but it may not
 *  be implemented.  If it returns -9999, use the non-skip version instead. */
// if n > 0, read 1 SAMPLE from each of n FRAMES starting at frame s skipping by 'skip' for field into buffer v
// if n < 0, read 1 SAMPLE from frame s for field into buffer v
int NADDirectSource::readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead) {
  kstdDebug() << "NAD::readField(" << field << ", s=" + QString::number(s) + ", n=" + QString::number(n) + ", skip=" + QString::number(skip) + ")" << endl;
  if (lastFrameRead) *lastFrameRead = -1;

  if (field == "INDEX") {
    if (n < 0) {
      v[0] = double(s);
      if (lastFrameRead) *lastFrameRead = s;
      return 1;
    }
    for (int i = 0; i < n; ++i) {
      v[i] = double(s + i*skip);
    }
    if (lastFrameRead) *lastFrameRead = s + (n - 1) * skip;
    return n > 0 ? n : 0;
  }

  if (!_valid || !_conn || !_conn->isValid()) {
    kstdDebug() << "tried to read from an invalid NAD source" << endl;
    kstdDebug() << "plugin is valid? " << _valid << endl;
    return -1;
  }

  QSize sz = _conn->range(field);
  long start = sz.width(), end = sz.height();
  long count = end - start + 1;
  int spf = _conn->samplesPerFrame(field);

  if (s + start > end) {
    kstdDebug() << "Nothing to read: (" << start << "," << end << ") " << s << endl;
    return 0;
  }

  if (n < 0) { // reading less than 0 -> read only one sample!
    double *temp = new double[spf];
    _conn->getData(field, temp, start + s, start + s);
    v[0] = temp[0];
    delete[] temp;
    if (lastFrameRead) *lastFrameRead = s;
    return 1;
  } else {
    if (s + (n-1)*skip >= count) { // trying to read past the end
      // n = ceil((count-s)/skip)
      if ((count - s) % skip == 0) {
        n = (count - s) / skip;
      } else {
        n = (count - s) / skip + 1;
      }

      n = count - s;
    }

    double *tmp = new double[n * spf];
//    kstdDebug() << "NAD: calling getData()" << endl;
    int rc = _conn->getData(field, tmp, start + s, start + s + (n - 1) * skip, skip); // get skipped frames
    int framesRead = rc/spf;
    //kstdDebug() << "readObject rc=" << rc << " from=" << start+s << " to=" << start + s + (n - 1) * skip << endl;

    // extract first sample from each returned frame
    int i = 0;
    while (i < QMAX(n, framesRead)) {
      v[i] = tmp[i * spf];
      ++i;
    }
    delete[] tmp;

//    kstdDebug() << "NAD::readField(" << field << ", s=" + QString::number(s) + ", n=" + QString::number(n) + ", skip=" + QString::number(skip) + ") = " << i << endl;
    if (i > 0 && lastFrameRead) {
      *lastFrameRead = s + (i - 1) * skip;
    }
    return i;
  }
}


bool NADDirectSource::isValidField(const QString& field) const {
  bool valid = _fieldList.contains(field);
  kstdDebug() << "NAD::isValidField(" << field << ") = " << valid << endl;
  return valid;
}


int NADDirectSource::samplesPerFrame(const QString &field) {
  if (!_valid) {
    return 0;
  }

  if (field == "INDEX") {
    kstdDebug() << "NAD::samplesPerFrame(" << field << ") = 1\n";
    return 1;
  }

  int spf = _conn->samplesPerFrame(field);
  kstdDebug() << "NAD::samplesPerFrame(" << field << ") = " << spf << endl;
  return spf;
}


int NADDirectSource::frameCount(const QString& field) const {
  if (!_valid) {
    return 0;
  }

  if (field.isEmpty() || _fieldList.contains(field)) {
    int maxLen = 0;
    for (QStringList::ConstIterator i = _fieldList.begin(); i != _fieldList.end(); ++i) {
//      assert(!(*i).isEmpty());
      QSize sz = _conn->range(*i);
      maxLen = QMAX(sz.height() - sz.width() + 1, maxLen);
    }
    kstdDebug() << "NAD::frameCount(" << field << ") = maxLen = " << maxLen << endl;
    return maxLen;
  }

  QSize sz = _conn->range(field);
  kstdDebug() << "NAD::frameCount(" << field << ") = " << sz.height() - sz.width() + 1 << endl;
  return sz.height() - sz.width() + 1;
}


QString NADDirectSource::fileType() const {
  return "Direct NAD Connection";
}


void NADDirectSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
}


bool NADDirectSource::isEmpty() const {
  kstdDebug() << "NAD::isEmpty()" << endl;

  if (!_valid) {
    return true;
  }

  for (QStringList::ConstIterator i = _fieldList.begin(); i != _fieldList.end(); ++i) {
    QSize sz = _conn->range(*i);
    if (sz.height() - sz.width() + 1 > 0) {
      return false;
    }
  }

  return true;
}


extern "C" {
KstDataSource *create_naddirect(KConfig *cfg, const QString& filename, const QString& type) {
  kstdDebug() << "in create_naddirect:" << filename << endl;

  return new NADDirectSource(cfg, filename, type);
}


QStringList provides_naddirect() {
  kstdDebug() << "in provides_naddirect" << endl;

  QStringList rc;
  rc += "Direct NAD Connection";
  return rc;
}


int understands_naddirect(KConfig *cfg, const QString& filename) {
  Q_UNUSED(cfg)

  kstdDebug() << "in understands_naddirect:" << filename << endl;

  KURL url(filename);
  if (url.protocol().lower() == "nad") {
    return 99;
  } else {
    return 0;
  }
}


QStringList fieldList_naddirect(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
  Q_UNUSED(cfg)

  kstdDebug() << "in fieldList_naddirect:" << filename << endl;

  if (!type.isEmpty() && !provides_naddirect().contains(type)) {
    return QStringList();
  }

  NAD::NADConnection conn;
  conn.setUrl(filename);
  if (!conn.isValid()) {
    return QStringList();
  }

  if (complete) {
    *complete = true;
  }

  if (typeSuggestion) {
    *typeSuggestion = "Direct NAD Connection";
  }

  QStringList rc = conn.getFields();
  if (!rc.empty()) {
    rc.prepend("INDEX");
  }

  return rc;
}


QWidget* widget_naddirect(const QString& filename) {
  Q_UNUSED(filename)
  // Return 0L, or a widget that inherits from KstDataSourceConfigWidget
  return 0L;
}

}

KST_KEY_DATASOURCE_PLUGIN(naddirect)

// vim: ts=2 sw=2 et
