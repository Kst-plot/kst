/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2014 Northwestern University                          *
 *                   nchapman@u.northwestern.edu                           *
 *                   g-novak@northwestern.edu                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef FITSTABLE_H
#define FITSTABLE_H

#include <datasource.h>
#include <dataplugin.h>

#include <fitsio.h>

class DataInterfaceFitsTableScalar;
class DataInterfaceFitsTableString;
class DataInterfaceFitsTableVector;
class DataInterfaceFitsTableMatrix;

class FitsTableSource : public Kst::DataSource {
  Q_OBJECT

  public:
    FitsTableSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~FitsTableSource();

    bool init();
    virtual void reset();

    Kst::Object::UpdateType internalDataSourceUpdate();

    int readScalar(double *v, const QString& field);

    int readString(QString *stringValue, const QString& stringName);

    int readField(double *v, const QString& field, int s, int n);

    int readMatrix(double *v, const QString& field);

    int samplesPerFrame(const QString& field);
    int frameCount(const QString& field = QString()) const;
    
    int validField(int typecode);
    
    QString fileType() const;

    void save(QXmlStreamWriter &streamWriter);

    class Config;


  private:
    QMap<QString, long> _frameCounts;
    long _maxFrameCount;

    // FITS file object
    fitsfile *_fptr;
    mutable Config *_config;

    // Primitive lists
    QMap<QString, QString> _strings; /* dictionary-like container of strings */
    QMap<QString, double> _scalars; /* dictionary-like container of scalars */
    QStringList _fieldList;  /* list of vector names */
    QStringList _matrixList; /* list of matrix names (not implemented) */
    
    QStringList _colName;  /* Name in FITS file of each item in _fieldList */
    QList<int> _colRepeat; /* FITS repeat count of each item in _fieldList */
    QList<int> _colType;   /* FITS typecode for each item in _fieldList */
    QList<int> _colOffset; /* Offset of data, useful when repeat > 1 */
    QList<int> tableHDU;   /* Array of HDU numbers with FITS tables */
    QList<int> tableRow;   /* number of rows for each HDU in tableHDU */ 

    friend class DataInterfaceFitsTableScalar;
    friend class DataInterfaceFitsTableString;
    friend class DataInterfaceFitsTableVector;
    friend class DataInterfaceFitsTableMatrix;
    DataInterfaceFitsTableScalar* is;
    DataInterfaceFitsTableString* it;
    DataInterfaceFitsTableVector* iv;
    DataInterfaceFitsTableMatrix* im;
};


class FitsTableSourcePlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
    Q_PLUGIN_METADATA(IID "com.kst.DataSourcePluginInterface/2.0")
  public:
    virtual ~FitsTableSourcePlugin() {}

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
