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

#ifndef KST_DMCSOURCE_H
#define KST_DMCSOURCE_H

#include "datasource.h"
#include "dmcdata.h"
#include "dmcobj.h"

#include "datasource.h"
#include "dataplugin.h"



class DataInterfaceDmcScalar;
class DataInterfaceDmcString;
class DataInterfaceDmcVector;
class DataInterfaceDmcMatrix;

class DmcSource : public Kst::DataSource {
  public:
    DmcSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement &element);

    ~DmcSource();

    bool initFile();

    Kst::Object::UpdateType internalDataSourceUpdate();

    virtual const QString& typeString() const;

    static const QString dmcTypeKey();


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

    // QMap<QString, QString> _metaData;

    // TODO remove friend
    QStringList _scalarList;
    QStringList _fieldList;
    QStringList _matrixList;
    QStringList _stringList;


    friend class DataInterfaceDmcScalar;
    friend class DataInterfaceDmcString;
    friend class DataInterfaceDmcVector;
    friend class DataInterfaceDmcMatrix;
    DataInterfaceDmcScalar* is;
    DataInterfaceDmcString* it;
    DataInterfaceDmcVector* iv;
    DataInterfaceDmcMatrix* im;


    // from old dmc code
    Kst::SharedPtr<DMC::Object> _dmcObject;
    QString _filename;

    void update();

    int readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead) ;
    bool isValidField(const QString& field) const;
    bool supportsTimeConversions() const;
    int sampleForTime(const QDateTime& time, bool *ok);
    QDateTime timeForSample(int sample, bool *ok);
    int sampleForTime(double ms, bool *ok);
    double relativeTimeForSample(int sample, bool *ok);
};



#endif

