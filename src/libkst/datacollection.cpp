/***************************************************************************
                            datacollection.cpp
                             -------------------
    begin                : June 12, 2003
    copyright            : (C) 2003 The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
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

static QMutex bigLock;

void *realloc(void *ptr, size_t size) {
#ifdef __linux__
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
#ifdef __linux__
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
  Q_ASSERT(_self);
  return _self;
}


void Data::replaceSelf(Data *newInstance) {
  cleanup();
  _self = newInstance;
}


Data::Data() {
  qAddPostRoutine(Data::cleanup);
}


Data::~Data() {
}


void Data::removeCurveFromPlots(Relation *c) {
  Q_UNUSED(c)
  // meaningless in no GUI: no plots!
}

QList<PlotItemInterface*> Data::plotList() const {
  return QList<PlotItemInterface*>();
}


int Data::rows() const {
  return -1;
}


int Data::columns() const {
  return -1;
}

}

// vim: ts=2 sw=2 et
