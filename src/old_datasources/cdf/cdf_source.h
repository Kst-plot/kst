/***************************************************************************
                       cdf.h  -  CDF data source
                             -------------------
    begin                : 17/06/2004
    copyright            : (C) 2004 Nicolas Brisset <nicodev@users.sourceforge.net>
    email                : kst@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CDF_H
#define CDF_H

#include <kstdatasource.h>
#include <cdf.h>


class CdfSource : public KstDataSource {
  public:
    CdfSource(KConfig *cfg, const QString& filename, const QString& type);

    ~CdfSource();

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
    QMap<QString,int> _frameCounts;
    int _maxFrameCount;
};


#endif
// vim: ts=2 sw=2 et
