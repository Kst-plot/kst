/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "measuretime.h"

 
#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <ctime>
#endif

 
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
#ifdef Q_CC_MSVC
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


void MeasureTime::restart()
{
#ifdef Q_CC_MSVC
  LARGE_INTEGER st;
  QueryPerformanceCounter(&st);
  started = st.QuadPart * frequency;
#else
  timespec t;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
  started = 1.0 * t.tv_sec + 1e-9 * t.tv_nsec;  
#endif
  interval = 0;
}


void MeasureTime::measure()
{
#ifdef Q_CC_MSVC
  LARGE_INTEGER st;
  QueryPerformanceCounter(&st);
  double now = st.QuadPart * frequency;
#else
  timespec t;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
  double now = 1.0 * t.tv_sec + 1e-9 * t.tv_nsec;
#endif
  interval += now - started;
  started = now;
}


void MeasureTime::print()
{
  measure();
  printf("%s: %f sec \n", qPrintable(name), interval);
}


 // vim: ts=2 sw=2 et
