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

#include <QVector>

template<class T, int Prealloc>
class QVarLengthArray;

class QFile;

class AsciiFileData
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
  
  AsciiFileData();
  ~AsciiFileData();

  inline int begin() const { return _begin; }
  inline int bytesRead() const { return _bytesRead; }
  void read(QFile&, int start, int numberOfBytes, int maximalBytes = -1);
  char* data();

  const char* const constPointer() const;
  inline const Array& constArray() const { return *_array; }

  bool resize(int size);
  void clear(bool forceDeletingArray = false);
  void release();

  inline int rowBegin() const { return _rowBegin; }
  inline int rowsRead() const { return _rowsRead; }
  inline void setRowBegin(int begin) { _rowBegin = begin; }
  inline void setRowsRead(int read) { _rowsRead = read; }

private:
  Array* _array;
  int _begin;
  int _bytesRead;
  int _rowBegin;
  int _rowsRead;
};

Q_DECLARE_TYPEINFO(AsciiFileData, Q_MOVABLE_TYPE);


class AsciiFileBuffer
{
public:
  AsciiFileBuffer();
  ~AsciiFileBuffer();
  
  typedef QVarLengthArray<int, AsciiFileData::Prealloc> RowIndex;

  inline int begin() const { return _begin; }
  inline int bytesRead() const { return _bytesRead; }

  void clear(bool forceDeletingArray = false);
  
  void read(QFile&, const RowIndex& rowIndex, int start, int numberOfBytes, int maximalBytes = -1);
  
  const QVector<AsciiFileData>& data() const;
  
private:
  QVector<AsciiFileData> _fileData;
  int _begin;
  int _bytesRead;
  void logData() const;
};

#endif
// vim: ts=2 sw=2 et
