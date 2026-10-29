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

#include <QFile>

#if defined(Q_OS_WIN)
#include <windows.h>
#elif defined(Q_OS_MACOS)
#include <mach/mach.h>
#elif defined(Q_OS_FREEBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>
#endif

namespace Kst {

#if defined(Q_OS_LINUX)
// Reads a cgroup v2 memory file (memory.max, memory.current).
// Returns -1 if the file is missing or contains "max" (no limit).
static double readCgroupValue(const char *name) {
  QFile f(QString("/sys/fs/cgroup/") + name);
  if (!f.open(QIODevice::ReadOnly)) {
    return -1.0;
  }
  bool ok;
  double v = f.readLine().trimmed().toDouble(&ok);
  return ok ? v : -1.0;
}

static double linuxAvailableMemory() {
  // MemAvailable (kernel >= 3.14) is the kernel's own estimate of how much
  // memory can be allocated without swapping, accounting for reclaimable
  // page cache and slab.  Fall back to MemFree + Buffers + Cached.
  QFile f("/proc/meminfo");
  if (!f.open(QIODevice::ReadOnly)) {
    return -1.0;
  }
  double mem_available = -1, mem_free = 0, buffers = 0, cached = 0;
  for (QByteArray line = f.readLine(); !line.isEmpty(); line = f.readLine()) {
    QList<QByteArray> fields = line.simplified().split(' ');
    if (fields.size() < 2) {
      continue;
    }
    double bytes = fields[1].toDouble() * 1024.0; // values are in kB
    if (fields[0] == "MemAvailable:") {
      mem_available = bytes;
      break;
    } else if (fields[0] == "MemFree:") {
      mem_free = bytes;
    } else if (fields[0] == "Buffers:") {
      buffers = bytes;
    } else if (fields[0] == "Cached:") {
      cached = bytes;
    }
  }
  double available = (mem_available >= 0) ? mem_available : mem_free + buffers + cached;

  // Respect a cgroup v2 memory limit (containers, systemd slices, flatpak...)
  double limit = readCgroupValue("memory.max");
  double used = readCgroupValue("memory.current");
  if (limit > 0 && used >= 0) {
    available = qMin(available, limit - used);
  }
  return available;
}
#endif


double Data::AvailableMemory(bool log) {
  const double one_GB = 1024.0*1024.0*1024.0;
  double available_memory = -1;

#if defined(Q_OS_WIN)
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if (GlobalMemoryStatusEx(&statex)) {
    // ullAvailVirtual only matters for 32 bit builds.
    available_memory = double(qMin(statex.ullAvailPhys, statex.ullAvailVirtual));
  }
#elif defined(Q_OS_LINUX)
  available_memory = linuxAvailableMemory();
#elif defined(Q_OS_MACOS)
  // Free + inactive pages can be handed out without paging (same estimate
  // as psutil's "available").
  vm_statistics64_data_t vm;
  mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
  vm_size_t page_size;
  mach_port_t host = mach_host_self();
  if (host_page_size(host, &page_size) == KERN_SUCCESS &&
      host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm, &count) == KERN_SUCCESS) {
    available_memory = (double(vm.free_count) + double(vm.inactive_count)) * double(page_size);
  }
  mach_port_deallocate(mach_task_self(), host);
#elif defined(Q_OS_FREEBSD)
  u_int free_pages = 0, inactive_pages = 0;
  size_t len = sizeof(u_int);
  if (sysctlbyname("vm.stats.vm.v_free_count", &free_pages, &len, NULL, 0) == 0) {
    len = sizeof(u_int);
    sysctlbyname("vm.stats.vm.v_inactive_count", &inactive_pages, &len, NULL, 0);
    available_memory = double(free_pages + inactive_pages) * double(sysconf(_SC_PAGESIZE));
  }
#endif

  if (available_memory < 0) {
    // Unknown OS or the query failed: assume a modest amount.
    available_memory = 4 * one_GB;
  }

  if (log) {
    Debug::self()->log(QString("Available memory: %1 GB").arg(available_memory/one_GB));
  }
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
