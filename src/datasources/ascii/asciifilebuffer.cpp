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
AsciiFileBuffer::AsciiFileBuffer() : 
  _file(0), _begin(-1), _bytesRead(0),
  _defaultChunkSize(qMin((size_t) 10 * MB, maxAllocate - 1))
{
}

//-------------------------------------------------------------------------------------------
AsciiFileBuffer::~AsciiFileBuffer()
{
  clear();
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::setFile(QFile* file)
{
  delete _file;
  _file = file; 
}

//-------------------------------------------------------------------------------------------
bool AsciiFileBuffer::openFile(QFile &file) 
{
  // Don't use 'QIODevice::Text'!
  // Because CR LF line ending breaks row offset calculation
  return file.open(QIODevice::ReadOnly);
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::clear()
{
  _fileData.clear();
  _begin = -1;
  _bytesRead = 0;
}

//-------------------------------------------------------------------------------------------
const QVector<AsciiFileData>& AsciiFileBuffer::data() const
{
  return _fileData;
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::logData(const QVector<AsciiFileData>& chunks) const
{
  foreach (const AsciiFileData& chunk, chunks) {
    chunk.logData();
  }
}

//-------------------------------------------------------------------------------------------
int AsciiFileBuffer::findRowOfPosition(const AsciiFileBuffer::RowIndex& rowIndex, int searchStart, int pos) const
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
  int endsInRow = 0;
  QVector<AsciiFileData> chunks;
  int pos = start;
  while (pos < end) {
    // use for storing reading information only
    AsciiFileData chunk;
    // read complete chunk or to end of file
    int endRead = (pos + chunkSize < end ? pos + chunkSize : end);
    // adjust to row end: pos + chunkRead is in the middle of a row, find index of this row
    const int rowBegin = endsInRow;
    endsInRow = findRowOfPosition(rowIndex, endsInRow, endRead);
    // read until the beginning of this row
    endRead = rowIndex[endsInRow];
    // check if it is the last row, and read remaining bytes from pos
    if (endsInRow == rowIndex.size() - 1)
      endRead = end;
    // set information about positions in the file
    chunk.setBegin(pos);
    chunk.setBytesRead(endRead - pos);
    // set information about rows
    chunk.setRowBegin(rowBegin);
    chunk.setRowsRead(endsInRow - rowBegin);
    chunks << chunk;
    if (endsInRow ==  rowIndex.size() - 1)
      break;
    pos = rowIndex[endsInRow];
  }
  //qDebug() << "File splitted into " << chunks.size() << " chunks:"; logData(chunks);
  return chunks;
}


//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::readWholeFile(const RowIndex& rowIndex, int start, int bytesToRead, int maximalBytes)
{
  clear();
  if (!_file)
    return;

  // first try to read the whole file into one array
  AsciiFileData wholeFile;
  wholeFile.read(*_file, start, bytesToRead, maximalBytes);
  if (bytesToRead == wholeFile.bytesRead()) {
    wholeFile.setRowBegin(0);
    wholeFile.setRowsRead(rowIndex.size() - 1);
    _begin = start;
    _bytesRead = bytesToRead;
    _fileData << wholeFile;
    return;
  }

  // reading whole file into one array failed, try to read into smaller arrays
  int chunkSize = _defaultChunkSize;
  _fileData = splitFile(chunkSize, rowIndex, start, bytesToRead);
  for (int i = 0; i < _fileData.size(); i++) {
    // use alread set
    _fileData[i].setFile(_file);
    if (!_fileData[i].read()) {
      Kst::Debug::self()->log(QString("AsciiFileBuffer: error when reading into chunk"));
      break;
    }
    _bytesRead += _fileData[i].bytesRead();
  }
  if (_bytesRead == bytesToRead) {
    _begin = start;
  } else {
    clear();
    Kst::Debug::self()->log(QString("AsciiFileBuffer: error while reading %1 chunks").arg(_fileData.size()));
  }
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::readFileSlidingWindow(const RowIndex& rowIndex, int start, int bytesToRead, int maximalBytes)
{
  clear();
  if (!_file)
    return;

  int chunkSize = _defaultChunkSize;
  _fileData = splitFile(chunkSize, rowIndex, start, bytesToRead);
  _bytesRead = 0;
  AsciiFileData master;
  if (!master.resize(chunkSize)) {
    Kst::Debug::self()->log(QString("AsciiFileBuffer: not enough memory available for creating sliding window"));
  }
  for (int i = 0; i < _fileData.size(); i++) {
    // reading from file is delayed until the data is accessed
    _fileData[i].setLazyRead(true);
    _fileData[i].setFile(_file);
    _fileData[i].setSharedArray(master);
  }
  _begin = start;
  _bytesRead = bytesToRead;
}



