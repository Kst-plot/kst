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
  
  typedef QVarLengthArray<int, AsciiFileData::Prealloc> RowIndex;

  inline int begin() const { return _begin; }
  inline int bytesRead() const { return _bytesRead; }
  
  void clear();

  void setFile(QFile* file);
  bool readWindow(QVector<AsciiFileData>& window) const;

  void useOneWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int numChunks);
  void useSlidingWindow(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize);
  void useSlidingWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize, int numWindowChunks);

  QVector<QVector<AsciiFileData> >& fileData() { return _fileData; }

  static bool openFile(QFile &file);

private:
  QFile* _file;
  QVector<QVector<AsciiFileData> > _fileData;

  int _begin;
  int _bytesRead;

  const QVector<AsciiFileData> splitFile(int chunkSize, const RowIndex& rowIndex, int start, int bytesToRead) const;
  int findRowOfPosition(const AsciiFileBuffer::RowIndex& rowIndex, int searchStart, int pos) const;
  void useSlidingWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize, int numWindowChunks, bool reread);

  friend class AsciiSourceTest;
};

#endif
// vim: ts=2 sw=2 et
