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

#ifndef KST_MEASURE_TIME
#define KST_MEASURE_TIME

#include "kst_export.h"

#include <QString>


class KSTCORE_EXPORT MeasureTime
{
  public:
    MeasureTime(const QString& name);
    ~MeasureTime();

    void restart();
    
    /// print interval to console
    void print();

    double getTime() const;

  private:
    double started;
    double interval;
    QString name;

    /// Increment interval by interval since last call/restart().
    void measure();
};

#define TIME_IN_SCOPE(x) MeasureTime x(QString("%1 at %2, line %3, time in scope").arg(#x).arg(__FILE__).arg(__LINE__))

#endif

// vim: ts=2 sw=2 et
