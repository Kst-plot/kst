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
bool AsciiDataReader::FileBuffer::resize(int bytes)
{ 
  try {
    _array->resize(bytes);
  } catch (const std::bad_alloc&) {
    // work around Qt bug
    clearFileBuffer(true);
    return false;
  }
  return true;
}

//-------------------------------------------------------------------------------------------
void AsciiDataReader::FileBuffer::clearFileBuffer(bool forceDelete)
{
  // force deletion of heap allocated memory if any
  if (forceDelete || _array->capacity() > AsciiDataReader::FileBuffer::Prealloc) {
    delete _array;
    _array = new Array;
  }
  _bufferedS = -10;
  _bufferedN = -10;
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


int AsciiDataReader::readFromFile(QFile& file, AsciiDataReader::FileBuffer& buffer, int start, int bytesToRead, int maximalBytes)
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


int AsciiDataReader::readField(const RowIndex& _rowIndex, FileBuffer* _fileBuffer, int col, int bufstart, int bufread,
                               double *v, const QString& field, int s, int n, bool& re_alloc) 
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
      return readColumns(_rowIndex, v, _fileBuffer->constData(), bufstart, bufread, col, s, n, _lineending, column_del);
    } if (_config._columnDelimiter.value().size() > 1) {
      MeasureTime t(QString("AsciiSource::readField: %1 custom column delimiters").arg(_config._columnDelimiter.value().size()));
      const AsciiDataReader::IsInString column_del(_config._columnDelimiter.value());
      return readColumns(_rowIndex, v, _fileBuffer->constData(), bufstart, bufread, col, s, n, _lineending, column_del);
    }
  } else if (_config._columnType == AsciiSourceConfig::Whitespace) {
    MeasureTime t("AsciiSource::readField: whitespace separated columns");
    const AsciiDataReader::IsWhiteSpace column_del;
    return readColumns(_rowIndex, v, _fileBuffer->constData(), bufstart, bufread, col, s, n, _lineending, column_del);
  }

  return 0;
}


// vim: ts=2 sw=2 et
