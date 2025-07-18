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

#include "asciifiledata.h"

#include "debug.h"

#include <QFile>
#include <QDebug>
#include <QByteArray>

int MB = 1024*1024;

// Simulate out of memory scenario
//#define KST_TEST_OOM

#ifdef KST_TEST_OOM
size_t maxAllocate = 1 * MB;
#else
size_t maxAllocate = (size_t) -1;
#endif

#define KST_MEMORY_DEBUG if(0)

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
  if(sum / MB != 0) Kst::Debug::self()->log(QString("AsciiFileData: %1 MB used").arg(sum / MB), Kst::Debug::Warning);
  KST_MEMORY_DEBUG if(sum / MB != 0) qDebug() << "AsciiFileData: " << sum / MB<< "MB used";
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
    KST_MEMORY_DEBUG if(bytes / MB != 0) qDebug() << "AsciiFileBuffer: " << bytes / MB << "MB allocated";
    KST_MEMORY_DEBUG if(bytes / MB != 0) logMemoryUsed();
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
    KST_MEMORY_DEBUG if(allocatedMBs[ptr] / MB != 0) qDebug() << "AsciiFileData: " << allocatedMBs[ptr] / MB << "MB freed";
    allocatedMBs.remove(ptr);
  }
  KST_MEMORY_DEBUG logMemoryUsed();
  free(ptr);
}



//-------------------------------------------------------------------------------------------
AsciiFileData::AsciiFileData() :
  _array(new Array), _file(0), _fileRead(false), _reread(false),
  _begin(-1), _bytesRead(0), _rowBegin(-1), _rowsRead(0)
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
const char* AsciiFileData::constPointer() const
{
  return _array->data();
}

const AsciiFileData::Array& AsciiFileData::constArray() const
{
  return *_array;
}

//-------------------------------------------------------------------------------------------
bool AsciiFileData::resize(qint64 bytes)
{
  try {
    _array->resize(bytes);
  } catch (const std::bad_alloc&) {
    // work around Qt bug: Qt4 does not release the heap allocated memory
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
    _array = QSharedPointer<Array>(new Array);
  }
  _begin = -1;
  _bytesRead = 0;
  _fileRead = false;
}

//-------------------------------------------------------------------------------------------
qint64 AsciiFileData::read(QFile& file, qint64 start, qint64 bytesToRead, qint64 maximalBytes)
{
  _begin = -1;
  _bytesRead = 0;

  if (bytesToRead <= 0 || start < 0)
    return 0;

  if (maximalBytes == -1) {
    if (!resize(bytesToRead + 1))
      return 0;
  } else {
    bytesToRead = qMin(bytesToRead, maximalBytes);
    if (!resize(bytesToRead + 1))
      return 0;
  }
  if (!file.seek(start)) // expensive?
    return 0;
  qint64 bytesRead = file.read(_array->data(), bytesToRead);
  if (!resize(bytesRead + 1))
    return 0;

  _array->data()[bytesRead] = '\0';
  _begin = start;
  _bytesRead = bytesRead;

  return bytesRead;
}

//-------------------------------------------------------------------------------------------
bool AsciiFileData::read()
{
  if (_fileRead && !_reread) {
    return true;
  }

  if (!_file || _file->openMode() != QIODevice::ReadOnly) {
    return false;
  }

  qint64 start = _begin;
  qint64 bytesToRead = _bytesRead;
  read(*_file, start, bytesToRead);
  if (begin() != start || bytesRead() != bytesToRead) {
    clear(true);
    return false;
  }

  _fileRead = true;
  return true;
}

//-------------------------------------------------------------------------------------------
void AsciiFileData::logData() const
{
  QString this_str;
  QString data_str;

  this_str.asprintf("%p", this);
  data_str.asprintf("%p", _array.data());
  qDebug() << QString("AsciiFileData %1, array %2, byte %3 ... %4 (%8), row %5 ... %6 (%9), lazy: %7")
    .arg(this_str)
    .arg(data_str)
    .arg(begin(), 8).arg(begin() + bytesRead(), 8)
    .arg(rowBegin(), 8).arg(rowBegin() + rowsRead(), 8)
    .arg(_reread).arg(bytesRead(), 8).arg(rowsRead(), 8);
}

//-------------------------------------------------------------------------------------------
void AsciiFileData::logData(const QVector<AsciiFileData>& chunks)
{
  foreach (const AsciiFileData& chunk, chunks) {
    chunk.logData();
  }
}

//-------------------------------------------------------------------------------------------
void AsciiFileData::setSharedArray(AsciiFileData& arrayData)
{
  _array = arrayData._array;
}

