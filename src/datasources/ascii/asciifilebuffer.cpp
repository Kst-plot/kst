/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2012 Peter Kümmel                                     *
 *   email     : syntheticpp@gmx.net                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "asciifilebuffer.h"
#include "debug.h"

#include <QFile>
#include <QDebug>
#include <QVarLengthArray>


//-------------------------------------------------------------------------------------------
extern int MB;
extern size_t maxAllocate;

//-------------------------------------------------------------------------------------------
AsciiFileBuffer::AsciiFileBuffer()
{
}

//-------------------------------------------------------------------------------------------
AsciiFileBuffer::~AsciiFileBuffer()
{
  clear();
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::clear()
{
  foreach (AsciiFileData chunk, _fileData) {
    chunk.release();
  }
  _fileData.clear();
}

//-------------------------------------------------------------------------------------------
const QVector<AsciiFileData>& AsciiFileBuffer::data() const
{
  return _fileData;
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::logData() const
{
  int i = 0;
  foreach (const AsciiFileData& chunk, _fileData) {
    qDebug() << "_fileData: " << i << ". " << chunk.rowBegin() << " ... " << chunk.rowBegin() + chunk.rowsRead();
    i++;
  }
}

//-------------------------------------------------------------------------------------------
static int findRowOfPosition(const AsciiFileBuffer::RowIndex& rowIndex, int searchStart, int pos)
{
  //TODO too expensive?
  const int size = rowIndex.size();
  for (int row = searchStart; row != size; row++) {
    if (rowIndex[row] > pos)
      return row - 1;
  }
  // must be the last row
  return size - 1;
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::read(QFile& file, const RowIndex& rowIndex, int start, int bytesToRead, int maximalBytes)
{
  _begin = -1;
  _bytesRead = 0;
  _fileData.clear();

  // first try to read the whole file into one array
  AsciiFileData wholeFile;
  wholeFile.read(file, start, bytesToRead, maximalBytes);
  if (bytesToRead == wholeFile.bytesRead()) {
    wholeFile.setRowBegin(0);
    wholeFile.setRowsRead(rowIndex.size());
    _begin = start;
    _bytesRead = bytesToRead;
    _fileData << wholeFile;
    return;
  } else {
    wholeFile.release();
  }

  // reading whole file into one array failed, try to read into smaller arrays
  int chunkSize = qMin((size_t) 10 * MB, maxAllocate);
  int end = start + bytesToRead;
  int chunkRead = 0;
  int row = 0;
  for (int pos = start; pos < end; pos += chunkRead) {
    AsciiFileData chunk;
    // remember first row index
    chunk.setRowBegin(row);
    // read complete chunk or to end of file
    chunkRead = (pos + chunkSize < end ? chunkSize : end - pos);  
    // adjust to row end: pos + chunkRead is in the middle of a row, find index of this row
    row = findRowOfPosition(rowIndex, row, pos + chunkRead);
    // read until the beginning of this row
    chunkRead = (rowIndex[row] - 1);
    // check if it is the last row, and read remaining bytes from pos
    chunkRead = (row == rowIndex.size() - 1) ? end - pos : chunkRead - pos;
    // read the rows
    chunk.read(file, pos, chunkRead);
    if (chunkRead != chunk.bytesRead()) {
      Kst::Debug::self()->log(QString("AsciiFileBuffer: error when reading into chunk"));
      chunk.release();
      break;
    }
    // remember number of read rows
    chunk.setRowsRead(row - chunk.rowBegin());
    _fileData << chunk;
    _bytesRead += chunk.bytesRead();
  }
  if (_bytesRead == bytesToRead) {
    _begin = start;
    return;
  } else {
    _bytesRead = 0;
    _fileData.clear();
    Kst::Debug::self()->log(QString("AsciiFileBuffer: error while reading %1 chunks").arg(_fileData.size()));
  }

  // sliding window
  // TODO
}



