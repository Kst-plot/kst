/***************************************************************************
                netcdf_source.h  -  netCDF data source reader
                             -------------------
    begin                : 28/01/2005
    copyright            : (C) 2004 Nicolas Brisset <nicodev@users.sourceforge.net>
    email                : kst@kde.org
    modified             : 03/16/05 by K. Scott
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NETCDF_H
#define NETCDF_H

#include <kstdatasource.h>
#include <netcdf.h>
#include <netcdfcpp.h>


class NetcdfSource : public KstDataSource {
  public:
    NetcdfSource(KConfig *cfg, const QString& filename, const QString& type);

    ~NetcdfSource();

    bool initFile();

    KstObject::UpdateType update(int = -1);

    int readField(double *v, const QString& field, int s, int n);

    bool isValidField(const QString& field) const;

    int samplesPerFrame(const QString& field);

    int frameCount(const QString& field = QString::null) const;

    QString fileType() const;

    void save(QTextStream &ts, const QString& indent = QString::null);

    bool isEmpty() const;

    bool reset();

  private:
    QMap<QString,long> _frameCounts;
    long _maxFrameCount;
    NcFile *_ncfile;
};


#endif

