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
  _file(0), _begin(-1), _bytesRead(0)
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
  if (chunkSize == 0)
    return chunks;
  chunks.reserve(bytesToRead / chunkSize);
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
  //qDebug() << "File splitted into " << chunks.size() << " chunks:"; AsciiFileData::logData(chunks);
  return chunks;
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::useOneWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int numChunks)
{
  useSlidingWindowWithChunks(rowIndex, start, bytesToRead, bytesToRead, numChunks, false);
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::useSlidingWindow(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize)
{
  useSlidingWindowWithChunks(rowIndex, start, bytesToRead, windowSize, 1, true);
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::useSlidingWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize, int numWindowChunks)
{
  useSlidingWindowWithChunks(rowIndex, start, bytesToRead, windowSize, numWindowChunks, true);
}

//-------------------------------------------------------------------------------------------
void AsciiFileBuffer::useSlidingWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize, int numWindowChunks, bool reread)
{
  clear();
  if (!_file)
    return;

  if (bytesToRead == 0 && numWindowChunks == 0 || windowSize == 0)
    return;

  int chunkSize = windowSize / numWindowChunks;
  QVector<AsciiFileData> chunks = splitFile(chunkSize, rowIndex, start, bytesToRead);
  // chunks.size() could be greater than numWindowChunks!

  // no sliding window
  if (bytesToRead == windowSize)
  {
    for (int i = 0; i < chunks.size(); i++) {
      AsciiFileData& chunk = chunks[i];
      if (!chunk.resize(chunk.bytesRead())) {
        Kst::Debug::self()->log(QString("AsciiFileBuffer: not enough memory available to read whole file"));
        return;
      }
      chunk.setFile(_file);
      chunk.setReread(reread);
      _bytesRead += chunk.bytesRead();
    }
    _fileData.push_back(chunks);
  }
  else
  {
    // sliding window
    // prepare window with numSubChunks chunks
    QVector<AsciiFileData> window;
    window.reserve(numWindowChunks);
    for (int i = 0; i < numWindowChunks; i++) {
      AsciiFileData sharedArray;
      if (!sharedArray.resize(chunkSize)) {
        Kst::Debug::self()->log(QString("AsciiFileBuffer: not enough memory available for sliding window"));
        return;
      }
      sharedArray.setFile(_file);
      window.push_back(sharedArray);
    }

    _fileData.reserve(bytesToRead / windowSize);
    int i = 0;
    while (i < chunks.size()) {
      QVector<AsciiFileData> windowChunks;
      windowChunks.reserve(window.size());
      for (int s = 0; s < window.size(); s++) {
        AsciiFileData chunk = chunks[i];
        chunk.setSharedArray(window[s]);
        chunk.setFile(_file);
        chunk.setReread(reread);
        _bytesRead += chunk.bytesRead();
        windowChunks.push_back(chunk);
        i++;
        if (i >= chunks.size())
          break;
      }
      // each entry is one slide of the window
      _fileData.push_back(windowChunks);
      qDebug() << "Window chunks:"; AsciiFileData::logData(windowChunks);
    }
  }

  _begin = start;
  if (_bytesRead != bytesToRead) {
    clear();
    Kst::Debug::self()->log(QString("AsciiFileBuffer: error while splitting into file %1 chunks").arg(_fileData.size()));
  }
}

//-------------------------------------------------------------------------------------------
bool AsciiFileBuffer::readWindow(QVector<AsciiFileData>& window) const
{
  for (int i = 0; i < window.size(); i++) {
    if (!window[i].read()) {
      return false;
    }
  }
  return true;
}

