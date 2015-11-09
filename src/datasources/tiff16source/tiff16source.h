/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2005 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef TIFF16SOURCE_H
#define TIFF16SOURCE_H

#include <datasource.h>
#include <dataplugin.h>

class DataInterfaceTiff16Vector;
class DataInterfaceTiff16Matrix;
  
class Tiff16Source : public Kst::DataSource 
{
  Q_OBJECT

  public:
    Tiff16Source(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~Tiff16Source();

    bool init();
    virtual void reset();

    Kst::Object::UpdateType internalDataSourceUpdate();

    bool isEmpty() const;
    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    virtual const QString& typeString() const;

    class Config;

    //int readScalar(double &S, const QString& scalar);
    //int readString(QString &S, const QString& string);

  private:
    unsigned short *_z;
    int _width;
    int _height;
    mutable Config *_config;

    DataInterfaceTiff16Vector* iv;
    DataInterfaceTiff16Matrix* im;
};


class Tiff16SourcePlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
    Q_PLUGIN_METADATA(IID "com.kst.DataSourcePluginInterface/2.0")
  public:
    virtual ~Tiff16SourcePlugin() {}

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
