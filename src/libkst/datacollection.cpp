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

#include "debug.h"
#include "sysinfo.h"
#include "psversion.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace Kst {

static QMutex bigLock;

void *realloc(void *ptr, size_t size) {
  return ::realloc(ptr, size);
}

void *malloc(size_t size) {
  return ::malloc(size);
}

double Data::AvailableMemory() {
  double one_GB = 1024.0*1024.0*1024.0;
  double available_memory = 0;
  
#ifdef Q_OS_WIN
  // (http://msdn.microsoft.com/en-us/library/aa366589)
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  GlobalMemoryStatusEx(&statex);
  available_memory = statex.ullAvailPhys;
#elif defined Q_OS_LINUX
  QMutexLocker ml(&bigLock);
  meminfo();
  available_memory = double(S(kb_main_free + kb_main_cached)) - 30.0*1024.0*1024.0; // 30MB margin
#else
  // TODO other OSs
  // or assume 32-bit on a big system
  available_memory = 4 * one_GB;
#endif
  Debug::self()->log(QString("Available memory: %1 GB").arg(available_memory/one_GB));
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
