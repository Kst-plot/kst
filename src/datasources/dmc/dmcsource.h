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


#include "datasource.h"
#include "dataplugin.h"

#include <netcdf.h>
#include <netcdfcpp.h>


class DataInterfaceNetCdfScalar;
class DataInterfaceNetCdfString;
class DataInterfaceNetCdfVector;
class DataInterfaceNetCdfMatrix;

class DmcSource : public Kst::DataSource {
  public:
    DmcSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement &element);

    ~DmcSource();

    bool initFile();

    Kst::Object::UpdateType internalDataSourceUpdate();


    virtual const QString& typeString() const;


    int readScalar(double *v, const QString& field);

    int readString(QString *stringValue, const QString& stringName);

    int readField(double *v, const QString& field, int s, int n);

    int readMatrix(double *v, const QString& field);

    int samplesPerFrame(const QString& field);

    int frameCount(const QString& field = QString()) const;

    QString fileType() const;

    void save(QTextStream &ts, const QString& indent = QString());

    bool isEmpty() const;

    void  reset();

  private:
    QMap<QString, int> _frameCounts;

    int _maxFrameCount;
    NcFile *_ncfile;    

    // QMap<QString, QString> _metaData;

    // TODO remove friend
    QStringList _scalarList;
    QStringList _fieldList;
    QStringList _matrixList;
    QStringList _stringList;


    friend class DataInterfaceNetCdfScalar;
    friend class DataInterfaceNetCdfString;
    friend class DataInterfaceNetCdfVector;
    friend class DataInterfaceNetCdfMatrix;
    DataInterfaceNetCdfScalar* is;
    DataInterfaceNetCdfString* it;
    DataInterfaceNetCdfVector* iv;
    DataInterfaceNetCdfMatrix* im;
};



#endif

