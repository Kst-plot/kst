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
  foreach (const AsciiFileData& chunk, _fileData) {
    chunk.logData();
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
const QVector<AsciiFileData> AsciiFileBuffer::splitFile(int chunkSize, const RowIndex& rowIndex, int start, int bytesToRead) const
{
  // reading whole file into one array failed, try to read into smaller arrays
  const int end = start + bytesToRead;
  int chunkRead = 0;
  int lastRow = 0;
  QVector<AsciiFileData> chunks;
  for (int pos = start; pos < end; pos += chunkRead) {
    // use for storing reading information only
    AsciiFileData chunk;
    // read complete chunk or to end of file
    chunkRead = (pos + chunkSize < end ? chunkSize : end - pos);
    // adjust to row end: pos + chunkRead is in the middle of a row, find index of this row
    const int rowBegin = lastRow;
    lastRow = findRowOfPosition(rowIndex, lastRow, pos + chunkRead);
    // read until the beginning of this row
    chunkRead = (rowIndex[lastRow] - 1);
    // check if it is the last row, and read remaining bytes from pos
    chunkRead = (lastRow == rowIndex.size() - 1) ? end - pos : chunkRead - pos;
    // set information about positions in the file
    chunk.setBegin(pos);
    chunk.setBytesRead(chunkRead);
    // set information about rows
    chunk.setRowBegin(rowBegin);
    chunk.setRowsRead(lastRow - rowBegin);
    chunks << chunk;
  }
  //qDebug() << "File splitted into " << chunks.size() << " chunks:"; logData();
  return chunks;
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
  _fileData = splitFile(chunkSize, rowIndex, start, bytesToRead);
  _bytesRead = 0;
  foreach (AsciiFileData chunk, _fileData) {
    // use alread set
    if (!chunk.lazyRead(file)) {
      Kst::Debug::self()->log(QString("AsciiFileBuffer: error when reading into chunk"));
      chunk.release();
      break;
    }
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



