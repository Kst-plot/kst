/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "measuretime.h"

#include "debug.h"

#include <iostream>
#include <time.h>


#ifdef Q_OS_WIN
#include <windows.h>
static qint64 largeIntToInt64(const LARGE_INTEGER& i)
{
  return ((qint64)i.HighPart) << 32 | i.LowPart;
}
static double readFrequency()
{
  LARGE_INTEGER largeInt;
  if (!QueryPerformanceFrequency(&largeInt))
    return 0;
  return 1.0 / largeIntToInt64(largeInt);
}
static double frequency = readFrequency();
#endif


MeasureTime::MeasureTime(const QString& n) :
    started(0),
    name(n)
{
  restart();
}


MeasureTime::~MeasureTime()
{
  print();
}


double MeasureTime::getTime() const
{
#ifdef Q_OS_WIN

  LARGE_INTEGER largeInt;
  if (!QueryPerformanceCounter(&largeInt))
    return 0;
  return  largeIntToInt64(largeInt) * frequency;

#else

#if defined(Q_OS_MAC)
  // TODO
  return 0;
#else
  timespec t;
#if defined(CLOCK_PROCESS_CPUTIME_ID)
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
#elif defined(CLOCK_PROF)
  clock_gettime(CLOCK_PROF, &t);
#else
  clock_gettime(CLOCK_REALTIME, &t);
#endif
  return 1.0 * t.tv_sec + 1e-9 * t.tv_nsec;
#endif

#endif

}


void MeasureTime::restart()
{
  started = getTime();
  interval = 0;
}


void MeasureTime::measure()
{
  double now = getTime();
  interval += now - started;
  started = now;
}


void MeasureTime::print()
{
  measure();
  qWarning(qPrintable(QString("MeasureTime in %1: %2 sec").arg(name ).arg(interval)));
  Kst::Debug::self()->log(QString("Timing: %2 sec, Scope: %1").arg(name).arg(interval), Kst::Debug::DebugLog);
}


 // vim: ts=2 sw=2 et
