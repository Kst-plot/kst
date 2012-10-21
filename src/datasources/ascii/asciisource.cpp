/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2003 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "asciisource.h"
#include "asciidatainterfaces.h"

#include "curve.h"
#include "colorsequence.h"
#include "objectstore.h"

#include "math_kst.h"
#include "kst_i18n.h"
#include "kst_atof.h"
#include "measuretime.h"

#include <QFile>
#include <QMessageBox>
#include <QThread>
#include <QtConcurrentRun>
#include <QFutureSynchronizer>
#include <ctype.h>
#include <stdlib.h>


using namespace Kst;


//-------------------------------------------------------------------------------------------
static const QString asciiTypeString = I18N_NOOP("ASCII file");


//-------------------------------------------------------------------------------------------
const QString AsciiSource::asciiTypeKey()
{
  return asciiTypeString;
}


//-------------------------------------------------------------------------------------------
AsciiSource::AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e) :
  Kst::DataSource(store, cfg, filename, type),
  _reader(_config),
  _fileBuffer(),
  is(new DataInterfaceAsciiString(*this)),
  iv(new DataInterfaceAsciiVector(*this))
{
  setInterface(is);
  setInterface(iv);
  
  reset();
  
  // TODO only works for local files
  setUpdateType(File);
  
  _source = asciiTypeString;
  if (!type.isEmpty() && type != asciiTypeString) {
    return;
  }
  
  _config.readGroup(*cfg, filename);
  if (!e.isNull()) {
    _config.load(e);
  }
  
  _valid = true;
  registerChange();
  internalDataSourceUpdate(false);
}


//-------------------------------------------------------------------------------------------
AsciiSource::~AsciiSource() 
{
}


//-------------------------------------------------------------------------------------------
void AsciiSource::reset() 
{
  // forget about cached data
  _fileBuffer.clear();
  _reader.clear();
  _haveWarned = false;

  _valid = false;
  _byteLength = 0;
  _haveHeader = false;
  _fieldListComplete = false;
  
  _fieldList.clear();
  _scalarList.clear();
  _strings.clear();
  
  Object::reset();
  
  _strings = fileMetas();
}

//-------------------------------------------------------------------------------------------
bool AsciiSource::initRowIndex() 
{
  _reader.clear();
  _byteLength = 0;
  
  if (_config._dataLine > 0) {
    QFile file(_filename);
    if (!AsciiFileBuffer::openFile(file)) {
      return false;
    }
    int header_row = 0;
    int left = _config._dataLine;
    int didRead = 0;
    while (left > 0) {
      QByteArray line = file.readLine();
      if (line.isEmpty() || file.atEnd()) {
        return false;
      }
      didRead += line.size();
      --left;
      if (header_row != _config._fieldsLine && header_row != _config._unitsLine) {
        _strings[QString("Header %1").arg(header_row, 2, 10, QChar('0'))] = QString::fromAscii(line).trimmed();
      }
      header_row++;
    }
    _reader.setRow0Begin(didRead);
  }
  
  return true;
}


//-------------------------------------------------------------------------------------------
Kst::Object::UpdateType AsciiSource::internalDataSourceUpdate()
{
  return internalDataSourceUpdate(true);
}


//-------------------------------------------------------------------------------------------
Kst::Object::UpdateType AsciiSource::internalDataSourceUpdate(bool read_completely)
{
  //MeasureTime t("AsciiSource::internalDataSourceUpdate: " + _filename);
  
  // forget about cached data
  _fileBuffer.clear();
  
  if (!_haveHeader) {
    _haveHeader = initRowIndex();
    if (!_haveHeader) {
      return NoChange;
    }
    // Re-update the field list since we have one now
    _fieldList = fieldListFor(_filename, &_config);
    QStringList units;
    if (_config._readUnits) {
      units += unitListFor(_filename, &_config);
      for (int index = 0; index < _fieldList.size(); ++index) {
        if (index >= units.size()) {
          break; // Missing units => the user's fault, but at least don't crash
        }
        _fieldUnits[_fieldList[index]] = units[index];
      }
    }
    _fieldListComplete = _fieldList.count() > 1;
    
    // Re-update the scalar list since we have one now
    _scalarList = scalarListFor(_filename, &_config);
  }
  
  QFile file(_filename);
  if (!AsciiFileBuffer::openFile(file)) {
    // Qt: If the device is closed, the size returned will not reflect the actual size of the device.
    return NoChange;
  }
  
  bool force_update = true;
  if (_byteLength == file.size()) {
    force_update = false;
  }
  _byteLength = file.size();
  
  bool new_data = _reader.findDataRows(read_completely, file, _byteLength);
  
  return (!new_data && !force_update ? NoChange : Updated);
}


//-------------------------------------------------------------------------------------------
int AsciiSource::columnOfField(const QString& field) const
{
  if (_fieldList.contains(field)) {
    return _fieldList.indexOf(field);
  } 
  
  if (_fieldListComplete) {
    return -1;
  }
  
  bool ok = false;
  int col = field.toInt(&ok);
  if (ok) {
    return col;
  }
  
  return -1;
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::useSlidingWindow(int bytesToRead)  const
{
  return _config._limitFileBuffer && _config._limitFileBufferSize < bytesToRead;
}


//-------------------------------------------------------------------------------------------
int AsciiSource::readField(double *v, const QString& field, int s, int n) 
{
  int read = tryReadField(v, field, s, n);

  QString msg("%1 because not enough memory is available.\nSetting a file buffer limit could help.");
  if (read == n) {
    return read;
  } else if (read > 0) {
    if (!_haveWarned)
      QMessageBox::warning(0, "Error while reading ASCII file", msg.arg("The file was only read partially"));
    _haveWarned = true;
    return read;
  } else if (read == 0) {
    if (!_haveWarned)
      QMessageBox::warning(0, "Error while reading ASCII file", msg.arg("The file could not be read"));
    _haveWarned = true;
  } else if (read == -3) {
    if (!_haveWarned)
      QMessageBox::warning(0, "Error while reading ASCII file", "The file could not be opened for reading");
    _haveWarned = true;
  }

  return 0;
}


//-------------------------------------------------------------------------------------------
int AsciiSource::tryReadField(double *v, const QString& field, int s, int n)
{
  if (n < 0) {
    n = 1; /* n < 0 means read one sample, not frame - irrelevent here */
  }
  
  if (field == "INDEX") {
    for (int i = 0; i < n; i++) {
      v[i] = double(s + i);
    }
    return n;
  }
  
  int col = columnOfField(field);
  if (col == -1) {
    return -2;
  }
  
  // check if the already in buffer
  int begin = _reader.beginOfRow(s);
  int bytesToRead = _reader.beginOfRow(s + n) - begin;
  if ((begin != _fileBuffer.begin()) || (bytesToRead != _fileBuffer.bytesRead())) {
    QFile* file = new QFile(_filename);
    if (!AsciiFileBuffer::openFile(*file)) {
      delete file;
      return -3;
    }

    // prepare file buffer

    _fileBuffer.setFile(file);

    int numThreads;
    if (!_config._useThreads) {
      numThreads = 1;
    } else {
      numThreads = QThread::idealThreadCount();
      numThreads = (numThreads > 0) ? numThreads : 1;
    }

    if (useSlidingWindow(bytesToRead)) {
      if (_config._useThreads) {
        _fileBuffer.useSlidingWindowWithChunks(_reader.rowIndex(), begin, bytesToRead, _config._limitFileBufferSize, numThreads);
      } else {
        _fileBuffer.useSlidingWindow(_reader.rowIndex(), begin, bytesToRead, _config._limitFileBufferSize);
      }
    } else {
      _fileBuffer.useOneWindowWithChunks(_reader.rowIndex(), begin, bytesToRead, numThreads);
    }

    if (_fileBuffer.bytesRead() == 0) {
      return 0;
    }

    _reader.detectLineEndingType(*file);
  }

  // now start reading

  LexicalCast::AutoDot useDot(_config._useDot);

  QVector<QVector<AsciiFileData> >& slidingWindow = _fileBuffer.fileData();
  int sampleRead = 0;
  for (int i = 0; i < slidingWindow.size(); i++) {

    int read;
    if (_config._useThreads)
      read = parseWindowMultithreaded(slidingWindow[i], col, v, field);
    else
      read = parseWindowSinglethreaded(slidingWindow[i], col, v, field, sampleRead);

    // something went wrong abort reading
    if (read == 0) {
      _fileBuffer.clear();
      break;
    }

    sampleRead += read;
  }

  return sampleRead;
}


//-------------------------------------------------------------------------------------------
int AsciiSource::parseWindowSinglethreaded(QVector<AsciiFileData>& window, int col, double* v, const QString& field, int sRead)
{
  int read = 0;
  for (int i = 0; i< window.size(); i++) {
    Q_ASSERT(sRead ==  window[i].rowBegin());
    if (!window[i].read() || window[i].bytesRead() == 0)
      return 0;
    read += _reader.readFieldFromChunk(window[i], col, v, field);
  }
  return read;
}


//-------------------------------------------------------------------------------------------
int AsciiSource::parseWindowMultithreaded(QVector<AsciiFileData>& window, int col, double* v, const QString& field)
{
  if (!_fileBuffer.readWindow(window))
    return 0;

  QFutureSynchronizer<int> readFutures;
  foreach (const AsciiFileData& chunk, window) {
    QFuture<int> future = QtConcurrent::run(&_reader, &AsciiDataReader::readFieldFromChunk, chunk, col, v, field);
    readFutures.addFuture(future);
  }
  readFutures.waitForFinished();
  int sampleRead = 0;
  foreach (const QFuture<int> future, readFutures.futures()) {
    sampleRead += future.result();
  }
  return sampleRead;
}


//-------------------------------------------------------------------------------------------
QString AsciiSource::fileType() const 
{
  return asciiTypeString;
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::isEmpty() const 
{
  return _reader.numberOfFrames() < 1;
}


//-------------------------------------------------------------------------------------------
QStringList AsciiSource::scalarListFor(const QString& filename, AsciiSourceConfig*) 
{
  QFile file(filename);
  if (!AsciiFileBuffer::openFile(file)) {
    return QStringList();
  }
  return QStringList() << "FRAMES";
}


//-------------------------------------------------------------------------------------------
QStringList AsciiSource::stringListFor(const QString& filename, AsciiSourceConfig*) 
{
  QFile file(filename);
  if (!AsciiFileBuffer::openFile(file)) {
    return QStringList();
  }
  return QStringList() << "FILE";
}


//-------------------------------------------------------------------------------------------
int AsciiSource::splitHeaderLine(const QByteArray& line, const AsciiSourceConfig& cfg, QStringList* stringList)
{
  QStringList dummy;
  QStringList& parts(stringList ? *stringList : dummy);
  parts.clear();
  const QRegExp regexColumnDelimiter(QString("[%1]").arg(QRegExp::escape(cfg._columnDelimiter.value())));
  
  if (cfg._columnType == AsciiSourceConfig::Custom && !cfg._columnDelimiter.value().isEmpty()) {
    parts += QString(line).trimmed().split(regexColumnDelimiter, QString::SkipEmptyParts);
  } else if (cfg._columnType == AsciiSourceConfig::Fixed) {
    int cnt = line.length() / cfg._columnWidth;
    for (int i = 0; i < cnt; ++i) {
      QString sub = line.mid(i * cfg._columnWidth).left(cfg._columnWidth);
      parts += sub.trimmed();
    }
  } else {
    if (!stringList) {
      //MeasureTime t("AsciiDataReader::countColumns()");
      int columns = AsciiDataReader::splitColumns(line, AsciiCharacterTraits::IsWhiteSpace());
      Q_ASSERT(columns == QString(line).trimmed().split(QRegExp("\\s"), QString::SkipEmptyParts).size());
      return columns;
    } else {
      //MeasureTime t("AsciiDataReader::countColumns(parts)");
      AsciiDataReader::splitColumns(line, AsciiCharacterTraits::IsWhiteSpace(), &parts);
      Q_ASSERT(parts == QString(line).trimmed().split(QRegExp("\\s"), QString::SkipEmptyParts));
    }
  }
  return parts.count();
}


//-------------------------------------------------------------------------------------------
QStringList AsciiSource::fieldListFor(const QString& filename, AsciiSourceConfig* cfg) 
{
  QFile file(filename);
  if (!AsciiFileBuffer::openFile(file)) {
    return QStringList();
  }
  
  QStringList fields;
  fields += "INDEX";
  
  if (cfg->_readFields) {
    int fieldsLine = cfg->_fieldsLine;
    int currentLine = 0; // Explicit line counter, to make the code easier to understand
    while (currentLine < cfg->_dataLine) {
      const QByteArray line = file.readLine();
      int r = line.size();
      if (currentLine == fieldsLine && r >= 0) {
        QStringList parts;
        AsciiSource::splitHeaderLine(line, *cfg, &parts);
        fields += parts;
        break;
      }
      currentLine++;
    }
    QStringList trimmed;
    foreach(const QString& str, fields) {
      trimmed << str.trimmed();
    }
    return trimmed;
  }
  
  
  QRegExp regex;
  if (cfg->_columnType == AsciiSourceConfig::Custom && !cfg->_columnDelimiter.value().isEmpty()) {
    regex.setPattern(QString("^[%1]*[%2].*").arg(QRegExp::escape(cfg->_columnDelimiter.value())).arg(cfg->_delimiters));
  } else {
    regex.setPattern(QString("^\\s*[%1].*").arg(cfg->_delimiters));
  }
  
  bool done = false;
  int skip = cfg->_dataLine;
  //FIXME This is a hack which should eventually be fixed by specifying
  // the starting frame of the data when calling KstDataSource::fieldListForSource
  // and KstDataSource::fieldList.  If the skip value is not specified, then
  // we scan a few lines and take the maximum number of fields that we find.
  int maxcnt;
  if (skip > 0) {
    maxcnt = -1;
  } else {
    maxcnt = 0;
  }
  int cnt;
  int nextscan = 0;
  int curscan = 0;
  while (!file.atEnd() && !done && (nextscan < 200)) {
    QByteArray line = file.readLine();
    int r = line.size();
    if (skip > 0) { //keep skipping until desired line
      --skip;
      if (r < 0) {
        return fields;
      }
      continue;
    }
    if (maxcnt >= 0) { //original skip value == 0, so scan some lines
      if (curscan >= nextscan) {
        if (r > 1 && !regex.exactMatch(line)) {
          cnt = splitHeaderLine(line, *cfg);
          if (cnt > maxcnt) {
            maxcnt = cnt;
          }
        } else if (r < 0) {
          return fields;
        }
        nextscan += nextscan + 1;
      }
      curscan++;
      continue;
    }
    if (r > 1 && !regex.exactMatch(line)) { //at desired line, find count
      maxcnt = splitHeaderLine(line, *cfg);
      done = true;
    } else if (r < 0) {
      return fields;
    }
  }
  
  for (int i = 1; i <= maxcnt; ++i) {
    fields += i18n("Column %1").arg(i).trimmed();
  }
  
  return fields;
}


//-------------------------------------------------------------------------------------------
QStringList AsciiSource::unitListFor(const QString& filename, AsciiSourceConfig* cfg)
{
  QFile file(filename);
  if (!AsciiFileBuffer::openFile(file)) {
    return QStringList();
  }
  
  QStringList units;
  units += ""; // To go with INDEX
  
  int unitsLine = cfg->_unitsLine;
  int currentLine = 0;
  while (currentLine < cfg->_dataLine) {
    const QByteArray line = file.readLine();
    int r = line.size();
    if (currentLine == unitsLine && r >= 0) {
      QStringList parts;
      AsciiSource::splitHeaderLine(line, *cfg, &parts);
      units += parts;
      break;
    }
    currentLine++;
  }
  QStringList trimmed;
  foreach(const QString& str, units) {
    trimmed << str.trimmed();
  }
  return trimmed;
}


//-------------------------------------------------------------------------------------------
void AsciiSource::save(QXmlStreamWriter &s) 
{
  Kst::DataSource::save(s);
  _config.save(s);
}


//-------------------------------------------------------------------------------------------
void AsciiSource::parseProperties(QXmlStreamAttributes &properties) 
{
  _config.parseProperties(properties);
  reset();
  internalDataSourceUpdate();
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::supportsTimeConversions() const 
{
  return false; //fieldList().contains(_config._indexVector) && _config._indexInterpretation != AsciiSourceConfig::Unknown && _config._indexInterpretation != AsciiSourceConfig::INDEX;
}


//-------------------------------------------------------------------------------------------
int AsciiSource::sampleForTime(double ms, bool *ok) 
{
  switch (_config._indexInterpretation) {
  case AsciiSourceConfig::Seconds:
    // FIXME: make sure "seconds" exists in _indexVector
    if (ok) {
      *ok = true;
    }
    return 0;
  case AsciiSourceConfig::CTime:
    // FIXME: make sure "seconds" exists in _indexVector (different than above?)
    if (ok) {
      *ok = true;
    }
    return 0;
  default:
    return Kst::DataSource::sampleForTime(ms, ok);
  }
}


//-------------------------------------------------------------------------------------------
const QString& AsciiSource::typeString() const 
{
  return asciiTypeString;
}


//-------------------------------------------------------------------------------------------
int AsciiSource::sampleForTime(const QDateTime& time, bool *ok) 
{
  switch (_config._indexInterpretation) {
  case AsciiSourceConfig::Seconds:
    // FIXME: make sure "time" exists in _indexVector
    if (ok) {
      *ok = true;
    }
    return time.toTime_t();
  case AsciiSourceConfig::CTime:
    // FIXME: make sure "time" exists in _indexVector (different than above?)
    if (ok) {
      *ok = true;
    }
    return time.toTime_t();
  default:
    return Kst::DataSource::sampleForTime(time, ok);
  }
}


//-------------------------------------------------------------------------------------------
Kst::ObjectList<Kst::Object> AsciiSource::autoCurves(ObjectStore& objectStore)
{
  // here we could do more sophisticated stuff when generating a list of curves
  return ObjectList<Kst::Object>();
}



// vim: ts=2 sw=2 et
