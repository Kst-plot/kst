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


//-------------------------------------------------------------------------------------------
bool AsciiFileBuffer::resize(int bytes)
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
void AsciiFileBuffer::clearFileBuffer(bool forceDelete)
{
  // force deletion of heap allocated memory if any
  if (forceDelete || _array->capacity() > Prealloc) {
    delete _array;
    _array = new Array;
  }
  _bufferedS = -10;
  _bufferedN = -10;
}

