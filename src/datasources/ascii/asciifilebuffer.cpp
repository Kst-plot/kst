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

// needed to track memeory usage
#include "qplatformdefs.h"
#include <stdlib.h>
void* fileBufferMalloc(size_t bytes);
void fileBufferFree(void* ptr);
#define malloc fileBufferMalloc
#define qMalloc fileBufferMalloc
#define free fileBufferFree
#define qFree fileBufferFree
#include <QVarLengthArray>
#undef malloc
#undef qMalloc
#undef free
#undef qFree

#include "asciifilebuffer.h"
#include "debug.h"

#include <QFile>
#include <QDebug>


static int MB = 1024*1024;

// Simulate out of memory scenario
//#define KST_TEST_OOM

#ifdef KST_TEST_OOM
static size_t maxAllocate = 2 * MB;
#else
static size_t maxAllocate = (size_t) -1;
#endif

#define KST_MEMORY_DEBUG if(1)

//-------------------------------------------------------------------------------------------
static QMap<void*, size_t> allocatedMBs;

//-------------------------------------------------------------------------------------------
static void logMemoryUsed()
{
  size_t sum = 0;
  QMapIterator<void*, size_t> it(allocatedMBs);
  while (it.hasNext()) {
    it.next();
    sum +=  it.value();
  }
  Kst::Debug::self()->log(QString("AsciiFileData: %1 MB used").arg(sum / MB), Kst::Debug::Warning);
  KST_MEMORY_DEBUG qDebug() << "AsciiFileData: " << sum / MB<< "MB used";
}

//-------------------------------------------------------------------------------------------
void* fileBufferMalloc(size_t bytes)
{
  void* ptr = 0;
#ifdef KST_TEST_OOM
  if (bytes <= maxAllocate)
#endif
    ptr = malloc(bytes);
  if (ptr)  {
    allocatedMBs[ptr] = bytes;
    KST_MEMORY_DEBUG qDebug() << "AsciiFileBuffer: " << bytes / MB << "MB allocated";
    KST_MEMORY_DEBUG logMemoryUsed();
  } else {
    Kst::Debug::self()->log(QString("AsciiFileData: failed to allocate %1 MBs").arg(bytes / MB), Kst::Debug::Warning);
    logMemoryUsed();
    KST_MEMORY_DEBUG qDebug() << "AsciiFileData: error when allocating " << bytes / MB << "MB";
  }
  return ptr;
}

//-------------------------------------------------------------------------------------------
void fileBufferFree(void* ptr)
{
  if (allocatedMBs.contains(ptr)) {
    KST_MEMORY_DEBUG qDebug() << "AsciiFileData: " << allocatedMBs[ptr] / MB << "MB freed";
    allocatedMBs.remove(ptr);
  }
  KST_MEMORY_DEBUG logMemoryUsed();
  free(ptr);
}

//-------------------------------------------------------------------------------------------
AsciiFileData::AsciiFileData() : _array(new Array), _begin(-1), _bytesRead(0), _rowBegin(-1), _rowsRead(0)
{
}

//-------------------------------------------------------------------------------------------
AsciiFileData::~AsciiFileData()
{
}

//-------------------------------------------------------------------------------------------
char* AsciiFileData::data()
{
  return _array->data();
}

//-------------------------------------------------------------------------------------------
const char* const AsciiFileData::constPointer() const
{
  return _array->data();
}

//-------------------------------------------------------------------------------------------
bool AsciiFileData::resize(int bytes)
{ 
  try {
    _array->resize(bytes);
  } catch (const std::bad_alloc&) {
    // work around Qt bug
    clear(true);
    return false;
  }
  return true;
}

//-------------------------------------------------------------------------------------------
void AsciiFileData::clear(bool forceDeletingArray)
{
  // force deletion of heap allocated memory if any
  if (forceDeletingArray || _array->capacity() > Prealloc) {
    delete _array;
    _array = new Array;
  }
  _begin = -1;
  _bytesRead = 0;
}

//-------------------------------------------------------------------------------------------
void AsciiFileData::release()
{
  delete _array;
  _array = 0;
  _begin = -1;
  _bytesRead = 0;
}

//-------------------------------------------------------------------------------------------
void AsciiFileData::read(QFile& file, int start, int bytesToRead, int maximalBytes)
{
  _begin = -1;
  _bytesRead = 0;

  if (bytesToRead <= 0)
    return;

  if (maximalBytes == -1) {
    if (!resize(bytesToRead + 1))
      return;
  } else {
    bytesToRead = qMin(bytesToRead, maximalBytes);
    if (!resize(bytesToRead + 1))
      return;
  }
  file.seek(start); // expensive?
  int bytesRead = file.read(data(), bytesToRead);
  if (!resize(bytesRead + 1))
    return;

  data()[bytesRead] = '\0';
  _begin = start;
  _bytesRead = bytesRead;
}


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
void AsciiFileBuffer::clear(bool forceDeletingArray)
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
  //TODO too expensive
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



