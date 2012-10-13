/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "asciidatareader.h"

#include "math_kst.h"
#include "kst_inf.h"
#include "kst_i18n.h"
#include "kst_atof.h"
#include "measuretime.h"

#include <QFile>
#include <QMessageBox>
        
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>


// Enable QASSERT in QVarLengthArray  when using [] on data
#if 0
  #define constData constArray
#else
  #define constData constPointer // loads faster in debug mode
#endif


AsciiDataReader::AsciiDataReader(AsciiSourceConfig& c) : _config(c)
{
}


AsciiDataReader::~AsciiDataReader()
{
}
    




//-------------------------------------------------------------------------------------------
AsciiDataReader::LineEndingType AsciiDataReader::detectLineEndingType(QFile& file) const
{
  QByteArray line;
  int line_size = 0;
  while (line_size < 2 && !file.atEnd()) {
     line = file.readLine();
     line_size = line.size();
  }
  file.seek(0);
  if (line_size < 2) {
    return LineEndingType();
  }
  LineEndingType end;
  end.is_crlf = line[line_size - 2] == '\r' && line[line_size - 1] == '\n' ;
  end.character =  end.is_crlf ? line[line_size - 2] : line[line_size - 1];
  return end;
}



//-------------------------------------------------------------------------------------------
void AsciiDataReader::toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int)
{
  if (   isDigit(buffer[ch])
      || buffer[ch] == '-'
      || buffer[ch] == '.'
      || buffer[ch] == '+'
      || isWhiteSpace(buffer[ch])) {
    *v = lexc.toDouble(&buffer[0] + ch);
  } else if ( ch + 2 < bufread
              && tolower(buffer[ch]) == 'i'
              && tolower(buffer[ch + 1]) == 'n'
              && tolower(buffer[ch + 2]) == 'f') {
    *v = INF;
  }

#if 0
  // TODO enable by option: "Add unparsable lines as strings"
  else {
    if (_rowIndex.size() > row + 1) {
      QString unparsable = QString::fromAscii(&buffer[_rowIndex[row]], _rowIndex[row + 1] - _rowIndex[row]);
      _strings[QString("Unparsable %1").arg(row)] = unparsable.trimmed();
    }
  }
#endif

}


int AsciiDataReader::readFromFile(QFile& file, AsciiFileBuffer& buffer, int start, int bytesToRead, int maximalBytes)
{    
  if (maximalBytes == -1) {
    if (!buffer.resize(bytesToRead + 1))
      return 0;
  } else {
    bytesToRead = qMin(bytesToRead, maximalBytes);
    if (buffer.size() <= bytesToRead) {
      if (!buffer.resize(bytesToRead + 1))
        return 0;
    }
  }
  file.seek(start); // expensive?
  int bytesRead = file.read(buffer.data(), bytesToRead);
  if (buffer.size() <= bytesRead) {
    if (!buffer.resize(bytesToRead + 1))
      return 0;
  }
  buffer.data()[bytesRead] = '\0';
  return bytesRead;
}


int AsciiDataReader::readField(AsciiFileBuffer* _fileBuffer, int col, int bufstart, int bufread,
                               double *v, const QString& field, int s, int n) 
  {

  if (_config._columnType == AsciiSourceConfig::Fixed) {
    MeasureTime t("AsciiSource::readField: same width for all columns");
    LexicalCast lexc;
    lexc.setDecimalSeparator(_config._useDot);
    // &buffer[0] points to first row at _rowIndex[0] , so if we wanna find
    // the column in row i by adding _rowIndex[i] we have to start at:
    const char* col_start = &_fileBuffer->constData()[0] - _rowIndex[0] + _config._columnWidth * (col - 1);
    for (int i = 0; i < n; ++i) {
      v[i] = lexc.toDouble(_rowIndex[i] + col_start);
    }
    return n;
  } else if (_config._columnType == AsciiSourceConfig::Custom) {
    if (_config._columnDelimiter.value().size() == 1) {
      MeasureTime t("AsciiSource::readField: 1 custom column delimiter");
      const AsciiDataReader::IsCharacter column_del(_config._columnDelimiter.value()[0].toLatin1());
      return readColumns(v, _fileBuffer->constData(), bufstart, bufread, col, s, n, _lineending, column_del);
    } if (_config._columnDelimiter.value().size() > 1) {
      MeasureTime t(QString("AsciiSource::readField: %1 custom column delimiters").arg(_config._columnDelimiter.value().size()));
      const AsciiDataReader::IsInString column_del(_config._columnDelimiter.value());
      return readColumns(v, _fileBuffer->constData(), bufstart, bufread, col, s, n, _lineending, column_del);
    }
  } else if (_config._columnType == AsciiSourceConfig::Whitespace) {
    MeasureTime t("AsciiSource::readField: whitespace separated columns");
    const AsciiDataReader::IsWhiteSpace column_del;
    return readColumns(v, _fileBuffer->constData(), bufstart, bufread, col, s, n, _lineending, column_del);
  }

  return 0;
}


bool AsciiDataReader::findDataRows(int& numFrames, bool read_completely, QFile& file, int _byteLength)
{
  AsciiDataReader::LineEndingType lineending = detectLineEndingType(file);

  bool new_data = false;

  AsciiFileBuffer buf;
  do {
    // Read the tmpbuffer, starting at row_index[_numFrames]
    buf.clear();
    buf.resize(buf.capacity());

    //bufstart += bufread;
    buf._bufferedS = _rowIndex[numFrames]; // always read from the start of a line
    buf._bufferedN = readFromFile(file, buf, buf._bufferedS, _byteLength - buf._bufferedS, AsciiFileBuffer::Prealloc - 1);

    if (_config._delimiters.value().size() == 0) {
      const AsciiDataReader::NoDelimiter comment_del;
      if (lineending.isLF()) {
        new_data = findDataRows(numFrames, buf.constData(), buf._bufferedS, buf._bufferedN, AsciiDataReader::IsLineBreakLF(lineending), comment_del);
      } else {
        new_data = findDataRows(numFrames, buf.constData(), buf._bufferedS, buf._bufferedN, AsciiDataReader::IsLineBreakCR(lineending), comment_del);
      }
    } else if (_config._delimiters.value().size() == 1) {
      const AsciiDataReader::IsCharacter comment_del(_config._delimiters.value()[0].toLatin1());
      if (lineending.isLF()) {
        new_data = findDataRows(numFrames, buf.constData(), buf._bufferedS, buf._bufferedN, AsciiDataReader::IsLineBreakLF(lineending), comment_del);
      } else {
        new_data = findDataRows(numFrames, buf.constData(), buf._bufferedS, buf._bufferedN, AsciiDataReader::IsLineBreakCR(lineending), comment_del);
      }
    } else if (_config._delimiters.value().size() > 1) {
      const AsciiDataReader::IsInString comment_del(_config._delimiters.value());
      if (lineending.isLF()) {
        new_data = findDataRows(numFrames, buf.constData(), buf._bufferedS, buf._bufferedS, AsciiDataReader::IsLineBreakLF(lineending), comment_del);
      } else {
        new_data = findDataRows(numFrames, buf.constData(), buf._bufferedS, buf._bufferedN, AsciiDataReader::IsLineBreakCR(lineending), comment_del);
      }
    }
  } while (buf._bufferedN == AsciiFileBuffer::Prealloc - 1  && read_completely);

  _rowIndex.resize(numFrames + 1);

  return new_data;
}

template<class Buffer, typename IsLineBreak, typename CommentDelimiter>
bool AsciiDataReader::findDataRows(int& _numFrames, const Buffer& buffer, int bufstart, int bufread, const IsLineBreak& isLineBreak, const CommentDelimiter& comment_del)
{
  const AsciiDataReader::IsWhiteSpace isWhiteSpace;

  bool new_data = false;
  bool row_has_data = false;
  bool is_comment = false;
  const int row_offset = bufstart + isLineBreak.size;
  int row_start = bufstart;

  for (int i = 0; i < bufread; i++) {
    if (comment_del(buffer[i])) {
      is_comment = true;
    } else if (isLineBreak(buffer[i])) {
      if (row_has_data) {
        _rowIndex[_numFrames] = row_start;
        ++_numFrames;
        if (_numFrames >= _rowIndex.size()) {
          _rowIndex.resize(_rowIndex.size() + AsciiFileBuffer::Prealloc - 1);
        }
        new_data = true;
        row_start = row_offset+i;
      } else if (is_comment) {
        row_start = row_offset+i;
      }
      row_has_data = false;
      is_comment = false;
    } else if (!row_has_data && !isWhiteSpace(buffer[i]) && !is_comment) {
      row_has_data = true;
    }
  }
  _rowIndex[_numFrames] = row_start;
  return new_data;
}


// vim: ts=2 sw=2 et
