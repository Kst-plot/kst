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

#include "datasource.h"
#include "dataplugin.h"
#include "asciisourceconfig.h"


class QFile;
class DataInterfaceAsciiVector;

class AsciiSource : public Kst::DataSource
{
  Q_OBJECT

  public:
    AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());

    ~AsciiSource();

    static int readFullLine(QFile &file, QByteArray &str);
    bool initRowIndex();

    virtual UpdateType internalDataSourceUpdate();

    int readField(double *v, const QString &field, int s, int n);


    QString fileType() const;

    void save(QXmlStreamWriter &s);
    void parseProperties(QXmlStreamAttributes &properties);

    bool isEmpty() const;

    bool supportsTimeConversions() const;

    int sampleForTime(double ms, bool *ok);

    int sampleForTime(const QDateTime& time, bool *ok);


    virtual void reset();

    virtual const QString& typeString() const;

    static const QString asciiTypeKey();

    static QStringList fieldListFor(const QString& filename, AsciiSourceConfig *cfg);
    static QStringList scalarListFor(const QString& filename, AsciiSourceConfig *cfg);
    static QStringList stringListFor(const QString& filename, AsciiSourceConfig *cfg);

  private:
    int *_rowIndex;
    int _numLinesAlloc;
    int _numFrames;
    int _byteLength;
    friend class ConfigWidgetAscii;
    mutable AsciiSourceConfig _config;
    char *_tmpBuf;
    uint _tmpBufSize;
    bool _haveHeader;
    bool _fieldListComplete;

    QStringList _scalarList;
    QStringList _stringList;
    QStringList _fieldList;

    DataInterfaceAsciiVector* iv;

    // TODO remove
    friend class DataInterfaceAsciiVector;
};


#endif
// vim: ts=2 sw=2 et
