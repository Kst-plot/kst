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

#include <QVarLengthArray>
#include <QMutex>

class QFile;
class LexicalCast;
class AsciiSourceConfig;


class AsciiDataReader
{
  public:
    AsciiDataReader(AsciiSourceConfig& config);
    ~AsciiDataReader();

    void clear();
    void setRow0Begin(int begin);
    inline int beginOfRow(int row) const { return _rowIndex[row]; }
    inline int numberOfFrames() const { return _numFrames; }

    // where
    const AsciiFileBuffer::RowIndex& rowIndex() const { return _rowIndex; }
    
    void detectLineEndingType(QFile& file);
    
    bool findDataRows(bool read_completely, QFile& file, int _byteLength);
    int readField(const AsciiFileData &buf, int col, double *v, const QString& field, int s, int n);
    int readFieldFromChunk(const AsciiFileData& chunk, int col, double *v, const QString& field);

  private:
    int _numFrames;
    AsciiFileBuffer::RowIndex _rowIndex;
    AsciiSourceConfig& _config;
    AsciiCharacterTraits::LineEndingType _lineending;

    const AsciiCharacterTraits::IsDigit isDigit;
    const AsciiCharacterTraits::IsWhiteSpace isWhiteSpace;

    template<class T>
    bool resizeBuffer(T& buffer, int bytes);

    template<class Buffer, typename ColumnDelimiter>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&) const;

    template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&, const CommentDelimiter&) const;

    template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
    int readColumns(double* v, const Buffer& buffer, int bufstart, int bufread, int col, int s, int n,
                    const IsLineBreak&, const ColumnDelimiter&, const CommentDelimiter&, const ColumnWidthsAreConst&) const;

    template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
    bool findDataRows(const Buffer& buffer, int bufstart, int bufread, const IsLineBreak&, const CommentDelimiter&);

    void toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row) const;

    mutable QMutex _localeMutex;
};

#endif
// vim: ts=2 sw=2 et
