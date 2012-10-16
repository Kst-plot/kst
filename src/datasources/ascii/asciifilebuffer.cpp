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
#include <QMap>

static int MB = 1024*1024;

// Simulate out of memory scenario
//#define KST_TEST_OOM
#ifdef KST_TEST_OOM
static const size_t maxMB = 50;
#else
static const size_t maxMB = 0;
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
  Kst::Debug::self()->log(QString("AsciiFileBuffer: %1 MB used").arg(sum/MB), Kst::Debug::Warning);
  KST_MEMORY_DEBUG qDebug() << "AsciiFileBuffer: " << sum/MB<< "MB used";
}

//-------------------------------------------------------------------------------------------
void* fileBufferMalloc(size_t bytes)
{
  void* ptr = 0;
  if (maxMB == 0 || bytes < maxMB*MB) {
    ptr = malloc(bytes);
  }
  if (ptr)  {
    allocatedMBs[ptr] = bytes;
    KST_MEMORY_DEBUG qDebug() << "AsciiFileBuffer: " << bytes/MB << "MB allocated";
    KST_MEMORY_DEBUG logMemoryUsed();
  } else {
    Kst::Debug::self()->log(QString("AsciiFileBuffer: failed to allocate %1 MBs").arg(bytes/MB), Kst::Debug::Warning);
    logMemoryUsed();
    KST_MEMORY_DEBUG qDebug() << "AsciiFileBuffer: error when allocating " << bytes/MB << "MB";
  }
  return ptr;
}

//-------------------------------------------------------------------------------------------
void fileBufferFree(void* ptr)
{
  if (allocatedMBs.contains(ptr)) {
    KST_MEMORY_DEBUG qDebug() << "AsciiFileBuffer: " << allocatedMBs[ptr]/MB << "MB freed";
    allocatedMBs.remove(ptr);
  }
  KST_MEMORY_DEBUG logMemoryUsed();
  free(ptr);
}

//-------------------------------------------------------------------------------------------
AsciiFileBuffer::AsciiFileBuffer() : _array(new Array), _begin(-1), _bytesRead(0)
{
}

//-------------------------------------------------------------------------------------------
AsciiFileBuffer::~AsciiFileBuffer()
{
  delete _array;
}


//-------------------------------------------------------------------------------------------
char* AsciiFileBuffer::data()
{
  return _array->data();
}

//-------------------------------------------------------------------------------------------
const char* const AsciiFileBuffer::constPointer() const
{
  return _array->data();
}

//-------------------------------------------------------------------------------------------
bool AsciiFileBuffer::resize(int bytes)
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
void AsciiFileBuffer::clear(bool forceDeletingArray)
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
void AsciiFileBuffer::read(QFile& file, int start, int bytesToRead, int maximalBytes)
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

