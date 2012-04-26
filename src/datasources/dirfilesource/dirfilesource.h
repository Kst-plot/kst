/***************************************************************************
                dirfile.h  -  data source plugin for dirfiles
                             -------------------
    begin                : Tue Oct 21 2003
    copyright            : (C) 2003 The University of Toronto
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DIRFILE_H
#define DIRFILE_H

#include <datasource.h>
#include <dataplugin.h>
#include <getdata/dirfile.h>

using namespace GetData;

class QFileSystemWatcher;
class DataInterfaceDirFileVector;
class DataInterfaceDirFileScalar;
class DataInterfaceDirFileString;

class DirFileSource : public Kst::DataSource {
  Q_OBJECT

  public:
    DirFileSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~DirFileSource();

    bool init();

    virtual UpdateType internalDataSourceUpdate();

    int readField(double *v, const QString &field, int s, int n);

//     int writeField(const double *v, const QString &field, int s, int n);

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString()) const;

    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    bool isEmpty() const;

    virtual void reset();

    class Config;

    int readScalar(double &S, const QString& scalar);

    int readString(QString &S, const QString& string);

    virtual QStringList fieldScalars(const QString& field);

    virtual int readFieldScalars(QList<double> &v, const QString& field, bool init);

    virtual QStringList fieldStrings(const QString& field);

    virtual int readFieldStrings(QStringList &v, const QString& field, bool init);

    virtual const QString& typeString() const;
    //static const QString staticTypeString;
    //static const QString staticTypeTag;



  private:
    QString _directoryName;
    Dirfile *_dirfile;

    QStringList _scalarList;
    QStringList _stringList;
    QStringList _fieldList;

    int _frameCount;
    mutable Config *_config;

    DataInterfaceDirFileVector* iv;
    DataInterfaceDirFileScalar* ix;
    DataInterfaceDirFileString* is;

    friend class DataInterfaceDirFileVector;
    friend class DataInterfaceDirFileScalar;
    friend class DataInterfaceDirFileString;
    bool _resetNeeded;
};


class DirFilePlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
  public:
    virtual ~DirFilePlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual bool hasConfigWidget() const { return false; }

    virtual Kst::DataSource *create(Kst::ObjectStore *store,
                                  QSettings *cfg,
                                  const QString &filename,
                                  const QString &type,
                                  const QDomElement &element) const;

    virtual QStringList matrixList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual QStringList fieldList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual QStringList scalarList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual QStringList stringList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const;

    virtual int understands(QSettings *cfg, const QString& filename) const;

    static QString getDirectory(QString filepath);

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const;

    virtual QStringList provides() const;

    virtual Kst::DataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const;
};


#endif
// vim: ts=2 sw=2 et
