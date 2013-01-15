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
    void setRow0Begin(qint64 begin);
    inline qint64 beginOfRow(qint64 row) const { return _rowIndex[row]; }
    inline qint64 numberOfFrames() const { return _numFrames; }

    // where
    const AsciiFileBuffer::RowIndex& rowIndex() const { return _rowIndex; }
    
    void detectLineEndingType(QFile& file);
    
    bool findDataRows(bool read_completely, QFile& file, qint64 _byteLength);
    int readField(const AsciiFileData &buf, int col, double *v, const QString& field, int start, int n);
    int readFieldFromChunk(const AsciiFileData& chunk, int col, double *v, int start, const QString& field);

    template<typename ColumnDelimiter>
    static int splitColumns(const QByteArray& line, const ColumnDelimiter& column_del, QStringList* cols = 0);

  private:
    int _numFrames;
    AsciiFileBuffer::RowIndex _rowIndex;
    AsciiSourceConfig& _config;
    AsciiCharacterTraits::LineEndingType _lineending;

    const AsciiCharacterTraits::IsDigit isDigit;
    const AsciiCharacterTraits::IsWhiteSpace isWhiteSpace;

    template<class T>
    bool resizeBuffer(T& buffer, qint64 bytes);

    template<class Buffer, typename ColumnDelimiter>
    int readColumns(double* v, const Buffer& buffer, qint64 bufstart, qint64 bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&) const;

    template<class Buffer, typename ColumnDelimiter, typename CommentDelimiter>
    int readColumns(double* v, const Buffer& buffer, qint64 bufstart, qint64 bufread, int col, int s, int n,
                    const AsciiCharacterTraits::LineEndingType&, const ColumnDelimiter&, const CommentDelimiter&) const;

    template<class Buffer, typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
    int readColumns(double* v, const Buffer& buffer, qint64 bufstart, qint64 bufread, int col, int s, int n,
                    const IsLineBreak&, const ColumnDelimiter&, const CommentDelimiter&, const ColumnWidthsAreConst&) const;

    template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
    bool findDataRows(const Buffer& buffer, qint64 bufstart, qint64 bufread, const IsLineBreak&, const CommentDelimiter&);

    void toDouble(const LexicalCast& lexc, const char* buffer, qint64 bufread, qint64 ch, double* v, int row) const;

    mutable QMutex _localeMutex;
};


template<>
int AsciiDataReader::splitColumns<AsciiCharacterTraits::IsWhiteSpace>(const QByteArray& line, const AsciiCharacterTraits::IsWhiteSpace& column_del, QStringList* cols);

#endif
// vim: ts=2 sw=2 et
