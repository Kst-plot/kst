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
#include "datacollection.h"

#include <config.h>

#include <stdlib.h>
#include <qapplication.h>


#include "sysinfo.h"
#include "psversion.h"

namespace Kst {

static QMutex bigLock;

void *realloc(void *ptr, size_t size) {
  return ::realloc(ptr, size);
}

void *malloc(size_t size) {
  return ::malloc(size);
}

unsigned long Data::AvailableMemory() {
  unsigned long available_memory = 1024*1024*1024;
  // FIXME: under windows or mac, this is totally wrong.
  // Under windows, try GlobalMemoryStatusEx
  // (http://msdn.microsoft.com/en-us/library/aa366589)
  // but, I don't have access to a windows devel environment...

#ifdef __linux__
  QMutexLocker ml(&bigLock);
  meminfo();
  available_memory = S(kb_main_free + kb_main_cached) - 30*1024*1024; // 30MB margin
#endif
  return available_memory;
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
