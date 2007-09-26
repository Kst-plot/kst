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

#include <qfile.h>

#include <kstdatasource.h>
#include <kstdataplugin.h>

class AsciiSource : public KstDataSource {
  public:
    AsciiSource(QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());

    ~AsciiSource();

    static int readFullLine(QFile &file, QByteArray &str);
    bool initRowIndex();

    KstObject::UpdateType update(int = -1);

    int readField(double *v, const QString &field, int s, int n);
    
    virtual int readMatrix(KstMatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps);
    
    bool matrixDimensions(const QString& matrix, int* xDim, int* yDim);
    
    bool isValidField(const QString &field) const;
    
    bool isValidMatrix(const QString &field) const;

    int samplesPerFrame(const QString &field);

    int frameCount(const QString& field = QString::null) const;

    QString fileType() const;

    QStringList fieldList() const;
    
    QStringList matrixList() const;

    void save(QXmlStreamWriter &s);

    bool isEmpty() const;

    bool supportsTimeConversions() const;

    int sampleForTime(double ms, bool *ok);

    int sampleForTime(const QDateTime& time, bool *ok);

    bool fieldListIsComplete() const;

    bool reset();

    class Config;
    static QStringList fieldListFor(const QString& filename, Config *cfg);

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


class AsciiPlugin : public QObject, public KstDataSourcePluginInterface {
    Q_OBJECT
    Q_INTERFACES(KstDataSourcePluginInterface)
  public:
    virtual ~AsciiPlugin() {}

    virtual QString pluginName() const;

    virtual bool hasConfigWidget() const { return true; }

    virtual KstDataSource *create(QSettings *cfg,
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

    virtual int understands(QSettings *cfg, const QString& filename) const;

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const;

    virtual QStringList provides() const;

    virtual KstDataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const;
};

#endif
// vim: ts=2 sw=2 et
