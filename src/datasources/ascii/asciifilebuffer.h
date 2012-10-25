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

#include "asciifiledata.h"

#include <QVector>
#include <stdlib.h>

class AsciiFileBuffer
{
public:
  AsciiFileBuffer();
  ~AsciiFileBuffer();
  
  typedef QVarLengthArray<qint64, AsciiFileData::Prealloc> RowIndex;

  inline qint64 begin() const { return _begin; }
  inline qint64 bytesRead() const { return _bytesRead; }
  
  void clear();

  void setFile(QFile* file);
  bool readWindow(QVector<AsciiFileData>& window) const;

  void useOneWindowWithChunks(const RowIndex& rowIndex, qint64 start, qint64 bytesToRead, int numChunks);
  void useSlidingWindow(const RowIndex& rowIndex, qint64 start, qint64 bytesToRead, qint64 windowSize);
  void useSlidingWindowWithChunks(const RowIndex& rowIndex, qint64 start, qint64 bytesToRead, qint64 windowSize, int numWindowChunks);

  QVector<QVector<AsciiFileData> >& fileData() { return _fileData; }

  static bool openFile(QFile &file);

private:
  QFile* _file;
  QVector<QVector<AsciiFileData> > _fileData;

  qint64 _begin;
  qint64 _bytesRead;

  const QVector<AsciiFileData> splitFile(qint64 chunkSize, const RowIndex& rowIndex, qint64 start, qint64 bytesToRead) const;
  qint64 findRowOfPosition(const AsciiFileBuffer::RowIndex& rowIndex, qint64 searchStart, qint64 pos) const;
  void useSlidingWindowWithChunks(const RowIndex& rowIndex, qint64 start, qint64 bytesToRead, qint64 windowSize, int numWindowChunks, bool reread);

  friend class AsciiSourceTest;
};

#endif
// vim: ts=2 sw=2 et
