/***************************************************************************
                  nadsource.h  - NAD data source plugin
                             -------------------
    begin                : May 02 2006
    copyright            : (C) 2006 Staikos Computing Services Inc.
    email                : info@staikos.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NADSOURCE_H
#define NADSOURCE_H

#include <kstdatasource.h>

#include <nad.h>


class NADSource : public KstDataSource {
  public:
    NADSource(KConfig *cfg, const QString& filename, const QString& type);

    ~NADSource();

    KstObject::UpdateType update(int = -1);

    int readField(double *v, const QString &field, int s, int n);
    int readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead);

    bool isValidField(const QString &field) const;

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString::null) const;

    QString fileType() const;

    void save(QTextStream &ts, const QString& indent = QString::null);

    bool isEmpty() const;

  private:
    bool init();

    QString _filename;
    bool _firstTime;
    NAD::NADCache *_cache;
    NAD::NADConnection *_serverConn;
    NAD::NADCacheAgentConnection *_cacheConn;
};


#endif
// vim: ts=2 sw=2 et
