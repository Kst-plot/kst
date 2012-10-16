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
  void read(const RowIndex& rowIndex, int start, int numberOfBytes, int maximalBytes = -1);

  const QVector<AsciiFileData>& data() const;

  static bool openFile(QFile &file);
  
private:
  QFile* _file;
  QVector<AsciiFileData> _fileData;
  int _begin;
  int _bytesRead;

  void logData(const QVector<AsciiFileData>& chunks) const;
  const QVector<AsciiFileData> splitFile(int chunkSize, const RowIndex& rowIndex, int start, int bytesToRead) const;
  void readWholeFile(const RowIndex& rowIndex, int start, int bytesToRead, int maximalBytes);
  void readFileSlidingWindow(const RowIndex& rowIndex, int start, int bytesToRead, int maximalBytes);
};

#endif
// vim: ts=2 sw=2 et
