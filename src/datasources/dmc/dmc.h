/***************************************************************************
                 dmc.h  -  data source plugin for planck DMC
                             -------------------
    begin                : Wed July 4 2007
    copyright            : (C) 2007 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DMC_H
#define DMC_H

#include <kstdatasource.h>
#include "dmcdata.h"
#include "dmcobj.h"


class DMCSource : public KstDataSource {
  public:
    DMCSource(KConfig *cfg, const QString& filename);

    ~DMCSource();

    KstObject::UpdateType update(int = -1);

    int readField(double *v, const QString &field, int s, int n);

    int readField(double *v, const QString &field, int s, int n, int skip, int *lastFrameRead = 0L);

    bool isValidField(const QString &field) const;

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString::null) const;

    QString fileType() const;

    void save(QTextStream &ts, const QString& indent = QString::null);

    bool isEmpty() const;

    bool supportsTimeConversions() const;

    int sampleForTime(const KST::ExtDateTime& time, bool *ok = 0L);

    int sampleForTime(double ms, bool *ok = 0L);

    KST::ExtDateTime timeForSample(int sample, bool *ok = 0L);

    // in (ms)
    double relativeTimeForSample(int sample, bool *ok = 0L);

    bool reset();

  private:
    KstSharedPtr<DMC::Object> _dmcObject;
    QString _filename;
};


#endif

