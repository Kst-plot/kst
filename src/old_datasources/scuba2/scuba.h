/***************************************************************************
                       scuba.h  -  SCUBA data source
                             -------------------
    begin                : Tue Jan 22 2007
    copyright            : (C) 2007 The University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCUBA_H
#define SCUBA_H

#include <qfile.h>

#include <kstdatasource.h>

enum DataFormat { FormatText = 0, FormatBinary, FormatText2 };
enum DataMode { DataError = 0, DataPreScaleFeedback, DataFiltered, DataRaw, Data18_14, Data24_8 };

class ScubaSource : public KstDataSource {
  public:
    ScubaSource(KConfig *cfg, const QString& filename, const QString& type, const QDomElement& e = QDomElement());
    ~ScubaSource();

    static int readFullLine(QFile &file, QString &str);

    KstObject::UpdateType update(int = -1);
    int readField(double *v, const QString &field, int s, int n); 
    virtual int readMatrix(KstMatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps);
    bool matrixDimensions(const QString& matrix, int* xDim, int* yDim);
    bool isValidField(const QString &field) const;
    bool isValidMatrix(const QString &field) const;
    int samplesPerFrame(const QString &field);
    int frameCount(const QString& field = QString::null) const;
    QString fileType() const;
    QStringList fieldList() const;
    QStringList matrixList() const;
    void save(QTextStream &ts, const QString& indent = QString::null);
    bool isEmpty() const;
    bool supportsTimeConversions() const;
    int sampleForTime(double ms, bool *ok);
    int sampleForTime(const KST::ExtDateTime& time, bool *ok);
    bool fieldListIsComplete() const;
    bool reset();

    class Config;
    static QStringList fieldListFor(const QString& filename, Config *cfg);
  private:
    bool initFrameIndex();

    int _datamode;
    int _rowLen;
    int _numCols;
    int _numRows;
    int _rowStart;
    int _colStart;
    int _numEntriesInFormatText2Line;
    QIODevice::Offset *_frameIndex;
    QValueList<int> _rows;
    DataFormat _format;
    int _numFrameIndexAlloc;
    int _numFrames;
    int _byteLength;
    mutable QStringList _fields;
    friend class ConfigWidgetScuba;
    mutable Config *_config;
    char *_tmpBuf;
    uint _tmpBufSize;
    bool _haveHeader;
    bool _first;
    int _numFramesLastReadMatrix;
    mutable bool _fieldListComplete;
};


#endif
// vim: ts=2 sw=2 et
