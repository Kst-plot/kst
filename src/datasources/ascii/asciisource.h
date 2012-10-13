/*************************
**************************************************
                       ascii.h  -  ASCII data source
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of Toronto
    email                : netterfield@astro.utoronto.ca
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
#include "asciidatareader.h"

#include <QVarLengthArray>
#include <QFile>
#include <QMap>


class DataInterfaceAsciiString;
class DataInterfaceAsciiVector;
struct LexicalCast;

class AsciiSource : public Kst::DataSource
{
  Q_OBJECT

  public:
    AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());

    ~AsciiSource();


    bool initRowIndex();

    UpdateType internalDataSourceUpdate(bool read_completely);

    virtual UpdateType internalDataSourceUpdate();

    int readField(double *v, const QString &field, int s, int n);
    int readField(double *v, const QString &field, int s, int n, bool& success);

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
    static QStringList unitListFor(const QString& filename, AsciiSourceConfig* cfg);
    static QStringList scalarListFor(const QString& filename, AsciiSourceConfig *cfg);
    static QStringList stringListFor(const QString& filename, AsciiSourceConfig *cfg);

    Kst::ObjectList<Kst::Object> autoCurves(Kst::ObjectStore& objectStore);

  private:
    AsciiDataReader r;
    AsciiDataReader::FileBuffer* _fileBuffer;
    

    friend class ConfigWidgetAscii;
    mutable AsciiSourceConfig _config;
       

    int _numFrames;
    int _byteLength;
    bool _haveHeader;
    bool _fieldListComplete;


    QStringList _scalarList;
    QMap<QString, QString> _strings;
    QStringList _fieldList;
    QMap<QString, QString> _fieldUnits;

    int columnOfField(const QString& field) const;
    static QStringList splitHeaderLine(const QByteArray& line, AsciiSourceConfig* cfg);

    DataInterfaceAsciiString* is;
    DataInterfaceAsciiVector* iv;

    bool openValidFile(QFile &file);
    static bool openFile(QFile &file);

    // TODO remove
    friend class DataInterfaceAsciiString;
    friend class DataInterfaceAsciiVector;
};


#endif
// vim: ts=2 sw=2 et
