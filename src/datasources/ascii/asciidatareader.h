/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2003 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASCII_DATA_READER_H
#define ASCII_DATA_READER_H

#include "asciifilebuffer.h"

class QFile;
class LexicalCast;
class AsciiSourceConfig;

class AsciiDataReader
{
  public:
    AsciiDataReader(AsciiSourceConfig& c);
    ~AsciiDataReader();

    // TODO remove
    AsciiSourceConfig& _config;
    
    typedef QVarLengthArray<int, AsciiFileBuffer::Prealloc> RowIndex;
    RowIndex _rowIndex;

    inline RowIndex& rowIndex() { return _rowIndex; }

    void clearFileBuffer(bool forceDelete = false);

    template<class T>
    bool resizeBuffer(T& buffer, int bytes);

    
    int readFromFile(QFile&, AsciiFileBuffer&, int start, int numberOfBytes, int maximalBytes = -1); 
    
    int readField(AsciiFileBuffer* _fileBuffer, int col, int bufstart, int bufread,
                  double *v, const QString& field, int s, int n);


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
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const LineEndingType&, const ColumnDelimiter&);

    template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const LineEndingType&, const ColumnDelimiter&, const CommentDelimiter&);

    template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const IsLineBreak&, const ColumnDelimiter&, const CommentDelimiter&, const ColumnWidthsAreConst&);

    bool findDataRows(int& _numFrames, bool read_completely, QFile& file, int _byteLength);

    template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
    bool findDataRows(int& _numFrames, const Buffer& buffer, int bufstart, int bufread, const IsLineBreak&, const CommentDelimiter&);

    void toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row);

    const IsDigit isDigit;
    const IsWhiteSpace isWhiteSpace;
};

#endif
// vim: ts=2 sw=2 et
