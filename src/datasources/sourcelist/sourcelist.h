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


#ifndef SOURCELIST_H
#define SOURCELIST_H

#include <datasource.h>
#include <dataplugin.h>

class DataInterfaceSourceListVector;
class DataInterfaceSourceListScalar;
class DataInterfaceSourceListString;
class DataInterfaceSourceListMatrix;

using namespace Kst;

class SourceListSource : public Kst::DataSource {
  Q_OBJECT

  public:
    SourceListSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~SourceListSource();

    bool init();
    virtual void reset();

    Kst::Object::UpdateType internalDataSourceUpdate();

 
    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    class Config;

    int samplesPerFrame(const QString &field);

    int readField(const QString& field, DataVector::ReadInfo& p);

  private:
    mutable Config *_config;

    int _frameCount;

    QStringList _scalarList;
    QStringList _stringList;
    QStringList _fieldList;
    QStringList _matrixList;

    DataInterfaceSourceListVector* iv;
    //DataInterfaceSourceListScalar* ix;
    //DataInterfaceSourceListString* is;
    //DataInterfaceSourceListMatrix* im;

    friend class DataInterfaceSourceListVector;
    //friend class DataInterfaceSourceListScalar;
    //friend class DataInterfaceSourceListString;
    //friend class DataInterfaceSourceListMatrix;

    DataSourceList _sources;
    QList<int> _sizeList;
};


class SourceListPlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
    Q_PLUGIN_METADATA(IID "com.kst.DataSourcePluginInterface/2.0")
  public:
    virtual ~SourceListPlugin() {}

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

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const;

    virtual QStringList provides() const;

    virtual Kst::DataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const;
};


#endif
// vim: ts=2 sw=2 et
