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

#ifndef ASCII_DATA_READER_H
#define ASCII_DATA_READER_H

#include "asciisourceconfig.h"
#include "asciidatareader.h"
#include "math_kst.h"
#include "kst_inf.h"

#include <QVarLengthArray>

class QFile;


class DataInterfaceAsciiString;
class DataInterfaceAsciiVector;
struct LexicalCast;

class AsciiDataReader
{
  public:
    AsciiDataReader(AsciiSourceConfig& c);
    ~AsciiDataReader();

    // TODO remove
    mutable AsciiSourceConfig& _config;


    class FileBuffer
    {
    public:

      enum SizeOnStack
      {
        Prealloc =
#if defined(__ANDROID__) || defined(__QNX__) // Some mobile systems really do not like you allocating 1MB on the stack.
        1 * 1024
#else
        1 * 1024 * 1024
#endif
      };

      typedef QVarLengthArray<char, Prealloc> Array;

      inline FileBuffer() : _bufferedS(-10), _bufferedN(-10), _array(new Array) {}
      inline ~FileBuffer() { delete _array; }
      
      int _bufferedS;
      int _bufferedN;

      inline void clear() { _array->clear(); }
      inline int size() const { return _array->size(); }
      inline bool resize(int size);
      inline int  capacity() const { return _array->capacity(); }
      inline char* data() { return _array->data(); }

      inline const char* const constPointer() const { return _array->data(); }
      inline const Array& constArray() const{ return *_array; }

      void clearFileBuffer(bool forceDelete = false);

    private:
      Array* _array;
    };

    typedef QVarLengthArray<int, AsciiDataReader::FileBuffer::Prealloc> RowIndex;

    void clearFileBuffer(bool forceDelete = false);

    template<class T>
    bool resizeBuffer(T& buffer, int bytes);


    int readFromFile(QFile&, AsciiDataReader::FileBuffer&, int start, int numberOfBytes, int maximalBytes = -1); 
    
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
    int readColumns(const RowIndex& rowIndex, double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const LineEndingType&, const ColumnDelimiter&);

    template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
    int readColumns(const RowIndex& rowIndex, double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const LineEndingType&, const ColumnDelimiter&, const CommentDelimiter&);

    template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
    int readColumns(const RowIndex& rowIndex, double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const IsLineBreak&, const ColumnDelimiter&, const CommentDelimiter&, const ColumnWidthsAreConst&);

    template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
    bool findDataRows(const Buffer& buffer, int bufstart, int bufread, const IsLineBreak&, const CommentDelimiter&);

    void toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row);

    const IsDigit isDigit;
    const IsWhiteSpace isWhiteSpace;
};



//-------------------------------------------------------------------------------------------
template<class Buffer, typename ColumnDelimiter>
int AsciiDataReader::readColumns(const RowIndex& rowIndex, double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                              const LineEndingType& lineending, const ColumnDelimiter& column_del)
{
  if (_config._delimiters.value().size() == 0) {
    const NoDelimiter comment_del;
    return readColumns(rowIndex, v, buffer, bufstart, bufread, col, s, n, lineending, column_del, comment_del);
  } else if (_config._delimiters.value().size() == 1) {
    const IsCharacter comment_del(_config._delimiters.value()[0].toLatin1());
    return readColumns(rowIndex, v, buffer, bufstart, bufread, col, s, n, lineending, column_del, comment_del);
  } else if (_config._delimiters.value().size() > 1) {
    const IsInString comment_del(_config._delimiters.value());
    return readColumns(rowIndex, v, buffer, bufstart, bufread, col, s, n, lineending, column_del, comment_del);
  }

  return 0;
}

template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
int AsciiDataReader::readColumns(const RowIndex& rowIndex, double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                              const LineEndingType& lineending, const ColumnDelimiter& column_del, const CommentDelimiter& comment_del)
{
  if (_config._columnWidthIsConst) {
    const AlwaysTrue column_withs_const;
    if (lineending.isLF()) {
      return readColumns(rowIndex, v, buffer, bufstart, bufread, col, s, n, IsLineBreakLF(lineending), column_del, comment_del, column_withs_const);
    } else {
      return readColumns(rowIndex, v, buffer, bufstart, bufread, col, s, n, IsLineBreakCR(lineending), column_del, comment_del, column_withs_const);
    }
  } else {
    const AlwaysFalse column_withs_const;
    if (lineending.isLF()) {
      return readColumns(rowIndex, v, buffer, bufstart, bufread, col, s, n, IsLineBreakLF(lineending), column_del, comment_del, column_withs_const);
    } else {
      return readColumns(rowIndex, v, buffer, bufstart, bufread, col, s, n, IsLineBreakCR(lineending), column_del, comment_del, column_withs_const);
    }
  }
}


template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
int AsciiDataReader::readColumns(const RowIndex& rowIndex, double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                              const IsLineBreak& isLineBreak,
                              const ColumnDelimiter& column_del, const CommentDelimiter& comment_del,
                              const ColumnWidthsAreConst& are_column_widths_const)
{
  LexicalCast lexc;
  lexc.setDecimalSeparator(_config._useDot);
  const QString delimiters = _config._delimiters.value();

  bool is_custom = (_config._columnType.value() == AsciiSourceConfig::Custom);

  int col_start = -1;
  for (int i = 0; i < n; i++, s++) {
    bool incol = false;
    int i_col = 0;

    if (are_column_widths_const()) {
      if (col_start != -1) {
        v[i] = lexc.toDouble(&buffer[0] + rowIndex[s] + col_start);
        continue;
      }
    }

    v[i] = Kst::NOPOINT;
    for (int ch = rowIndex[s] - bufstart; ch < bufread; ++ch) {
      if (isLineBreak(buffer[ch])) {
        break;
      } else if (column_del(buffer[ch])) { //<- check for column start
        if ((!incol) && is_custom) {
          ++i_col;
          if (i_col == col) {
            v[i] = NAN;
          }
        }
        incol = false;
      } else if (comment_del(buffer[ch])) {
        break;
      } else {
        if (!incol) {
          incol = true;
          ++i_col;
          if (i_col == col) {
            toDouble(lexc, &buffer[0], bufread, ch, &v[i], i);
            if (are_column_widths_const()) {
              if (col_start == -1) {
                col_start = ch - rowIndex[s];
              }
            }
            break;
          }
        }
      }
    }
  }
  return n;
}






#endif
// vim: ts=2 sw=2 et
