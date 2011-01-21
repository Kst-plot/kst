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

    friend class ConfigWidgetAscii;
    mutable AsciiSourceConfig _config;
       

    int _numFrames;
    int _byteLength;
    bool _haveHeader;
    bool _fieldListComplete;

    QStringList _scalarList;
    QMap<QString, QString> _strings;
    QStringList _fieldList;

    int columnOfField(const QString& field) const;

    DataInterfaceAsciiString* is;
    DataInterfaceAsciiVector* iv;

    bool openValidFile(QFile &file);
    static bool openFile(QFile &file);
    template<class T>
    int readFromFile(QFile&, T& buffer, int start, int numberOfBytes, int maximalBytes = -1);

    void toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row);

    // TODO remove
    friend class DataInterfaceAsciiString;
    friend class DataInterfaceAsciiVector;
};


template<class T>
int AsciiSource::readFromFile(QFile& file, T& buffer, int start, int bytesToRead, int maximalBytes)
{    
  if (maximalBytes == -1) {
    buffer.resize(bytesToRead + 1);
  } else {
    bytesToRead = qMin(bytesToRead, maximalBytes);
    if (buffer.size() <= bytesToRead) {
      buffer.resize(bytesToRead + 1);
    }
  }
  file.seek(start); // expensive?
  int bytesRead = file.read(buffer.data(), bytesToRead);
  if (buffer.size() <= bytesRead) {
    buffer.resize(bytesRead + 1);
  }
  buffer.data()[bytesRead] = '\0';  
  return bytesRead;
}


#endif
// vim: ts=2 sw=2 et
