/***************************************************************************
                  wmap.h  -  data source plugin template
                             -------------------
    begin                : Feb 7 17 2007
    copyright            : (C) 2007 The University of British Columbia
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

#ifndef WMAP_H
#define WMAP_H

#include <kstdatasource.h>
#include <fitsio.h>

typedef struct {
    int table;
    int column;
    int entry;
    int entries;
    int numFrames;
    int numSamplesPerFrame;
} field;

class WMAPSource : public KstDataSource {
  public:
    WMAPSource(KConfig *cfg, const QString& filename, const QString& type);
   ~WMAPSource();

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
    void addToMetadata( fitsfile *ffits, int &iStatus );
    void addToMetadata( fitsfile *ffits, const int iNumCols, int &iStatus );
    void addToFieldList( fitsfile *ffits, const int iNumCols, const long lNumRows, const long lNumBaseRows, int &iStatus );

    QDict<field> _fields;
    double _dTimeZero;
    double _dTimeDelta;
    bool _bHasTime;
    bool _first;
    int _numFrames;
    int _numCols;
};


#endif
// vim: ts=2 sw=2 et
