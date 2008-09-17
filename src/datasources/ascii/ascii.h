/***************************************************************************
                       ascii.h  -  ASCII data source
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASCII_H
#define ASCII_H

#include <QFile>

#include <datasource.h>
#include <dataplugin.h>

class AsciiSource : public Kst::DataSource {
  Q_OBJECT

  public:
    AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());

    ~AsciiSource();

    static int readFullLine(QFile &file, QByteArray &str);
    bool initRowIndex();

    Kst::Object::UpdateType update();

    int readField(double *v, const QString &field, int s, int n);

    virtual int readMatrix(Kst::MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps);

    bool matrixDimensions(const QString& matrix, int* xDim, int* yDim);

    bool isValidField(const QString &field) const;

    bool isValidMatrix(const QString &field) const;

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString::null) const;

    QString fileType() const;

    QStringList fieldList() const;

    QStringList matrixList() const;

    void save(QXmlStreamWriter &s);
    void parseProperties(QXmlStreamAttributes &properties);

    bool isEmpty() const;

    bool supportsTimeConversions() const;

    int sampleForTime(double ms, bool *ok);

    int sampleForTime(const QDateTime& time, bool *ok);

    bool fieldListIsComplete() const;

    bool reset();

    class Config;
    static QStringList fieldListFor(const QString& filename, Config *cfg);
    static QStringList scalarListFor(const QString& filename, Config *cfg);

  private:
    int *_rowIndex;
    int _numLinesAlloc;
    int _numFrames;
    int _byteLength;
    mutable QStringList _fields;
    friend class ConfigWidgetAscii;
    mutable Config *_config;
    char *_tmpBuf;
    uint _tmpBufSize;
    bool _haveHeader;
    mutable bool _fieldListComplete;
};


class AsciiPlugin : public QObject, public Kst::DataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(Kst::DataSourcePluginInterface)
  public:
    virtual ~AsciiPlugin() {}

    virtual QString pluginName() const;
    virtual QString pluginDescription() const;

    virtual bool hasConfigWidget() const { return true; }

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

    virtual int understands(QSettings *cfg, const QString& filename) const;

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const;

    virtual QStringList provides() const;

    virtual Kst::DataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const;
};

#endif
// vim: ts=2 sw=2 et
