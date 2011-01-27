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


void MeasureTime::restart()
{
#ifdef Q_OS_WIN
  LARGE_INTEGER st;
  QueryPerformanceCounter(&st);
  started = st.QuadPart * frequency;
#else
#ifndef Q_OS_MAC
  timespec t;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
  started = 1.0 * t.tv_sec + 1e-9 * t.tv_nsec;  
#endif
#endif
  interval = 0;
}


void MeasureTime::measure()
{
  double now = 0;
#ifdef Q_OS_WIN
  LARGE_INTEGER st;
  QueryPerformanceCounter(&st);
  now = st.QuadPart * frequency;
#else
#ifndef Q_OS_MAC  
  timespec t;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
  now = 1.0 * t.tv_sec + 1e-9 * t.tv_nsec;
#endif
#endif
  interval += now - started;
  started = now;
}


void MeasureTime::print()
{
  measure();
  //std::cout << qPrintable(name) << ": " << interval << " seconds\n";
  Kst::Debug::self()->log(QString("Timing: %2 sec, Scope: %1").arg(name).arg(interval));
}


 // vim: ts=2 sw=2 et
