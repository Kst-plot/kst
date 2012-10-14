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

    AsciiCharacterTraits::LineEndingType detectLineEndingType(QFile& file) const;
    AsciiCharacterTraits::LineEndingType _lineending;
    
    int readFromFile(QFile&, AsciiFileBuffer&, int start, int numberOfBytes, int maximalBytes = -1); 
    
    int readField(AsciiFileBuffer* _fileBuffer, int col, int bufstart, int bufread,
                  double *v, const QString& field, int s, int n);

    template<class Buffer, typename ColumnDelimiter>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&);

    template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&, const CommentDelimiter&);

    template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const IsLineBreak&, const ColumnDelimiter&, const CommentDelimiter&, const ColumnWidthsAreConst&);

    bool findDataRows(int& _numFrames, bool read_completely, QFile& file, int _byteLength);

    template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
    bool findDataRows(int& _numFrames, const Buffer& buffer, int bufstart, int bufread, const IsLineBreak&, const CommentDelimiter&);

    void toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row);

    const AsciiCharacterTraits::IsDigit isDigit;
    const AsciiCharacterTraits::IsWhiteSpace isWhiteSpace;
};

#endif
// vim: ts=2 sw=2 et
