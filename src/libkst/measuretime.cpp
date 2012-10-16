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

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <time.h>
#endif

#include <iostream>
 
MeasureTime::MeasureTime(const QString& n) :
    started(0),
    interval(0),
    name(n),
    other_interval(0)
{
  setFrequency();
  restart();
}


void MeasureTime::setFrequency()
{
#ifdef Q_OS_WIN
  LARGE_INTEGER proc_freq;
  QueryPerformanceFrequency(&proc_freq);
  frequency =  1.0 / proc_freq.QuadPart;
#endif
}


MeasureTime::MeasureTime(MeasureTime& rhs) :
      started(0),
      interval(0),
      frequency(rhs.frequency),
      other_interval(&rhs.interval)
{
  restart();
}


MeasureTime::~MeasureTime()
{
  if (other_interval) {
    measure();
    *other_interval += interval;
  } else {
    print();
  }
}


double MeasureTime::getTime() const
{
#ifdef Q_OS_WIN

  LARGE_INTEGER st;
  QueryPerformanceCounter(&st);
  return st.QuadPart * frequency;

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
  //qDebug() << "MeasureTime in " << name << ": " << interval << " seconds\n";
  Kst::Debug::self()->log(QString("Timing: %2 sec, Scope: %1").arg(name).arg(interval), Kst::Debug::DebugLog);
}


 // vim: ts=2 sw=2 et
