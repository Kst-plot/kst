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

#define KST_SMALL_PRREALLOC

#include "asciifilebuffer.h"

#include <QtTest>


template<>
bool QTest::qCompare<qint64, qint32>(qint64 const &t1, qint32 const &t2, const char *actual, const char *expected, const char *file, int line)
{
    return qCompare<qint64>(t1, qint64(t2), actual, expected, file, line);
}



class AsciiSourceTest: public QObject
{
    Q_OBJECT
    
public:
    
    AsciiSourceTest()
    {
    }
   
private slots:

    void initTestcase()
    {
      buf.setFile(&file);
    }


    // int findRowOfPosition(const AsciiFileBuffer::RowIndex& rowIndex, int searchStart, int pos) const

    void find_wrong_parameters()
    {
      int rows = 3;
      int rowLength = 100;
      initRowIndex(rows, rowLength);
      QCOMPARE(buf.findRowOfPosition(idx, 0, -1), -1);
      QCOMPARE(buf.findRowOfPosition(AsciiFileBuffer::RowIndex(), 0, 1), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 5,  1), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 1, 99), -1);
    }


    void find_small()
    {
      int rows = 1;
      int rowLength = 1;
      initRowIndex(rows, rowLength);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  0), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  1), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  2), -1);

      rows = 2;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  0), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  1), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  2), -1);

      rows = 3;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  0), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  1), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  2), 2);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  3), -1);

      rows = 1;
      rowLength = 2;
      initRowIndex(rows, rowLength);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  0), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  1), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  2), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  3), -1);
    }


    void find_start_0()
    {
      int rows = 1;
      int rowLength = 100;
      initRowIndex(rows, rowLength);

      QCOMPARE(buf.findRowOfPosition(idx, 0,   0), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,   1), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  99), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 100), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 101), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 199), -1);

      rows = 2;
      rowLength = 100;
      initRowIndex(rows, rowLength);

      QCOMPARE(buf.findRowOfPosition(idx, 0,   0), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,   1), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  99), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 100), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 101), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 199), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 200), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 201), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 299), -1);

      rows = 3;
      rowLength = 100;
      initRowIndex(rows, rowLength);

      QCOMPARE(buf.findRowOfPosition(idx, 0,   0), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,   1), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0,  99), 0);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 100), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 101), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 199), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 200), 2);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 201), 2);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 299), 2);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 300), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 301), -1);
      QCOMPARE(buf.findRowOfPosition(idx, 0, 399), -1);
    }


    void find_start_n()
    {
      int rows = 3;
      int rowLength = 100;
      initRowIndex(rows, rowLength);

      QCOMPARE(buf.findRowOfPosition(idx, 0, 100), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 1, 100), 1);

      QCOMPARE(buf.findRowOfPosition(idx, 0, 101), 1);
      QCOMPARE(buf.findRowOfPosition(idx, 1, 101), 1);

    }


    // const QVector<AsciiFileData> splitFile(int chunkSize, const RowIndex& rowIndex, int start, int bytesToRead) const;

    void split_wrong_parameters()
    {
      int rows = 1;
      int rowLength = 1;
      initRowIndex(rows, rowLength);
      QCOMPARE(buf.splitFile(1, AsciiFileBuffer::RowIndex(), 0, 1).size(), 0);
      QCOMPARE(buf.splitFile(-1, idx, 0, 1).size(), 0);
      QCOMPARE(buf.splitFile(1, idx, -1, 1).size(), 0);
      QCOMPARE(buf.splitFile(1, idx, -1, 1).size(), 0);
      QCOMPARE(buf.splitFile(1, idx, 10, 1).size(), 0);
      QCOMPARE(buf.splitFile(1, idx,  0, 2).size(), 0);
    }


    void split_small()
    {
      int rows = 1;
      int rowLength = 1;
      initRowIndex(rows, rowLength);
      QVector<AsciiFileData> c = buf.splitFile(1, idx, 0, 1);
      QCOMPARE(c.size(), 1);
      QCOMPARE(c[0].begin(), 0);
      QCOMPARE(c[0].bytesRead(), 1);
      QCOMPARE(c[0].rowBegin(), 0);
      QCOMPARE(c[0].rowsRead(), 1);

      rows = 2;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      c = buf.splitFile(1, idx, 0, 2);
      QCOMPARE(c.size(), 2);
      QCOMPARE(c[0].begin(), 0);
      QCOMPARE(c[0].bytesRead(), 1);
      QCOMPARE(c[0].rowBegin(), 0);
      QCOMPARE(c[0].rowsRead(), 1);
      QCOMPARE(c[1].begin(), 1);
      QCOMPARE(c[1].bytesRead(), 1);
      QCOMPARE(c[1].rowBegin(), 1);
      QCOMPARE(c[1].rowsRead(), 1);

      rows = 3;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      c = buf.splitFile(1, idx, 0, 3);
      QCOMPARE(c.size(), 3);
      QCOMPARE(c[2].begin(), 2);
      QCOMPARE(c[2].bytesRead(), 1);
      QCOMPARE(c[2].rowBegin(), 2);
      QCOMPARE(c[2].rowsRead(), 1);


      rows = 1;
      rowLength = 2;
      initRowIndex(rows, rowLength);
      c = buf.splitFile(2, idx, 0, 2);
      QCOMPARE(c.size(), 1);
      QCOMPARE(c[0].begin(), 0);
      QCOMPARE(c[0].bytesRead(), 2);
      QCOMPARE(c[0].rowBegin(), 0);
      QCOMPARE(c[0].rowsRead(), 1);

      rows = 2;
      rowLength = 2;
      initRowIndex(rows, rowLength);
      c = buf.splitFile(2, idx, 0, 4);
      QCOMPARE(c.size(), 2);
      QCOMPARE(c[0].begin(), 0);
      QCOMPARE(c[0].bytesRead(), 2);
      QCOMPARE(c[0].rowBegin(), 0);
      QCOMPARE(c[0].rowsRead(), 1);
      QCOMPARE(c[1].begin(), 2);
      QCOMPARE(c[1].bytesRead(), 2);
      QCOMPARE(c[1].rowBegin(), 1);
      QCOMPARE(c[1].rowsRead(), 1);

      // chunk ends in the middle of a row
      rows = 2;
      rowLength = 3;
      initRowIndex(rows, rowLength);
      c = buf.splitFile(5, idx, 0, 6);
      QCOMPARE(c.size(), 2);
      QCOMPARE(c[0].begin(), 0);
      QCOMPARE(c[0].bytesRead(), 3);
      QCOMPARE(c[0].rowBegin(), 0);
      QCOMPARE(c[0].rowsRead(), 1);
      QCOMPARE(c[1].begin(), 3);
      QCOMPARE(c[1].bytesRead(), 3);
      QCOMPARE(c[1].rowBegin(), 1);
      QCOMPARE(c[1].rowsRead(), 1);

      rows = 1;
      rowLength = 2;
      initRowIndex(rows, rowLength);
      c = buf.splitFile(1, idx, 0, 2);
      QCOMPARE(c.size(), 0); // chunk size to small for one row
    }


    void split_into_1()
    {
      int rows = 3;
      int rowLength = 100;
      int bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      QVector<AsciiFileData> c = buf.splitFile(rows * rowLength, idx, 0, bytes);
      QCOMPARE(c[0].begin(), 0);
      QCOMPARE(c[0].bytesRead(), bytes);

      idx[0] = 10;
      bytes -= 10;
      c = buf.splitFile(rows * rowLength, idx, 10, bytes);
      QCOMPARE(c[0].begin(), 10);
      QCOMPARE(c[0].bytesRead(), bytes);
    }


    void split_into_3()
    {
      int rows = 3;
      int rowLength = 100;
      int bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      QVector<AsciiFileData> c = buf.splitFile(rowLength, idx, 0, bytes);
      QCOMPARE(c.size(), 3);
      QCOMPARE(c[0].begin(), idx[0]);
      QCOMPARE(c[1].begin(), idx[1]);
      QCOMPARE(c[2].begin(), idx[2]);
    }


    void split()
    {
      int rows = 999;
      int rowLength = 100;
      int bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      QVector<AsciiFileData> c = buf.splitFile(rowLength * 100, idx, 0, bytes);
      QCOMPARE(c.size(), 10);
      QCOMPARE(c[0].begin(), idx[0]);
      QCOMPARE(c[1].rowsRead(), 100);
      QCOMPARE(c[1].begin(), idx[100]);
      QCOMPARE(c[1].rowsRead(), 100);
      QCOMPARE(c[9].begin(), idx[900]);
      QCOMPARE(c[9].rowsRead(), 99);

      int rowsRead = 0;
      for (int i=0; i < 10; i++) {
        rowsRead += c[i].rowsRead();
      }
      QCOMPARE(rowsRead, rows);
    }
    

    void split_equal_chunkSize_is_bytesToRead()
    {
      int rows = 20;
      int rowLength = 10;
      int offset = 5;
      int bytesToRead = (rows - offset) * rowLength;
      initRowIndex(rows, rowLength);
      int start = offset * rowLength;
      QVector<AsciiFileData> c = buf.splitFile(bytesToRead, idx, start, bytesToRead);
      QCOMPARE(c.size(), 1);
      QCOMPARE(c[0].bytesRead(), bytesToRead);
      QCOMPARE(c[0].begin(), idx[offset]);
      QCOMPARE(c[0].rowsRead(), rows - offset);
    }

    //void useOneWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int numChunks);

    void useOneWindowWithChunks_small()
    {
      int rows = 1;
      int rowLength = 1;
      initRowIndex(rows, rowLength);
      buf.useOneWindowWithChunks(idx, 0, 1, 1);
      QVector<QVector<AsciiFileData> > d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 1);
      
      rows = 2;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      buf.useOneWindowWithChunks(idx, 0, 2, 1);
      d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 1); // only data for one chunk

      rows = 2;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      buf.useOneWindowWithChunks(idx, 0, 2, 2);
      d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 2); 

      rows = 3;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      buf.useOneWindowWithChunks(idx, 0, 3, 2);
      d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 3); // more than 2 chunks needed
    }

    void useOneWindowWithChunks()
    {
      int rows = 1000;
      int rowLength = 100;
      int bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      buf.useOneWindowWithChunks(idx, 0, bytes, 1);
      QVector<QVector<AsciiFileData> > d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 1);

      buf.useOneWindowWithChunks(idx, 0, bytes, 10);
      d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 10);

      buf.useOneWindowWithChunks(idx, 0, bytes, 1000);
      d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 1000);

      buf.useOneWindowWithChunks(idx, 0, bytes, 1001); // row too long for implizit chunk size
      d = buf.fileData();
      QCOMPARE(d.size(), 0);
    }


    // void useSlidingWindow(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize);

    void useSlidingWindow_small()
    {
      int rows = 1;
      int rowLength = 1;
      initRowIndex(rows, rowLength);
      int windowSize = 1;
      buf.useSlidingWindow(idx, 0, 1, windowSize);
      QVector<QVector<AsciiFileData> > d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 1);

      rows = 2;
      rowLength = 1;
      initRowIndex(rows, rowLength);
      windowSize = 1;
      buf.useSlidingWindow(idx, 0, 2, windowSize);
      d = buf.fileData();
      QCOMPARE(d.size(), 2);
      QCOMPARE(d[0].size(), 1); 
      QCOMPARE(d[1].size(), 1); 
    }


    void useSlidingWindow()
    {
      int rows = 1000;
      int rowLength = 100;
      int bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      int windowSize = rowLength;
      buf.useSlidingWindow(idx, 0, bytes, windowSize);
      QVector<QVector<AsciiFileData> > d = buf.fileData();
      QCOMPARE(d.size(), 1000);
      QCOMPARE(d[0].size(), 1);

      rows = 999;
      rowLength = 100;
      bytes = rows * rowLength;
      windowSize = rowLength * 100;
      buf.useSlidingWindow(idx, 0, bytes, windowSize);
      d = buf.fileData();
      QCOMPARE(d.size(), 10);
      QCOMPARE(d[0].size(), 1);
      QCOMPARE(d[0][0].begin(), 0);
      QCOMPARE(d[0][0].bytesRead(), rowLength * 100);
      QCOMPARE(d[0][0].rowBegin(), 0);
      QCOMPARE(d[1].size(), 1);
      QCOMPARE(d[1][0].rowBegin(), 100);

      int rowsRead = 0;
      for (int i=0; i < 10; i++) {
        rowsRead += d[i][0].rowsRead();
      }
      QCOMPARE(rowsRead, rows);
    }


    // void useSlidingWindowWithChunks(const RowIndex& rowIndex, int start, int bytesToRead, int windowSize, int numWindowChunks)
    
    void useSlidingWindowWithChunks_small()
    {
      int rows = 1;
      int rowLength = 1;
      int bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      int windowSize = 1;
      buf.useSlidingWindowWithChunks(idx, 0, bytes, windowSize, 1);
      QVector<QVector<AsciiFileData> > d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 1);

      rows = 3;
      rowLength = 1;
      bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      windowSize = 3;
      buf.useSlidingWindowWithChunks(idx, 0, bytes, windowSize, 3);
      d = buf.fileData();
      QCOMPARE(d.size(), 1);
      QCOMPARE(d[0].size(), 3);

      rows = 3;
      rowLength = 1;
      bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      windowSize = 1;
      buf.useSlidingWindowWithChunks(idx, 0, bytes, windowSize, 3); // int chunkSize = windowSize / numWindowChunks, == 0
      d = buf.fileData();
      QCOMPARE(d.size(), 0);

      rows = 12;
      rowLength = 1;
      bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      windowSize = 4;
      buf.useSlidingWindowWithChunks(idx, 0, bytes, windowSize, 1);
      d = buf.fileData();
      QCOMPARE(d.size(), 3);
      QCOMPARE(d[0].size(), 1);
      QCOMPARE(d[2].size(), 1);

      buf.useSlidingWindowWithChunks(idx, 0, bytes, windowSize, 2);
      d = buf.fileData();
      QCOMPARE(d.size(), 3);
      QCOMPARE(d[0].size(), 2);
      QCOMPARE(d[2].size(), 2);

      buf.useSlidingWindowWithChunks(idx, 0, bytes, windowSize, 3);
      d = buf.fileData();
      QCOMPARE(d.size(), 4);  // could not split 4 rows into 3 chunks
      QCOMPARE(d[0].size(), 3);
      QCOMPARE(d[1].size(), 3);
      QCOMPARE(d[2].size(), 3);
      QCOMPARE(d[3].size(), 3);
    }


    void useSlidingWindowWithChunks()
    {
      int rows = 1000;
      int rowLength = 100;
      int bytes = rows * rowLength;
      initRowIndex(rows, rowLength);
      int windowSize = rowLength * 100;
      buf.useSlidingWindowWithChunks(idx, 0, bytes, windowSize, 5);
      QVector<QVector<AsciiFileData> > d = buf.fileData();
      QCOMPARE(d.size(), 10);
      QCOMPARE(d[0].size(), 5);
      QCOMPARE(d[9].size(), 5);
    }


private:
    AsciiFileBuffer::RowIndex idx;
    AsciiFileBuffer buf;
    QFile file;

    void initRowIndex(int rows, int rowLength)
    {
      idx.clear();
      idx.resize(rows + 1);
      for (int i = 0; i < rows; i++) {
        idx[i] = i * rowLength;
      }
      idx[rows] = rows * rowLength;
    }
};



QTEST_MAIN(AsciiSourceTest)



#include "moc_asciifilebuffertest.cpp"
