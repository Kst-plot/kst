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

#ifndef ASCII_FILE_DATA_H
#define ASCII_FILE_DATA_H

#include <QVector>
#include <QSharedPointer>

class QFile;
template<class T, int Prealloc>
class QVarLengthArray;


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

  inline bool lazyRead() const { return _lazyRead; }
  inline void setLazyRead(bool value) { _lazyRead = value; }

  inline int begin() const { return _begin; }
  inline int bytesRead() const { return _bytesRead; }
  inline void setBegin(int begin) { _begin = begin; }
  inline void setBytesRead(int read) { _bytesRead = read; }

  inline void setFile(QFile* file) { _file = file; }
  bool read();
  void read(QFile&, int start, int numberOfBytes, int maximalBytes = -1);

  char* data();
  const char* const constPointer() const;
  const Array& constArray() const;

  bool resize(int size);
  void clear(bool forceDeletingArray = false);

  inline int rowBegin() const { return _rowBegin; }
  inline int rowsRead() const { return _rowsRead; }
  inline void setRowBegin(int begin) { _rowBegin = begin; }
  inline void setRowsRead(int read) { _rowsRead = read; }

  void setSharedArray(AsciiFileData&);


  void logData() const;
  static void logData(const QVector<AsciiFileData>& chunks);

private:
  QSharedPointer<Array> _array;
  QFile* _file;
  bool _lazyRead;
  int _begin;
  int _bytesRead;
  int _rowBegin;
  int _rowsRead;

  void readLazy() const;
};

Q_DECLARE_TYPEINFO(AsciiFileData, Q_MOVABLE_TYPE);

#endif
// vim: ts=2 sw=2 et
