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



// vim: ts=2 sw=2 et
