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


#ifndef PLANCKIDEF_H
#define PLANCKIDEF_H

#include <datasource.h>
#include <dataplugin.h>
#include <libcfitsio0/fitsio.h>

typedef struct {
  QString file;
  double dTimeZero;
  double dTimeDelta;
  long frameLo;
  long frames;
} folderField;

typedef QList<folderField> fileList;

typedef struct {
  QString basefile;
  int table;
  int column;
} field;

class PlanckIDEFSource : public Kst::DataSource {
  Q_OBJECT

  public:
    PlanckIDEFSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e);

    ~PlanckIDEFSource();

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
    void parseProperties(QXmlStreamAttributes &properties);

    class Config;

    int readScalar(double &S, const QString& scalar);
    int readString(QString &S, const QString& string);

    static bool isValidFilename(const QString& filename, Config *config);
    static QString baseFilename(const QString& filename);

    static bool checkValidPlanckIDEFFile(const QString& filename, PlanckIDEFSource::Config *config);
    static bool checkValidPlanckIDEFFolder(const QString& filename);

    static QStringList fieldList(const QString& filename);
    static QStringList stringList(const QString& filename);
    static QStringList fieldListFromFile(const QString& filename);
    static QStringList stringListFromFile(const QString& filename);
    static QStringList fieldListFromFolder(const QString& filename);
    static QStringList stringListFromFolder(const QString& filename);

  private:
    bool initFile(const QString& filename);
    bool initFile();
    bool initFolderFile(const QString& filename, const QString& prefix, const QString& baseName);
    bool initFolder();

    void addToMetadata(fitsfile *ffits, int &iStatus);
    void addToFieldList(fitsfile *ffits, const int iNumCols, int &iStatus);
    void addToFieldList(fitsfile *ffits, const QString& prefix, const QString& baseName, const int iNumCols, int &iStatus);

    long getNumFrames(const QString& filename);
    static long getNumFrames(fitsfile* ffits, int iNumHeaderDataUnits);
    int readFileFrames(const QString& filename, field *fld, double *v, int s, int n);
    int readFolderFrames(field *fld, double *v, int s, int n);

    mutable Config *_config;
    friend class ConfigWidgetPlanckIDEF;

    QMap<QString, QString> _metaData;
    QMap<QString, field*> _fields;
    QMap<QString, fileList*> _basefiles;
    double _dTimeZero;
    double _dTimeDelta;
    bool _bHasTime;
    bool _first;
    bool _isSingleFile;
    int _numFrames;
    int _numCols;
};


class PlanckIDEFPlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
  public:
    virtual ~PlanckIDEFPlugin() {}

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
