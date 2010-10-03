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


#ifndef HEALPIX_H
#define HEALPIX_H

#include <datasource.h>
#include <dataplugin.h>
//#include <fitsio.h>

#include "healpix_tools.h"

class HealpixSource : public Kst::DataSource {
  Q_OBJECT

  public:
    HealpixSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~HealpixSource();

    bool init();
    bool reset();

    Kst::Object::UpdateType update();

    bool matrixDimensions( const QString& matrix, int* xDim, int* yDim);
    int readMatrix(Kst::MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps);
    bool isValidMatrix(const QString& field) const;

    int readField(double *v, const QString &field, int s, int n);
    bool isValidField(const QString &field) const;

    int samplesPerFrame(const QString &field);
    int frameCount(const QString& field = QString::null) const;
    bool isEmpty() const;
    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);
    void parseProperties(QXmlStreamAttributes &properties);

    class Config;


    int readScalar(double &S, const QString& scalar);
    int readString(QString &S, const QString& string);

  private:
    mutable Config *_config;
    friend class ConfigWidgetHealpix;

    // file parameters
    int _mapType;
    int _mapOrder;
    int _mapCoord;
    size_t _mapNside;
    size_t _nMaps;
    size_t _mapNpix;
    char _healpixfile[HEALPIX_STRNL];
    healpix_keys *_keys;
    char _creator[HEALPIX_STRNL];
    char _extname[HEALPIX_STRNL];
    char **_names;
    char **_units;

    QMap<QString, QString> _metaData;
};


class HealpixPlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
  public:
    virtual ~HealpixPlugin() {}

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
