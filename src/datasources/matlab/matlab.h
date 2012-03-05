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


#ifndef MATLAB_DATASOURCE_H
#define MATLAB_DATASOURCE_H

#include <datasource.h>
#include <dataplugin.h>

#include <matio.h>

class DataInterfaceMatlabScalar;
class DataInterfaceMatlabString;
class DataInterfaceMatlabVector;
class DataInterfaceMatlabMatrix;

class MatlabSource : public Kst::DataSource {
  Q_OBJECT

  public:
    MatlabSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~MatlabSource();

    bool init();
    virtual void reset();

    Kst::Object::UpdateType internalDataSourceUpdate();

    int readScalar(double *v, const QString& field);

    int readString(QString *stringValue, const QString& stringName);

    int readField(double *v, const QString& field, int s, int n);

    int readMatrix(double *v, const QString& field);

    int samplesPerFrame(const QString& field);
    int frameCount(const QString& field = QString()) const;

    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    class Config;


  private:
    QMap<QString, int> _frameCounts;
    int _maxFrameCount;

    // Matio file object
    mat_t *_matfile;
    mutable Config *_config;

    // Primitive lists
    QMap<QString, QString> _strings;
    QStringList _scalarList;
    QStringList _fieldList;
    QStringList _matrixList;

    friend class DataInterfaceMatlabScalar;
    friend class DataInterfaceMatlabString;
    friend class DataInterfaceMatlabVector;
    friend class DataInterfaceMatlabMatrix;
    DataInterfaceMatlabScalar* is;
    DataInterfaceMatlabString* it;
    DataInterfaceMatlabVector* iv;
    DataInterfaceMatlabMatrix* im;

};


class MatlabSourcePlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
  public:
    virtual ~MatlabSourcePlugin() {}

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
