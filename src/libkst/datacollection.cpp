/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2003 The University of Toronto                        *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>

#include <stdlib.h>
#include <qapplication.h>

#include "datacollection.h"

#include "sysinfo.h"
#include "psversion.h"

namespace Kst {

/** The list of data sources (files) */
DataSourceList dataSourceList;

/** The list of vectors that are being read */
VectorCollection vectorList;

/** The list of matrices that are being read */
MatrixCollection matrixList;

/** The list of Scalars which have been generated */
ScalarCollection scalarList;

/** The list of Strings */
KstStringCollection stringList;

static QMutex bigLock;

void *realloc(void *ptr, size_t size) {
#ifdef HAVE_LINUX
  QMutexLocker ml(&bigLock);
  meminfo();
  unsigned long bFree = S(kb_main_free + kb_main_buffers + kb_main_cached);
  if (size > bFree) {
    const unsigned long sz = size;
    qDebug("Tried to allocate too much memory! (Wanted %lu, had %lu)", sz, bFree);
    return 0L;
  }
#endif
  return ::realloc(ptr, size);
}

void *malloc(size_t size) {
#ifdef HAVE_LINUX
  QMutexLocker ml(&bigLock);
  meminfo();
  unsigned long bFree = S(kb_main_free + kb_main_buffers + kb_main_cached);
  if (size > bFree) {
    const unsigned long sz = size;
    qDebug("Tried to allocate too much memory! (Wanted %lu, had %lu)", sz, bFree);
    return 0L;
  }
#endif
  return ::malloc(size);
}


Data *Data::_self = 0L;
void Data::cleanup() {
    delete _self;
    _self = 0;
}


Data *Data::self() {
  if (!_self) {
    _self = new Data;
    qAddPostRoutine(Data::cleanup);
  }
  return _self;
}


void Data::replaceSelf(Data *newInstance) {
  delete _self;
  _self = 0L;
  _self = newInstance;
}


Data::Data() {
}


Data::~Data() {
}


bool Data::vectorTagNameNotUniqueInternal(const QString& tag) {
  /* verify that the tag name is not empty */
  if (tag.trimmed().isEmpty()) {
      return true;
  }

  /* verify that the tag name is not used by a data object */
  vectorList.lock().readLock();
  bool vc = vectorList.tagExists(tag);
  vectorList.lock().unlock();
  if (!vc) {
    scalarList.lock().readLock();
    vc = scalarList.tagExists(tag);
    scalarList.lock().unlock();
  }
  return vc;
}


bool Data::matrixTagNameNotUniqueInternal(const QString& tag) {
  /* verify that the tag name is not empty */
  if (tag.trimmed().isEmpty()) {
    return true;
  }

  /* verify that the tag name is not used by a data object */
  KstReadLocker ml(&matrixList.lock());
  KstReadLocker ml2(&scalarList.lock());
  if (matrixList.tagExists(tag) || scalarList.tagExists(tag)) {
    return true;
  }
  return false;  
}


bool Data::tagNameNotUnique(const QString& tag, bool warn, void *p) {
  Q_UNUSED(p)
  return dataTagNameNotUnique(tag, warn) || vectorTagNameNotUnique(tag, warn);
}


bool Data::dataTagNameNotUnique(const QString& tag, bool warn, void *parent) {
  Q_UNUSED(tag)
  Q_UNUSED(warn)
  Q_UNUSED(parent)
  return false;
}


bool Data::vectorTagNameNotUnique(const QString& tag, bool warn, void *p) {
  Q_UNUSED(p)
  Q_UNUSED(warn)
  /* verify that the tag name is not empty */
  if (tag.trimmed().isEmpty()) {
      return true;
  }

  /* verify that the tag name is not used by a data object */
  KstReadLocker ml(&vectorList.lock());
  KstReadLocker ml2(&scalarList.lock());
  if (vectorList.tagExists(tag) || scalarList.tagExists(tag)) {
      return true;
  }

  return false;
}


bool Data::matrixTagNameNotUnique(const QString& tag, bool warn, void *p) {
  Q_UNUSED(p)
  Q_UNUSED(warn)
  /* verify that the tag name is not empty */
  if (tag.trimmed().isEmpty()) {
    return true;
  }

  /* verify that the tag name is not used by a data object */
  KstReadLocker ml(&matrixList.lock());
  KstReadLocker ml2(&scalarList.lock());
  if (matrixList.tagExists(tag) || scalarList.tagExists(tag)) {
    return true;
  }
  return false;
}


bool Data::dataSourceTagNameNotUnique(const QString& tag, bool warn, void *p) {
  Q_UNUSED(p)
  Q_UNUSED(warn)
  /* verify that the tag name is not empty */
  if (tag.trimmed().isEmpty()) {
    return true;
  }

  /* verify that the tag name is not used by a data source */
  KstReadLocker l(&dataSourceList.lock());
  if (dataSourceList.findTag(tag) != dataSourceList.end()) {
    return true;
  }
  return false;
}


QStringList Data::plotList(const QString& window) {
  Q_UNUSED(window)
  return QStringList();
}


void Data::removeCurveFromPlots(KstRelation *c) {
  Q_UNUSED(c)
  // meaningless in no GUI: no plots!
}

bool Data::viewObjectNameNotUnique(const QString& tag) {
  Q_UNUSED(tag)
  // meaningless in no GUI: no view objects!
  return false;
}

int Data::vectorToFile(Kst::VectorPtr v, QFile *f) {
  // FIXME: implement me (non-gui)
  return 0;
}


int Data::vectorsToFile(const Kst::VectorList& l, QFile *f, bool interpolate) {
  // FIXME: implement me (non-gui)
  return 0;
}


int Data::columns(const QString& window) {
  Q_UNUSED(window)
  return 0;
}


void Data::newWindow(QWidget *dialogParent) {
  Q_UNUSED(dialogParent)
}


QStringList Data::windowList() {
  return QStringList();
}


QString Data::currentWindow() {
  return QString::null;
}

}
// vim: ts=2 sw=2 et
