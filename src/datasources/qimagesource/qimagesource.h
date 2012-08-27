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


#ifndef QIMAGESOURCE_H
#define QIMAGESOURCE_H

#include <datasource.h>
#include <dataplugin.h>

class DataInterfaceQImageVector;
class DataInterfaceQImageMatrix;
  
class QImageSource : public Kst::DataSource 
{
  Q_OBJECT

  public:
    QImageSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~QImageSource();

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
    QImage _image;
    mutable Config *_config;

    DataInterfaceQImageVector* iv;
    DataInterfaceQImageMatrix* im;
};


class QImageSourcePlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
#ifdef QT5
    Q_PLUGIN_METADATA(IID "com.kst.DataObjectPluginInterface/2.0" FILE "pluginkey.json")
#endif
  public:
    virtual ~QImageSourcePlugin() {}

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
