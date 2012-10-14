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
#include "asciicharactertraits.h"

class QFile;
struct LexicalCast;
class AsciiSourceConfig;


class AsciiDataReader
{
  public:
    AsciiDataReader(AsciiSourceConfig& config);
    ~AsciiDataReader();

    typedef QVarLengthArray<int, AsciiFileBuffer::Prealloc> RowIndex;

    void clear();
    void setRow0Begin(int begin);
    inline int beginOfRow(int row) const { return _rowIndex[row]; }
    inline int numberOfFrames() const { return _numFrames; }

    void detectLineEndingType(QFile& file);
    
    bool findDataRows(bool read_completely, QFile& file, int _byteLength);
    int readField(const AsciiFileBuffer &buf, int col, double *v, const QString& field, int s, int n);

  private:
    int _numFrames;
    RowIndex _rowIndex;
    AsciiSourceConfig& _config;
    AsciiCharacterTraits::LineEndingType _lineending;

    const AsciiCharacterTraits::IsDigit isDigit;
    const AsciiCharacterTraits::IsWhiteSpace isWhiteSpace;

    template<class T>
    bool resizeBuffer(T& buffer, int bytes);

    template<class Buffer, typename ColumnDelimiter>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&);

    template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&, const CommentDelimiter&);

    template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const IsLineBreak&, const ColumnDelimiter&, const CommentDelimiter&, const ColumnWidthsAreConst&);    

    template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
    bool findDataRows(const Buffer& buffer, int bufstart, int bufread, const IsLineBreak&, const CommentDelimiter&);

    void toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row);
};

#endif
// vim: ts=2 sw=2 et
