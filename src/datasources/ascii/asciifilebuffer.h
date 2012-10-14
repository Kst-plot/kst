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
  
  inline AsciiFileBuffer() : _bufferedS(-10), _bufferedN(-10), _array(new Array) {}
  inline ~AsciiFileBuffer() { delete _array; }
      
  int _bufferedS;
  int _bufferedN;

  inline void clear() { _array->clear(); }
  inline int size() const { return _array->size(); }
  inline int  capacity() const { return _array->capacity(); }
  inline char* data() { return _array->data(); }

  inline const char* const constPointer() const { return _array->data(); }
  inline const Array& constArray() const{ return *_array; }

  bool resize(int size);
  void clearFileBuffer(bool forceDelete = false);

private:

  Array* _array;
};


#endif
// vim: ts=2 sw=2 et
