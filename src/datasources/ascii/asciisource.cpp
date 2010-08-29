/***************************************************************************
                      ASCII file data source
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of Toronto
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "asciisource.h"
#include "asciisourceconfig.h"

#include "math_kst.h"
#include "kst_inf.h"
#include "kst_i18n.h"
#include "kst_atof.h"
#include "measuretime.h"

#include <QFile>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>



using namespace Kst;


//
// Vector interface
//


//-------------------------------------------------------------------------------------------
class DataInterfaceAsciiVector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceAsciiVector(AsciiSource& a) : ascii(a) {}

  // read one element
  int read(const QString&, DataVector::ReadInfo&);

  // named elements
  QStringList list() const { return ascii._fieldList; }
  bool isListComplete() const { return ascii._fieldListComplete; }
  bool isValid(const QString& field) const { return ascii._fieldList.contains( field ); }

  // T specific
  const DataVector::DataInfo dataInfo(const QString&) const;
  void setDataInfo(const QString&, const DataVector::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  AsciiSource& ascii;
};


//-------------------------------------------------------------------------------------------
const DataVector::DataInfo DataInterfaceAsciiVector::dataInfo(const QString &field) const
{
  if (!ascii._fieldList.contains(field))
    return DataVector::DataInfo();

  return DataVector::DataInfo(ascii._numFrames, 1);
}


//-------------------------------------------------------------------------------------------
int DataInterfaceAsciiVector::read(const QString& field, DataVector::ReadInfo& p)
{
  return ascii.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


//-------------------------------------------------------------------------------------------
// TODO FRAMES only in vector?
QMap<QString, double> DataInterfaceAsciiVector::metaScalars(const QString&)
{
  QMap<QString, double> m;
  m["FRAMES"] = ascii._numFrames;;
  return m;
}





//
// AsciiSource
//

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
  _tmpBuffer(),
  _rowIndex(),
  iv(new DataInterfaceAsciiVector(*this))
{
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
  internalDataSourceUpdate();
}


//-------------------------------------------------------------------------------------------
AsciiSource::~AsciiSource() 
{
}


//-------------------------------------------------------------------------------------------
void AsciiSource::reset() 
{
  _tmpBuffer.clear();
  _rowIndex.clear();

  _valid = false;
  _byteLength = 0;
  _numFrames = 0;
  _haveHeader = false;
  _fieldListComplete = false;

  _fieldList.clear();
  _scalarList.clear();
  _stringList.clear();

  Object::reset();
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::openFile(QFile &file) 
{
  return file.open(QIODevice::ReadOnly | QIODevice::Text);
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::openValidFile(QFile &file) 
{
  _valid = openFile(file);
  return _valid;
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::initRowIndex() 
{
  // capacity is at least the pre-allocated memory
  _rowIndex.resize(_rowIndex.capacity());

  _rowIndex[0] = 0;
  _byteLength = 0;
  _numFrames = 0;

  if (_config._dataLine > 0) {
    QFile file(_filename);
    if (!openValidFile(file)) {
      return false;
    }
    int left = _config._dataLine;
    int didRead = 0;    
    while (left > 0) {
      QByteArray line = file.readLine();
      if (line.isEmpty() || file.atEnd()) {
        return false;
      }
      didRead += line.size();
      --left;
    }
    _rowIndex[0] = didRead;
  }

  return true;
}


//-------------------------------------------------------------------------------------------
#define MAXBUFREADLEN 32768
Kst::Object::UpdateType AsciiSource::internalDataSourceUpdate() 
{
  if (!_haveHeader) {
    _haveHeader = initRowIndex();
    if (!_haveHeader) {
      return NoChange;
    }
    // Re-update the field list since we have one now
    _fieldList = fieldListFor(_filename, &_config);
    _fieldListComplete = _fieldList.count() > 1;

    // Re-update the scalar list since we have one now
    _scalarList = scalarListFor(_filename, &_config);
    _stringList = stringListFor(_filename, &_config);
  }

  QFile file(_filename);
  if (!openValidFile(file)) {
    // Qt: If the device is closed, the size returned will not reflect the actual size of the device.
    return NoChange;
  }
  
  bool forceUpdate;
  if (_byteLength == file.size()) {
    forceUpdate = false;
  } else {
    forceUpdate = true;
    _byteLength = file.size();
  }

  int bufread;
  bool new_data = false;
  bool first_read = (_numFrames == 0);

  QByteArray delbytes = _config._delimiters.value().toLatin1();
  const char *del = delbytes.constData();

  do {
    // Read the tmpbuffer, starting at row_index[_numFrames]
    QVarLengthArray<char, MAXBUFREADLEN + 1> tmpbuf;
    tmpbuf.resize(tmpbuf.capacity());
    int bufstart = _rowIndex[_numFrames];
    bufread = readFromFile(file, tmpbuf, bufstart, _byteLength - bufstart, MAXBUFREADLEN);    

    bool is_comment = false, has_dat = false;
    char *comment = strpbrk(tmpbuf.data(), del);
    for (int i = 0; i < bufread; i++) {
      if (comment == &(tmpbuf[i])) {
        is_comment = true;
      } else if (tmpbuf[i] == '\n' || tmpbuf[i] == '\r') {
        if (has_dat) {
          ++_numFrames;
          if (_numFrames >= _rowIndex.size()) {
            _rowIndex.resize(_rowIndex.size() + 32768);
            if (_numFrames >= _rowIndex.size()) {
              // TODO where could we report an error;
              return NoChange;
            }
          }
          new_data = true;
        }
        _rowIndex[_numFrames] = bufstart + i + 1;
        has_dat = is_comment = false;
        if (comment && comment < &(tmpbuf[i])) {
          comment = strpbrk(&(tmpbuf[i]), del);
        }
      } else if (!is_comment && !isspace(tmpbuf[i])) {  
        // FIXME: this breaks custom delimiters
        has_dat = true;
      }
    }
  } while ((bufread == MAXBUFREADLEN) && (!first_read));

  return (forceUpdate ? Updated : (new_data ? Updated : NoChange));
}

//-------------------------------------------------------------------------------------------
/* TODO needed?
int AsciiSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  }
  return 0;
}
*/


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
int AsciiSource::readField(double *v, const QString& field, int s, int n) 
{
  LexicalCast lexc;
  lexc.setDecimalSeparator(_config._useDot, _config._localSeparator);

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
    return 0;
  }

  int bufstart = _rowIndex[s];
  int bufread = _rowIndex[s + n] - bufstart;
  if (bufread <= 0) {
    return 0;
  }

  QFile file(_filename);
  if (!openValidFile(file)) {
    return 0;
  }
  bufread = readFromFile(file, _tmpBuffer, bufstart, bufread);    

  if (_config._columnType == AsciiSourceConfig::Fixed) {
    for (int i = 0; i < n; ++i, ++s) {
      // Read appropriate column and convert to double
      v[i] = lexc.toDouble(&_tmpBuffer[0] + _rowIndex[i] - _rowIndex[0] + _config._columnWidth * (col - 1));
    }
  } else if (_config._columnType == AsciiSourceConfig::Custom) {
    const QString delimiters = _config._delimiters.value();
    const QString columnDelimiter = _config._columnDelimiter.value();
    for (int i = 0; i < n; ++i, ++s) {
      bool incol = false;
      int i_col = 0;
      v[i] = Kst::NOPOINT;
      for (int ch = _rowIndex[s] - bufstart; ch < bufread; ++ch) {
        if (columnDelimiter.contains(_tmpBuffer[ch])) {
          incol = false;
        } else if (_tmpBuffer[ch] == '\n' || _tmpBuffer[ch] == '\r') {
          break;
        } else if (delimiters.contains(_tmpBuffer[ch])) {
          break;
        } else {
          if (!incol) {
            incol = true;
            ++i_col;
            if (i_col == col) {
              if (isdigit(_tmpBuffer[ch]) || _tmpBuffer[ch] == '-' || _tmpBuffer[ch] == '.' || _tmpBuffer[ch] == '+') {
                v[i] = lexc.toDouble(&_tmpBuffer[0] + ch);
              } else if (ch + 2 < bufread && tolower(_tmpBuffer[ch]) == 'i' &&
                  tolower(_tmpBuffer[ch + 1]) == 'n' && tolower(_tmpBuffer[ch + 2]) == 'f') {
                v[i] = INF;
              }
              break;
            }
          }
        }
      }
    }
  } else if (_config._columnType == AsciiSourceConfig::Whitespace) {
    const QString delimiters = _config._delimiters.value();
    for (int i = 0; i < n; i++, s++) {
      bool incol = false;
      int i_col = 0;

      v[i] = Kst::NOPOINT;
      for (int ch = _rowIndex[s] - bufstart; ch < bufread; ++ch) {
        if (isspace(_tmpBuffer[ch])) {
          if (_tmpBuffer[ch] == '\n' || _tmpBuffer[ch] == '\r') {
            break;
          } else {
            incol = false;
          }
        } else if (delimiters.contains(_tmpBuffer[ch])) {
          break;
        } else {
          if (!incol) {
            incol = true;
            ++i_col;
            if (i_col == col) {
              if (isdigit(_tmpBuffer[ch]) || _tmpBuffer[ch] == '-' || _tmpBuffer[ch] == '.' || _tmpBuffer[ch] == '+') {
                v[i] = lexc.toDouble(&_tmpBuffer[0] + ch);
              } else if (ch + 2 < bufread && tolower(_tmpBuffer[ch]) == 'i' &&
                  tolower(_tmpBuffer[ch + 1]) == 'n' && tolower(_tmpBuffer[ch + 2]) == 'f') {
                v[i] = INF;
              }
              break;
            }
          }
        }
      }
    }
  } else {
    return 0;
  }

  return n;
}


//-------------------------------------------------------------------------------------------
QString AsciiSource::fileType() const 
{
  return asciiTypeString;
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::isEmpty() const 
{
  return _numFrames < 1;
}


//-------------------------------------------------------------------------------------------
QStringList AsciiSource::scalarListFor(const QString& filename, AsciiSourceConfig*) 
{
  QStringList rc;
  QFile file(filename);
  if (!openFile(file)) {
    return rc;
  }
  rc += "FRAMES";
  return rc;
}

//-------------------------------------------------------------------------------------------
QStringList AsciiSource::stringListFor(const QString& filename, AsciiSourceConfig*) 
{
  QStringList rc;
  QFile file(filename);
  if (!openFile(file)) {
    return rc;
  }
  rc += "FILE";
  return rc;
}


//-------------------------------------------------------------------------------------------
QStringList AsciiSource::fieldListFor(const QString& filename, AsciiSourceConfig* cfg) 
{
  QStringList rc;
  QFile file(filename);
  if (!openFile(file)) {
    return rc;
  }

  rc += "INDEX";

  const QString columnDelimiter = cfg->_columnDelimiter.value();
  const QRegExp regexColumnDelemiter(QString("[%1]").arg(QRegExp::escape(columnDelimiter)));
  if (cfg->_readFields) {
    int l = cfg->_fieldsLine;
    while (!file.atEnd()) {
      const QByteArray line = file.readLine();
      int r = line.size();
      if (l-- == 0) {
        if (r >= 0) {
          if (cfg->_columnType == AsciiSourceConfig::Custom && !columnDelimiter.isEmpty()) {
            rc += QString(line).trimmed().split(regexColumnDelemiter, QString::SkipEmptyParts);
          } else if (cfg->_columnType == AsciiSourceConfig::Fixed) {
            int cnt = line.length() / cfg->_columnWidth;
            for (int i = 0; i < cnt; ++i) {
              QString sub = line.mid(i * cfg->_columnWidth).left(cfg->_columnWidth);
              rc += sub.trimmed();
            }
          } else {
            rc += QString(line).trimmed().split(QRegExp("[\\s]"), QString::SkipEmptyParts);
          }
        }
        break;
      }
    }
    return rc;
  }

  QRegExp regex;
  if (cfg->_columnType == AsciiSourceConfig::Custom && !columnDelimiter.isEmpty()) {
    regex.setPattern(QString("^[%1]*[%2].*").arg(QRegExp::escape(columnDelimiter)).arg(cfg->_delimiters));
  } else {
    regex.setPattern(QString("^\\s*[%1].*").arg(cfg->_delimiters));
  }

  const QRegExp regexS("\\s");

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
        return rc;
      }
      continue;
    }
    if (maxcnt >= 0) { //original skip value == 0, so scan some lines
      if (curscan >= nextscan) {
        if (r > 1 && !regex.exactMatch(line)) {
          line = line.trimmed();
          if (cfg->_columnType == AsciiSourceConfig::Custom && !columnDelimiter.isEmpty()) {
            cnt = QString(line).split(regexColumnDelemiter, QString::SkipEmptyParts).count();
          } else if (cfg->_columnType == AsciiSourceConfig::Fixed) {
            cnt = line.length() / cfg->_columnWidth;
          } else {
            cnt = QString(line).split(regexS, QString::SkipEmptyParts).count();
          }
          if (cnt > maxcnt) {
            maxcnt = cnt;
          }
        } else if (r < 0) {
          return rc;
        }
        nextscan += nextscan + 1;
      }
      curscan++;
      continue;
    }
    if (r > 1 && !regex.exactMatch(line)) { //at desired line, find count
      line = line.trimmed();
      if (cfg->_columnType == AsciiSourceConfig::Custom && !columnDelimiter.isEmpty()) {
        maxcnt = QString(line).split(regexColumnDelemiter, QString::SkipEmptyParts).count();
      } else if (cfg->_columnType == AsciiSourceConfig::Fixed) {
        maxcnt = line.length() / cfg->_columnWidth;
      } else {
        maxcnt = QString(line).split(regexS, QString::SkipEmptyParts).count();
      }
      done = true;
    } else if (r < 0) {
      return rc;
    }
  }

  for (int i = 1; i <= maxcnt; ++i) {
    rc += i18n("Column %1").arg(i);
  }

  return rc;
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


// vim: ts=2 sw=2 et
