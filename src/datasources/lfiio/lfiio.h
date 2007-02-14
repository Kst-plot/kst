/***************************************************************************
                  lfiio.h  -  data source plugin template
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of British Columbia
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

#ifndef LFIIO_H
#define LFIIO_H

#include <kstdatasource.h>


class LFIIOSource : public KstDataSource {
  public:
    LFIIOSource(KConfig *cfg, const QString& filename, const QString& type);

    ~LFIIOSource();
    bool initFile();
    KstObject::UpdateType update(int = -1);
    int readField(double *v, const QString &field, int s, int n);
    bool isValidField(const QString &field) const;
    int samplesPerFrame(const QString &field);
    int frameCount(const QString& field = QString::null) const;
    QString fileType() const;
    void save(QTextStream &ts, const QString& indent = QString::null);
    bool isEmpty() const;

    bool reset();

  private:
    bool getColNumber(const QString& field, int* piColNumber) const;

    double _dTimeZero;
    double _dTimeDelta;
    bool _bHasTime;
    bool _first;
    int _numFrames;
    int _numCols;
};


#endif
// vim: ts=2 sw=2 et
