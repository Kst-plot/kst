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
  void readWholeFile(const RowIndex& rowIndex, int start, int bytesToRead, int numChunks, int maximalBytes = -1);
  const QVector<AsciiFileData>& data() const; // -> wholeFile();
  
  void readFileSlidingWindow(const RowIndex& rowIndex, int start, int bytesToRead, int chunkSize);
  void readFileSlidingWindow(const RowIndex& rowIndex, int start, int bytesToRead, int chunkSize, int numSubChunks);

  const QVector<QVector<AsciiFileData> >& slidingWindow() const { return _slidingWindow; }

  static bool openFile(QFile &file);

private:
  QFile* _file;
  QVector<AsciiFileData> _fileData;
  QVector<QVector<AsciiFileData> > _slidingWindow;

  int _begin;
  int _bytesRead;
  const int _defaultChunkSize;

  const QVector<AsciiFileData> splitFile(int chunkSize, const RowIndex& rowIndex, int start, int bytesToRead) const;
  int findRowOfPosition(const AsciiFileBuffer::RowIndex& rowIndex, int searchStart, int pos) const;
};

#endif
// vim: ts=2 sw=2 et
