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
#if defined(__ANDROID__) || defined(__QNX__) || defined(KST_SMALL_PRREALLOC)
    // Some mobile systems really do not like you allocating 1MB on the stack.
    1 * 1024
#else
    1 * 1024 * 1024
#endif
  };

  typedef QVarLengthArray<char, Prealloc> Array;
  
  AsciiFileData();
  ~AsciiFileData();

  inline bool reread() const { return _reread; }
  inline void setReread(bool value) { _reread = value; }

  inline qint64 begin() const { return _begin; }
  inline qint64 bytesRead() const { return _bytesRead; }
  inline void setBegin(qint64 begin) { _begin = begin; }
  inline void setBytesRead(qint64 read) { _bytesRead = read; }

  inline void setFile(QFile* file) { _file = file; }
  bool read();
  void read(QFile&, qint64 start, qint64 numberOfBytes, qint64 maximalBytes = -1);

  char* data();
  const char* const constPointer() const;
  const Array& constArray() const;

  bool resize(qint64 size);
  void clear(bool forceDeletingArray = false);

  inline qint64 rowBegin() const { return _rowBegin; }
  inline qint64 rowsRead() const { return _rowsRead; }
  inline void setRowBegin(qint64 begin) { _rowBegin = begin; }
  inline void setRowsRead(qint64 read) { _rowsRead = read; }

  void setSharedArray(AsciiFileData&);


  void logData() const;
  static void logData(const QVector<AsciiFileData>& chunks);

private:
  QSharedPointer<Array> _array;
  QFile* _file;
  bool _fileRead;
  bool _reread;
  qint64 _begin;
  qint64 _bytesRead;
  qint64 _rowBegin;
  qint64 _rowsRead;

};

Q_DECLARE_TYPEINFO(AsciiFileData, Q_MOVABLE_TYPE);

#endif
// vim: ts=2 sw=2 et
