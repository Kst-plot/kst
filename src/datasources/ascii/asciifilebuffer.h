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

#ifndef ASCII_FILE_BUFFER_H
#define ASCII_FILE_BUFFER_H

#include <QVarLengthArray>

class QFile;

class AsciiFileBuffer
{
public:

  enum SizeOnStack
  {
    Prealloc =
#if defined(__ANDROID__) || defined(__QNX__) // Some mobile systems really do not like you allocating 1MB on the stack.
    1 * 1024
#else
    1 * 1024 * 1024
#endif
  };

  typedef QVarLengthArray<char, Prealloc> Array;
  
  inline AsciiFileBuffer() : _begin(-10), _bytesRead(-10), _array(new Array) {}
  inline ~AsciiFileBuffer() { delete _array; }

  inline int begin() const { return _begin; }
  inline int bytesRead() const { return _bytesRead; }

  int read(QFile&, int start, int numberOfBytes, int maximalBytes = -1);

  inline char* data() { return _array->data(); }

  inline const char* const constPointer() const { return _array->data(); }
  inline const Array& constArray() const{ return *_array; }

  bool resize(int size);
  void clear(bool forceDeletingArray = false);

private:
  Array* _array;
  int _begin;
  int _bytesRead;
};


#endif
// vim: ts=2 sw=2 et
