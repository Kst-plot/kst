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

    UpdateType internalDataSourceUpdate(bool read_completely);

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
    static QStringList unitListFor(const QString& filename, AsciiSourceConfig* cfg);
    static QStringList scalarListFor(const QString& filename, AsciiSourceConfig *cfg);
    static QStringList stringListFor(const QString& filename, AsciiSourceConfig *cfg);

    Kst::ObjectList<Kst::Object> autoCurves(Kst::ObjectStore& objectStore);

  private:
    // Question: Is this too big or should we use even more: 1MB on the stack?
#if defined(__ANDROID__) || defined(__QNX__)
    // Answer: Depends on the system. Some mobile systems, for example, really do not like you allocating 1MB on the stack.
#define KST_PREALLOC 1 * 1024
#else
#define KST_PREALLOC 1 * 1024 * 1024
#endif
    typedef QVarLengthArray<char, KST_PREALLOC> FileBuffer;
    FileBuffer* _fileBuffer;
    void clearFileBuffer();
    int _bufferedS;
    int _bufferedN;
    
    QVarLengthArray<int, KST_PREALLOC> _rowIndex;

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

    template<class T>
    int readFromFile(QFile&, T& buffer, int start, int numberOfBytes, int maximalBytes = -1);
    
    int readField(double *v, const QString &field, int s, int n, bool& re_alloc);


    struct LineEndingType {
      bool is_crlf;
      char character;
      bool isCR() const { return character == '\r'; }
      bool isLF() const { return character == '\n'; }
    };
    LineEndingType detectLineEndingType(QFile& file) const;
    LineEndingType _lineending;

    // column and comment delimiter functions

    struct AlwaysTrue {
      AlwaysTrue() {
      }
      inline bool operator()() const {
        return true;
      }
    };

    struct AlwaysFalse {
      AlwaysFalse() {
      }
      inline bool operator()() const {
        return false;
      }
    };

    struct NoDelimiter {
      NoDelimiter() {
      }
      inline bool operator()(const char) const {
        return false;
      }
    };

    struct  IsWhiteSpace {
      IsWhiteSpace() {
      }
      inline bool operator()(const char c) const {
        return c == ' ' || c == '\t';
      }
    };

    struct IsDigit {
      IsDigit() {
      }
      inline bool operator()(const char c) const {
        return (c >= 48) && (c <= 57) ? true : false;
      }
    };

    struct IsCharacter {
      IsCharacter(char c) : character(c) {
      }
      const char character;
      inline bool operator()(const char c) const {
        return character == c;
      }
    };

    struct IsInString {
      IsInString(const QString& s) : str(s), chars(s.size()) {
        QByteArray ascii = str.toLatin1();
        for (int i = 0; i < 6 && i < chars; i++) {
          ch[i] = ascii[i];
        }
      }
      const QString str;
      const int chars;
      char ch[6];
      inline bool operator()(const char c) const {
        switch (chars) {
          case 0: return false;
          case 1: return ch[0]==c;
          case 2: return ch[0]==c || ch[1]==c;
          case 3: return ch[0]==c || ch[1]==c || ch[2]==c;
          case 4: return ch[0]==c || ch[1]==c || ch[2]==c || ch[3]==c;
          case 5: return ch[0]==c || ch[1]==c || ch[2]==c || ch[3]==c || ch[4]==c;
          case 6: return ch[0]==c || ch[1]==c || ch[2]==c || ch[3]==c || ch[4]==c || ch[5]==c;
          default: return str.contains(c);
        }
      }
    };

    struct IsLineBreakLF {
      IsLineBreakLF(const LineEndingType&) : size(1) {
      }
      const int size;
      inline bool operator()(const char c) const {
        return c == '\n';
      }
    };

    struct IsLineBreakCR {
      IsLineBreakCR(const LineEndingType& t) : size( t.is_crlf ? 2 : 1 ) {
      }
      const int size;
      inline bool operator()(const char c) const {
        return c == '\r';
      }
    };


    template<class Buffer, typename ColumnDelimiter>
    int readColumns(double* v, Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const LineEndingType&, const ColumnDelimiter&);

    template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
    int readColumns(double* v, Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const LineEndingType&, const ColumnDelimiter&, const CommentDelimiter&);

    template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
    int readColumns(double* v, Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const IsLineBreak&, const ColumnDelimiter&, const CommentDelimiter&, const ColumnWidthsAreConst&);

    template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
    bool findDataRows(Buffer& buffer, int bufstart, int bufread, const IsLineBreak&, const CommentDelimiter&);

    void toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row);

    const IsDigit isDigit;
    const IsWhiteSpace isWhiteSpace;

    // TODO remove
    friend class DataInterfaceAsciiString;
    friend class DataInterfaceAsciiVector;
};


template<class T>
int AsciiSource::readFromFile(QFile& file, T& buffer, int start, int bytesToRead, int maximalBytes)
{    
  const int oldSize = buffer.size();
  if (maximalBytes == -1) {
    buffer.resize(bytesToRead + 1);
    if (buffer.size() == oldSize)
      return 0;
  } else {
    bytesToRead = qMin(bytesToRead, maximalBytes);
    if (buffer.size() <= bytesToRead) {
      buffer.resize(bytesToRead + 1);
      if (buffer.size() == oldSize)
        return 0;
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
