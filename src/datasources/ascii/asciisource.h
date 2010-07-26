/*************************
**************************************************
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

#include <QVarLengthArray>

class QFile;
class DataInterfaceAsciiVector;

class AsciiSource : public Kst::DataSource
{
  Q_OBJECT

  public:
    AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());

    ~AsciiSource();


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
    // TODO Is this too big or should we use even more: 1MB on the stack?
#define KST_PREALLOC 1 * 1024 * 1024
    QVarLengthArray<char, KST_PREALLOC> _tmpBuffer;
    QVarLengthArray<int, KST_PREALLOC / 4> _rowIndex;

    int _numFrames;
    int _byteLength;
    friend class ConfigWidgetAscii;
    mutable AsciiSourceConfig _config;
       

    bool _haveHeader;
    bool _fieldListComplete;

    QStringList _scalarList;
    QStringList _stringList;
    QStringList _fieldList;

    DataInterfaceAsciiVector* iv;

    bool openValidFile(QFile &file);

    static bool openFile(QFile &file);

    // TODO remove
    friend class DataInterfaceAsciiVector;
};


#endif
// vim: ts=2 sw=2 et
