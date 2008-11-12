/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef LFIIO_H
#define LFIIO_H

#include <datasource.h>
#include <dataplugin.h>
#include <fitsio.h>

class LFIIOSource : public Kst::DataSource {
  Q_OBJECT

  public:
    LFIIOSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~LFIIOSource();

    bool init();
    bool reset();

    Kst::Object::UpdateType update();

    int readField(double *v, const QString &field, int s, int n);
    bool isValidField(const QString &field) const;

    int samplesPerFrame(const QString &field);
    int frameCount(const QString& field = QString::null) const;
    bool isEmpty() const;
    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    class Config;

    int readScalar(double &S, const QString& scalar);
    int readString(QString &S, const QString& string);

  private:
    bool getColNumber(const QString& field, int* piColNumber) const;

    double _dTimeZero;
    double _dTimeDelta;
    bool _bHasTime;
    bool _first;
    int _numFrames;
    int _numCols;
    mutable Config *_config;

    QMap<QString, QString> _metaData;
};


class LFIIOPlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
  public:
    virtual ~LFIIOPlugin() {}

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
