/***************************************************************************
                     scuba.cpp  -  SCUBA file data source
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


#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include <qcheckbox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qstringlist.h>
#include <qstylesheet.h>

#include <kcombobox.h>
#include <ksdebug.h>

#include <kstmath.h>
#include "kststring.h"
#include "scuba.h"
#include "scubaconfig.h"

#define BEGIN_HEADRER_1 "ac"
#define END_HEADER_1 "end_status\n"

#define BEGIN_HEADER_2 "<HEADER>"
#define END_HEADER_2 "</HEADER>"

#define MAXBUFREADLEN 32768
#define FRAMEINDEXBUFFERINCREMENT 32768

#define DEFAULT_COLUMN_WIDTH 16

static char* housekeepingFields[] = {
    "Status",                 // value  0 of housekeeping block
    "Sequence Number",        // value  1 of housekeeping block
    "Sync Number",            // value  2 of housekeeping block
    "DV Pulse Number",        // value  3 of housekeeping block

    "Comm Error FPGA Temp",   // value  4 of housekeeping block
    "AC FPGA Temp",           // value  5 of housekeeping block
    "BC1 FPGA Temp",          // value  6 of housekeeping block
    "BC2 FPGA Temp",          // value  7 of housekeeping block
    "BC3 FPGA Temp",          // value  8 of housekeeping block
    "RC1 FPGA Temp",          // value  9 of housekeeping block
    "RC2 FPGA Temp",          // value 10 of housekeeping block
    "RC3 FPGA Temp",          // value 11 of housekeeping block
    "RC4 FPGA Temp",          // value 12 of housekeeping block
    "CC FPGA Temp",           // value 13 of housekeeping block

    "Comm Error Card Temp",   // value 14 of housekeeping block
    "AC card Temp",           // value 15 of housekeeping block
    "BC1 card Temp",          // value 16 of housekeeping block
    "BC2 card Temp",          // value 17 of housekeeping block
    "BC3 card Temp",          // value 18 of housekeeping block
    "RC1 card Temp",          // value 19 of housekeeping block
    "RC2 card Temp",          // value 20 of housekeeping block
    "RC3 card Temp",          // value 21 of housekeeping block
    "RC4 card Temp",          // value 22 of housekeeping block
    "CC card Temp",           // value 23 of housekeeping block

    "Comm Error PSUC",        // value 24 of housekeeping block
    "PSUC 1",                 // value 25 of housekeeping block
    "PSUC 2",                 // value 26 of housekeeping block
    "PSUC 3",                 // value 27 of housekeeping block
    "PSUC 4",                 // value 28 of housekeeping block
    "PSUC 5",                 // value 29 of housekeeping block
    "PSUC 6",                 // value 30 of housekeeping block
    "PSUC 7",                 // value 31 of housekeeping block

    "Comm Error Box Temp",    // value 32 of housekeeping block
    "Box Temp",               // value 33 of housekeeping block

                              // value 34 of housekeeping block is currently reserved
                              // value 35 of housekeeping block is currently reserved
                              // value 36 of housekeeping block is currently reserved
                              // value 37 of housekeeping block is currently reserved
                              // value 38 of housekeeping block is currently reserved
                              // value 39 of housekeeping block is currently reserved
                              // value 40 of housekeeping block is currently reserved
                              // value 41 of housekeeping block is currently reserved
                              // value 42 of housekeeping block is currently reserved
                              // value 43 of housekeeping block is currently reserved
};

static int numHousekeepingFields = sizeof(housekeepingFields)/sizeof(char*);

class ScubaSource::Config {
  public:
    Config() {
      _readMatrices = false;
      _validateChecksum = true;
    }

    void read(KConfig *cfg, const QString& fileName = QString::null) {
      cfg->setGroup("SCUBA General");
      _readMatrices = cfg->readBoolEntry("Read Matrices", false);
      _validateChecksum = cfg->readBoolEntry("Validate Checksum", false);
      if (!fileName.isEmpty()) {
        cfg->setGroup(fileName);
        _readMatrices = cfg->readBoolEntry("Read Matrices", _readMatrices);
        _validateChecksum = cfg->readBoolEntry("Validate Checksum", _validateChecksum);
      }
    }

    bool _readMatrices;
    bool _validateChecksum;

    void save(QTextStream& str, const QString& indent) {
      if (_readMatrices) {
        str << indent << "<matrices>\"";
      }
      if (_validateChecksum) {
        str << indent << "<checksum>\"";
      }
    }

    void load(const QDomElement& e) {
      QDomNode n = e.firstChild();
      while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
          if (e.tagName() == "matrices") {
            _readMatrices = true;
          } else if (e.tagName() == "checksum") {
            _validateChecksum = true;
          }

        }
        n = n.nextSibling();
      }
   }
};


ScubaSource::ScubaSource(KConfig *cfg, const QString& filename, const QString& type, const QDomElement& e)
: KstDataSource(cfg, filename, type), _frameIndex(0L), _config(0L), _tmpBuf(0L), _tmpBufSize(0) {
  _valid = false;
  _haveHeader = false;
  _fieldListComplete = false;
  _rowStart = 0;
  _colStart = 0;
  _numCols = 8;
  _numRows = 41;
  _rowLen = 0;
  _format = FormatText2;
  _numEntriesInFormatText2Line = 8;
  _first = true;
  _numFramesLastReadMatrix = 0;

  if (type.isEmpty() || type == "SCUBA") {
    _config = new ScubaSource::Config;
    _config->read(cfg, filename);
    if (!e.isNull()) {
      _config->load(e);
    }

    _valid = true;
    update();
  }
}


ScubaSource::~ScubaSource() {
  if (_tmpBuf) {
    free(_tmpBuf);
    _tmpBuf = 0L;
    _tmpBufSize = 0;
  }

  if (_frameIndex) {
    free(_frameIndex);
    _frameIndex = 0L;
    _numFrameIndexAlloc = 0;
  }

  delete _config;
  _config = 0L;
}


bool ScubaSource::reset() {
  if (_tmpBuf) {
    free(_tmpBuf);
    _tmpBuf = 0L;
    _tmpBufSize = 0;
  }

  if (_frameIndex) {
    free(_frameIndex);
    _frameIndex = 0L;
    _numFrameIndexAlloc = 0;
  }

  _haveHeader = false;
  _fieldListComplete = false;
  _fieldList.clear();

  update();

  return true;
}


int ScubaSource::readFullLine(QFile &file, QString &str) {
  int read = file.readLine(str, 1000);

  if (read == 1000-1) {
    QString strExtra;
    while (str[read-1] != '\n') {
      int readExtra = file.readLine(strExtra, 1000);
      if (readExtra > 0) {
        read += readExtra;
        str += strExtra;
      } else {
        read = readExtra;
        break;
      }
    }
  }

  return read;
}


bool ScubaSource::initFrameIndex() {
  bool rc = false;

  if (!_frameIndex) {
    _frameIndex = (QIODevice::Offset*)malloc(FRAMEINDEXBUFFERINCREMENT * sizeof(QIODevice::Offset));
    _numFrameIndexAlloc = FRAMEINDEXBUFFERINCREMENT;
  }

  _frameIndex[0] = 0;
  _byteLength = 0;
  _numFrames = 0;

  QFile file(_filename);
  if (file.open(IO_ReadOnly)) {
    KstString *metaString;
    QStringList entries;
    QString metaName;
    QString s;
    bool done = false;
    bool ok;
    int line = 0;
    int read;
    int row;

    while (!done) {
      read = readFullLine(file, s);

      if (_first) {
        metaName.sprintf("LINE%03d", line);
        metaString = new KstString(KstObjectTag(metaName, tag()), this, s);
        _metaData.insert(metaName, metaString);
      }

      if (read < 0) {
        done = true;
      } else if (s.compare(END_HEADER_1) == 0) {
        _frameIndex[0] = file.at();
        done = true;
        rc = true;
      } else if (s.contains("data_mode") == 1) {
        _datamode = s.right(8).toInt(&ok, 16);
        if (!ok) {
          _datamode = -1;
        }
      } else if (s.contains("row_len") == 1) {
        _rowLen = s.right(8).toInt(&ok, 16);
        if (!ok) {
          _rowLen = -1;
        }
      } else if (s.contains("num_rows") == 1) {
        _numRows = s.right(8).toInt(&ok, 16);
        if (!ok) {
          _numRows = -1;
        }
      } else if (s.contains("row_order") == 1) {
        entries = QStringList::split( QChar('\t'), s);
        if (!entries.empty()) {
          s = entries.last();
          entries = QStringList::split( QChar(' '), s);
          for (QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it) {
            row = (*it).toInt(&ok, 16);
            if (ok) {
              _rows.append(row);
            }
          }
        }
      }
      ++line;
    }

    _numCols = 8;

    _first = false;
  }

  return rc;
}


KstObject::UpdateType ScubaSource::update(int u) {
  KstObject::UpdateType rc = lastUpdateResult();

  if (!KstObject::checkUpdateCounter(u)) {
    if (!_haveHeader) {
      _haveHeader = initFrameIndex();
      if (_haveHeader) {
        //
        // update the field list since we have one now
        //
        _fields = fieldListFor(_filename, _config);
        _fieldListComplete = _fields.count() > 1;

        //
        // update the matrix list since we have one now
        //
        _matrixList = matrixList();
      }
    }

    if (_haveHeader) {
      bool forceUpdate = false;
      QFile file(_filename);

      if (file.exists()) {
        if (uint(_byteLength) != file.size() || !_valid) {
          forceUpdate = true;
        }

        _byteLength = file.size();

        if (file.open(IO_ReadOnly)) {
          bool new_data = false;

          file.at(_frameIndex[_numFrames]);

          if (_format == FormatText) {
            //
            // currently not operational...
            //
          } else if (_format == FormatBinary) {
            //
            // currently no test data available...
            //
          } else if (_format == FormatText2) {
            QStringList entries;
            QString s;
            long value;
            long checksum;
            bool ok;
            int read = 0;
            int i;

            while (read != -1) {
              checksum = 0;

              for (i=0; i<42; i++) {
                read = readFullLine(file, s);

                if (read == -1) {
                  break;
                }
                entries = QStringList::split(QChar(' '), s);
                for (QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it) {
                  value = (*it).toInt(&ok, 10);
                  if (ok) {
                    checksum ^= value;
                  }
                }
              }

              if (read != -1) {
                read = readFullLine(file, s);

                if (read != -1) {
                  value = s.toInt(&ok, 10);

                  if (value == checksum) {
                    if (_numFrames >= _numFrameIndexAlloc) {
                      _numFrameIndexAlloc += FRAMEINDEXBUFFERINCREMENT;
                      _frameIndex = (QIODevice::Offset*)realloc(_frameIndex, _numFrameIndexAlloc*sizeof(QIODevice::Offset));
                    }
                    new_data = true;
                    ++_numFrames;
                    _frameIndex[_numFrames] = file.at();
                  }
                }
              }
            }
          }

          file.close();

          updateNumFramesScalar();

          _valid = true;

          rc = setLastUpdateResult(forceUpdate ? KstObject::UPDATE : (new_data ? KstObject::UPDATE : KstObject::NO_CHANGE));
        } else {
          _valid = false;

          rc = setLastUpdateResult(KstObject::NO_CHANGE);
        }
      } else {
        _valid = false;

        rc = setLastUpdateResult(KstObject::NO_CHANGE);
      }
    } else {
      rc = setLastUpdateResult(KstObject::NO_CHANGE);
    }
  }

  return rc;
}


bool ScubaSource::fieldListIsComplete() const {
  return _fieldListComplete;
}


bool ScubaSource::matrixDimensions(const QString& matrix, int* xDim, int* yDim) {
  bool rc = false;

  if (isValidMatrix(matrix)) {
    rc = true;

    *yDim = _numRows;
    *xDim = _numCols;
  }

  return rc;
}


int ScubaSource::readMatrix(KstMatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps) {
  int totalSamples = 0;

  if (isValidMatrix(matrix)) {
    if (xNumSteps > 0 && yNumSteps > 0) {
      if (_datamode != DataRaw) {
        QFile file(_filename);

        if (file.open(IO_ReadOnly)) {
          double *values = data->z;
          QStringList entries;
          QString str;
          long lvalue = 0;
          bool ok;
          bool error = false;
          bool max = false;
          bool avg = false;
          int read = 0;
          int frameStart = _numFrames;
          int frameEnd = _numFrames;
          int frame;
          int i;
          int j;

          if (matrix.contains("Error") > 0) {
            error = true;
          }
          if (matrix.contains("Max") > 0) {
           frameStart = _numFramesLastReadMatrix;
           max = true;
          }
          if (matrix.contains("Avg") > 0) {
            frameStart = _numFramesLastReadMatrix;
            avg = true;
          }

          for (frame=frameStart-1; frame<frameEnd; frame++) {
            file.at(_frameIndex[frame]);

            if (_format == FormatText) {
              //
              // currently not operational...
              //
            } else if (_format == FormatBinary) {
              //
              // currently no test data available...
              //
            } else if (_format == FormatText2) {
              //
              // always need to skip the first line as it is the header...
              //
              for (i=0; i<yStart+1; ++i) {
                read = readFullLine(file, str);
              }

              for (i=0; i<yNumSteps; ++i) {
                read = readFullLine(file, str);
                if (read != -1) {
                  entries = QStringList::split(QChar(' '), str);
                  for (j=0; j<xNumSteps; j++) {
                    if (j < int(entries.size())) {
                      lvalue = entries[j].toInt(&ok, 10);
                      if (!ok) {
                        *values = KST::NOPOINT;
                      } else {
                        switch (_datamode) {
                          case DataError:
                          case DataPreScaleFeedback:
                          case DataFiltered:
                          case DataRaw:
                            break;
                          case Data18_14:
                            if (!error) {
                              lvalue >>= 14;
                              lvalue &= 0x3FFFF;
                              if (lvalue & 0x40000) {
                                lvalue &= 0x1FFFF;
                                lvalue *= -1;
                              }
                            } else {
                              lvalue &= 0x3FFF;
                              if (lvalue & 0x4000) {
                                lvalue &= 0x1FFF;
                                lvalue *= -1;
                              }
                            }
                            break;
                          case Data24_8:
                            if (!error) {
                              lvalue >>= 8;
                              lvalue  &= 0xFFFFFF;
                              if (lvalue & 0x800000) {
                                lvalue &= 0x7FFFFF;
                                lvalue *= -1;
                              }
                            } else {
                              lvalue &= 0xFF;
                              if (lvalue & 0x80) {
                                lvalue &= 0x7F;
                                lvalue *= -1;
                              }
                            }
                            break;
                        }

                        if (max) {
                          if (frame > frameStart && double(lvalue) > *values) {
                            *values = double(lvalue);
                          }
                        } else if (avg) {
                          *values += double(lvalue);
                        } else {
                          *values = double(lvalue);
                        }
                      }

                      ++values;
                    }
                  }
                }
              }
            }
          }

          if (avg && frameEnd > frameStart) {
            values = data->z;

            for (i=0; i<yNumSteps; ++i) {
              for (j=0; j<xNumSteps; j++) {
                *values /= double(frameEnd - frameStart);
              }
            }
          }

          data->xMin = xStart;
          data->yMin = yStart;
          data->xStepSize = 1;
          data->yStepSize = 1;

          totalSamples = xNumSteps * yNumSteps;

          _numFramesLastReadMatrix = _numFrames;

          file.close( );
        }
      }
    }
  }

  return totalSamples;
}


int ScubaSource::readField(double *v, const QString& field, int s, int n) {
  int rc = 0;
  int fieldIndex = -1;
  int i = 0;

  if (n < 0) {
    n = 1;
  }

  if (field == "INDEX") {
    for (i = 0; i < n; i++) {
      v[i] = double(s + i);

      rc = n;
    }
  } else {
    QStringList fieldList = this->fieldList();

    for (QStringList::ConstIterator it = fieldList.begin(); it != fieldList.end(); ++it, ++i) {
      if (*it == field) {
        fieldIndex = i;

        break;
      }
    }

    if (fieldIndex != -1) {
      int iSamplesPerFrame = samplesPerFrame(field);

      QIODevice::Offset bufread = _frameIndex[(s + n)/iSamplesPerFrame] - _frameIndex[s/iSamplesPerFrame];

      if (bufread > 0) {
        QStringList values;
        QFile file(_filename);
        QString str;
        long lvalue = 0;
        bool ok;
        int valueIndex = 0;
        int read = 0;

        if (file.open(IO_ReadOnly)) {
          if (_format == FormatText) {
            //
            // currently not operational...
            //
          } else if (_format == FormatBinary) {
            //
            // currently no test data available...
            //
          } else if (_format == FormatText2) {
            QStringList entries;
            QString str;
            int lineIndex;
            int lines;

            for (i = 0; i < n; ++i) {
              v[i] = KST::NOPOINT;

              file.at(_frameIndex[(s + i)/iSamplesPerFrame]);

              if (fieldIndex < numHousekeepingFields) {
                if( readFullLine(file, str) != -1) {
                  entries = QStringList::split(QChar(' '), str);
                  if (fieldIndex < int(entries.size())) {
                    v[i] = double(entries[fieldIndex].toInt(&ok, 10));
                    if (!ok) {
                      v[i] = KST::NOPOINT;
                    }
                  }
                }
              } else {
                switch (_datamode) {
                  case DataError:
                  case DataPreScaleFeedback:
                  case DataFiltered:
                  case DataRaw:
                    valueIndex = fieldIndex - numHousekeepingFields;
                    valueIndex += s + i - (((s + i)/iSamplesPerFrame) * iSamplesPerFrame ); 
                    break;
                  case Data18_14:
                  case Data24_8:
                    valueIndex = (fieldIndex - numHousekeepingFields) / 2;
                    break;
                }

                lines = valueIndex / _numEntriesInFormatText2Line;
                lineIndex = valueIndex - ( lines * _numEntriesInFormatText2Line );

                //
                // always need to skip the first line as it is the header...
                //
                for (int j=0; j<lines+1; j++) {
                  readFullLine(file, str);
                }

                if (read != -1) {
                  if (readFullLine(file, str) != -1) { 
                    values = QStringList::split(QChar(' '), str);
                    if (lineIndex < int(values.size())) {
                      lvalue = values[lineIndex].toInt(&ok, 10);
                      switch (_datamode) {
                        case DataError:
                        case DataPreScaleFeedback:
                        case DataFiltered:
                        case DataRaw:
                          break;
                        case Data18_14:
                          if (valueIndex % 2 == 0) {
                            lvalue >>= 14;
                            lvalue &= 0x3FFFF;
                            if (lvalue & 0x40000) {
                              lvalue &= 0x1FFFF;
                              lvalue *= -1;
                            }
                          } else {
                            lvalue &= 0x3FFF;
                            if (lvalue & 0x4000) {
                              lvalue &= 0x1FFF;
                              lvalue *= -1;
                            }
                          }
                          break;
                        case Data24_8:
                          if (valueIndex % 2 == 0) {
                            lvalue >>= 8;
                            lvalue  &= 0xFFFFFF;
                            if (lvalue & 0x800000) {
                              lvalue &= 0x7FFFFF;
                              lvalue *= -1;
                            }
                          } else {
                            lvalue &= 0xFF;
                            if (lvalue & 0x80) {
                              lvalue &= 0x7F;
                              lvalue *= -1;
                            }
                          }
                          break;
                      }
                      v[i] = double(lvalue);
                    }
                  }
                }
              }
            }

            rc = n;
          }

          file.close();
        } else {
          _valid = false;

          rc = 0;
        }
      }
    }
  }

  return rc;
}


bool ScubaSource::isValidField(const QString& field) const {
  return fieldList().contains(field);
}


bool ScubaSource::isValidMatrix(const QString& matrix) const {
  return matrixList().contains(matrix);
}


int ScubaSource::samplesPerFrame(const QString &field) {
  //
  // the samplesPerFrame may depend on the field...
  //
  int i;
  int rc = -1;

  for (i=0; i<numHousekeepingFields; i++) {
    if (field.compare(housekeepingFields[i]) == 0) {
      rc = 1;

      break;
    }
  }

  if (rc == -1) {
    //
    // need to determine the number of samples per frame...
    //
    if (_datamode == 3) {
      rc = _numCols * _numRows;
    } else {
      rc = 1;
    }
  }

  return rc;
}


int ScubaSource::frameCount(const QString& field) const {
  Q_UNUSED(field)

  //
  // the number of frames is the same for all fields...
  //
  return _numFrames;
}


QString ScubaSource::fileType() const {
  return "SCUBA";
}


bool ScubaSource::isEmpty() const {
  return _numFrames < 1;
}

QStringList ScubaSource::fieldListFor(const QString& filename, ScubaSource::Config *cfg) {
  Q_UNUSED(cfg)

  QStringList rc;
  QFile file(filename);

  if (file.open(IO_ReadOnly)) {
    QStringList entries;
    QValueList<int> rows;
    QString s;
    bool done = false;
    bool ok;
    int datamode = -1;
    int num_rows = -1;
    int num_cols = -1;
    int row_len = -1;
    int read;
    int row;
    int i;
    int j;

    while (!done) {
      read = file.readLine(s, 1000);
      if (read < 0) {
        done = true;
      } else if (s.compare(END_HEADER_1) == 0) {
        done = true;
      } else if (s.contains("data_mode") == 1) {
        datamode = s.right(8).toInt(&ok, 16);
        if (!ok) {
          datamode = -1;
        }
      } else if (s.contains("row_len") == 1) {
        row_len = s.right(8).toInt(&ok, 16);
        if (!ok) {
          row_len = -1;
        }
      } else if (s.contains("num_rows") == 1) {
        num_rows = s.right(8).toInt(&ok, 16);
        if (!ok) {
          num_rows = -1;
        }
      } else if (s.contains("row_order") == 1) {
        entries = QStringList::split( QChar('\t'), s);
        if (!entries.empty()) {
          s = entries.last();
          entries = QStringList::split( QChar(' '), s);
          for (QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it) {
            row = (*it).toInt(&ok, 16);
            if (ok) {
              rows.append(row);
            }
          }
        }
      }
    }

    num_cols = 8;

    rc += "INDEX";

    for (i=0; i<numHousekeepingFields; i++) {
      rc += housekeepingFields[i];
    }

    if (datamode == DataRaw) {
      rc += QString("Pixel(raw)");
    } else if (datamode >= 0) {
      for (i=0; i<num_rows; i++) {
        for (j=0; j<num_cols; j++) {
          switch (datamode) {
            case DataError:
              rc += QString("Error_%1_%2").arg(i).arg(j);
              break;
            case DataPreScaleFeedback: 
              rc += QString("Pixel_%1_%2").arg(i).arg(j);
              break;
            case DataFiltered:
              rc += QString("Pixel_%1_%2").arg(i).arg(j);
              break;
            case Data18_14:
              rc += QString("Pixel_%1_%2").arg(i).arg(j);
              rc += QString("Error_%1_%2").arg(i).arg(j);
              break;
            case Data24_8: 
              rc += QString("Pixel_%1_%2").arg(i).arg(j);
              rc += QString("Error_%1_%2").arg(i).arg(j);
              break;
            default:
              break;
          }
        }
      }

      //
      // include some simple calculations, such as row average, column average, etc...
      //
    }

    file.close();
  }

  return rc;
}


QStringList ScubaSource::fieldList() const {
  if (_fields.isEmpty()) {
    _fields = fieldListFor(_filename, _config);
    _fieldListComplete = _fields.count() > 1;
  }

  return _fields;
}


QStringList ScubaSource::matrixList() const {
  if (_matrixList.isEmpty()) {
    if (_config->_readMatrices) {
      if (_datamode != DataRaw) {
        QFile file(_filename);
    
        if (file.open(IO_ReadOnly)) {
          QStringList entries;
          QStringList rc;
          QValueList<int> rows;
          QString s;
          bool done = false;
          bool ok;
          int datamode = -1;
          int num_rows = -1;
          int num_cols = -1;
          int row_len = -1;
          int read;
          int row;
          int i;
    
          while (!done) {
            read = file.readLine(s, 1000);
            if (read < 0) {
              done = true;
            } else if (s.compare(END_HEADER_1) == 0) {
              done = true;
            } else if (s.contains("data_mode") == 1) {
              datamode = s.right(8).toInt(&ok, 16);
              if (!ok) {
                datamode = -1;
              }
            } else if (s.contains("row_len") == 1) {
              row_len = s.right(8).toInt(&ok, 16);
              if (!ok) {
                row_len = -1;
              }
            } else if (s.contains("num_rows") == 1) {
              num_rows = s.right(8).toInt(&ok, 16);
              if (!ok) {
                num_rows = -1;
              }
            } else if (s.contains("row_order") == 1) {
              entries = QStringList::split( QChar('\t'), s);
              if (!entries.empty()) {
                s = entries.last();
                entries = QStringList::split( QChar(' '), s);
                for (QStringList::ConstIterator it = entries.begin(); it != entries.end(); ++it) {
                  row = (*it).toInt(&ok, 16);
                  if (ok) {
                    rows.append(row);
                  }
                }
              }
            }
          }

          num_cols = 8;

          for (i=0; i<_numFrames; i++) {
            switch (datamode) {
              case DataError:
                _matrixList += QString("FrameError_%d").arg(i);
                break;
              case DataPreScaleFeedback: 
                _matrixList += QString("FramePixel_%d").arg(i);
                break;
              case DataFiltered:
                _matrixList += QString("FramePixel_%d").arg(i);
                break;
              case Data18_14:
                _matrixList += QString("FramePixel_%d").arg(i);
                _matrixList += QString("FrameError_%d").arg(i);
                break;
              case Data24_8:
                _matrixList += QString("FramePixel_%d").arg(i);
                _matrixList += QString("FrameError_%d").arg(i);
                break;
              default:
                break;
            }
          }

          switch (datamode) {
            case DataError:
              _matrixList += QString("FrameErrorLast");
              _matrixList += QString("FrameErrorRecentAvg");
              _matrixList += QString("FrameErrorRecentMax");
              break;
            case DataPreScaleFeedback:
              _matrixList += QString("FramePixelLast");
              _matrixList += QString("FramePixelRecentAvg");
              _matrixList += QString("FramePixelRecentMax");
              _matrixList += QString("FramePixelRecentMin");

              _matrixList += QString("FrameErrorLast");
              _matrixList += QString("FrameErrorRecentAvg");
              _matrixList += QString("FrameErrorRecentMax");
              break;
            case DataFiltered:
              _matrixList += QString("FramePixelLast");
              _matrixList += QString("FramePixelRecentAvg");
              _matrixList += QString("FramePixelRecentMax");
              _matrixList += QString("FramePixelRecentMin");
              break;
            case Data18_14:
              _matrixList += QString("FramePixelLast");
              _matrixList += QString("FramePixelRecentAvg");
              _matrixList += QString("FramePixelRecentMax");
              _matrixList += QString("FramePixelRecentMin");

              _matrixList += QString("FrameErrorLast");
              _matrixList += QString("FrameErrorRecentAvg");
              _matrixList += QString("FrameErrorRecentMax");
              break;
            case Data24_8:
              _matrixList += QString("FramePixelLast");
              _matrixList += QString("FramePixelRecentAvg");
              _matrixList += QString("FramePixelRecentMax");
              _matrixList += QString("FramePixelRecentMin");

              _matrixList += QString("FrameErrorLast");
              _matrixList += QString("FrameErrorRecentAvg");
              _matrixList += QString("FrameErrorRecentMax");
              break;
            default:
              break;
          }

          file.close();
        }
      }
    }
  }

  return _matrixList;
}


void ScubaSource::save(QTextStream &ts, const QString& indent) {
  KstDataSource::save(ts, indent);
  _config->save(ts, indent);
}


bool ScubaSource::supportsTimeConversions() const {
  return false; //fieldList().contains(_config->_indexVector) && _config->_indexInterpretation != ScubaSource::Config::Unknown && _config->_indexInterpretation != ScubaSource::Config::INDEX;
}


int ScubaSource::sampleForTime(double ms, bool *ok) {
  return 0;
}


int ScubaSource::sampleForTime(const KST::ExtDateTime& time, bool *ok) {
  return 0;
}


class ConfigWidgetScuba : public KstDataSourceConfigWidget {
  public:
    ConfigWidgetScuba() : KstDataSourceConfigWidget() {
      QGridLayout *layout = new QGridLayout(this, 1, 1);
      _ac = new ScubaConfig(this);
      layout->addWidget(_ac, 0, 0);
      layout->activate();
    }

    virtual ~ConfigWidgetScuba() {}

    virtual void setConfig(KConfig *cfg) {
      KstDataSourceConfigWidget::setConfig(cfg);
    }

    virtual void load() {
      bool hasInstance = _instance != 0L;

      _cfg->setGroup("SCUBA General");
      _ac->_readMatrices->setChecked(_cfg->readBoolEntry("Read Matrices", false));
      _ac->_validateChecksum->setChecked(_cfg->readBoolEntry("Validate Checksum", false));

      if (hasInstance) {
        KstSharedPtr<ScubaSource> src = kst_cast<ScubaSource>(_instance);
        if (src) {
          _cfg->setGroup(src->fileName());
          _ac->_readMatrices->setChecked(_cfg->readBoolEntry("Read Matrices", _ac->_readMatrices->isChecked()));
          _ac->_validateChecksum->setChecked(_cfg->readBoolEntry("Validate Checksum", _ac->_validateChecksum->isChecked()));
        }
      }
    }

    virtual void save() {
      assert(_cfg);
      _cfg->setGroup("SCUBA General");

      KstSharedPtr<ScubaSource> src = kst_cast<ScubaSource>(_instance);
      if (src) {
        _cfg->setGroup(src->fileName());
      }
      _cfg->writeEntry("Read Matrices", _ac->_readMatrices->isChecked());
      _cfg->writeEntry("Validate Checksum", _ac->_validateChecksum->isChecked());

      //
      // update the instance from our new settings...
      //
      if (src && src->reusable()) {
        src->_config->read(_cfg, src->fileName());
      }
    }

    ScubaConfig *_ac;
};


extern "C" {
  KstDataSource *create_scuba(KConfig *cfg, const QString& filename, const QString& type) {
    return new ScubaSource(cfg, filename, type);
  }
  
  KstDataSource *load_scuba(KConfig *cfg, const QString& filename, const QString& type, const QDomElement& e) {
    return new ScubaSource(cfg, filename, type, e);
  }
  
  QStringList provides_scuba() {
    QStringList rc;
    rc += "SCUBA";
    return rc;
  }
  
  int understands_scuba(KConfig *cfg, const QString& filename) {
    ScubaSource::Config config;
    config.read(cfg, filename);
    int retVal = 0;
  
    if (QFile::exists(filename) && !QFileInfo(filename).isDir()) {
      QFile f(filename);
  
      if (f.open(IO_ReadOnly)) {
        QString s;
        Q_LONG rc = 0;
        bool done = false;
        int rownum = 0;
  
        while (!done && rownum < 2000) {
          rc = f.readLine(s, 1000);
          if (rc < 0) {
            retVal = 0;
            done = true;
          } else if (s.compare(END_HEADER_1) == 0) {
            retVal = 100;
            done = true;
          }
          ++rownum;
        }

        f.close( );
      } else {
        retVal = 0;
      }
    }

    return retVal;
  }


  QStringList fieldList_scuba(KConfig *cfg, const QString& filename, const QString& type, QString *typeSuggestion, bool *complete) {
    if ((!type.isEmpty() && !provides_scuba().contains(type)) ||
        0 == understands_scuba(cfg, filename)) {
      if (complete) {
        *complete = false;
      }
      return QStringList();
    }

    if (typeSuggestion) {
      *typeSuggestion = "SCUBA";
    }

    ScubaSource::Config config;

    config.read(cfg, filename);
    QStringList rc = ScubaSource::fieldListFor(filename, &config);

    if (complete) {
      *complete = rc.count() > 1;
    }

    return rc;
  }


  QWidget *widget_scuba(const QString& filename) {
    Q_UNUSED(filename)
    return new ConfigWidgetScuba;
  }
}

KST_KEY_DATASOURCE_PLUGIN(scuba)

// vim: ts=2 sw=2 et
