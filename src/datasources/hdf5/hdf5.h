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


#ifndef HDF5_H
#define HDF5_H

#include <datasource.h>
#include <dataplugin.h>

#include <QFileInfo>
#include <hdf5/serial/H5Cpp.h>
#include <exception>

#include "debug.h"

class DataInterfaceHDF5Vector;
class DataInterfaceHDF5Scalar;
class DataInterfaceHDF5Matrix;
class DataInterfaceHDF5String;

class HDF5Source : public Kst::DataSource {
  Q_OBJECT

  public:
    HDF5Source(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    //HDF5Source(HDF5Source& s);

    ~HDF5Source();

    friend class DataInterfaceHDF5Vector;
    friend class DataInterfaceHDF5Matrix;
    friend class DataInterfaceHDF5Scalar;
    friend class DataInterfaceHDF5String;

    bool init();
    virtual void reset();

    Kst::Object::UpdateType internalDataSourceUpdate();
 
    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    class Config;

    unsigned samplesPerFrame(const QString& field);

    unsigned frameCount(const QString& field);

    int readField(double *v, const QString& field, int start, int numFrames);

    int readScalar(double& s, const QString& field);

    int readMatrix(Kst::DataMatrix::ReadInfo& m, const QString& field);

    int readString(QString &data, const QString& field);

  private:
    mutable Config *_config;
    H5::H5File* _hdfFile;
   
    static herr_t visitFunc(hid_t id, const char* name, const H5L_info_t* info, void* opData);
    static herr_t attrIterFunc(hid_t id, const char* name, const H5A_info_t* info, void* opData);
    QString _directoryName;

    DataInterfaceHDF5Vector* iv;
    DataInterfaceHDF5Scalar* ix;
    DataInterfaceHDF5Matrix* im;
    DataInterfaceHDF5String* is;

    QStringList _scalarList;
    QStringList _vectorList;
    QStringList _matrixList;
    QStringList _stringList;
    QStringList _fieldList;
    QStringList _indexList;
    QStringList _mpfList; //List of dim-3 objects that are a matrix per frame (mpf)
    //Not implemented yet but could be

    bool _resetNeeded;

    QVector<int> lengths;

};


class HDF5Plugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
    Q_PLUGIN_METADATA(IID "com.kst.DataSourcePluginInterface/2.0")
  public:
    virtual ~HDF5Plugin() {}

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
