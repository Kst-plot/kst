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

#include <QFile>


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

