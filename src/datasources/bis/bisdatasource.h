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


#ifndef BISDATASOURCE_H
#define BISDATASOURCE_H

#include <datasource.h>
#include <dataplugin.h>

#include "bis.h"

class DataInterfaceBISMatrix;
class DataInterfaceBISVector;

class BISSource : public Kst::DataSource {
  Q_OBJECT

  public:
    BISSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~BISSource();

    friend class DataInterfaceBISMatrix;
    friend class DataInterfaceBISVector;

    bool init();
    virtual void reset();

    Kst::Object::UpdateType internalDataSourceUpdate();

 
    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    class Config;

    virtual bool isImageStream(QString field) {Q_UNUSED(field) return true;}

  private:
    mutable Config *_config;

    DataInterfaceBISMatrix* im;
    DataInterfaceBISVector* iv;

    QHash<QString, int> _matrixHash;
    QStringList _vectorList;

    BISfile *_bisfile;
    
    int _nframes;

    BISimage _bisImage;
};


class BISSourcePlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
    Q_PLUGIN_METADATA(IID "com.kst.DataSourcePluginInterface/2.0")
  public:
    virtual ~BISSourcePlugin() {}

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
