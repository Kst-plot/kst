/***************************************************************************
                      ASCII file data source
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of Toronto
     email                : netterfield@astro.utoronto.ca
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
#include <iostream>


// Load faster in debug mode:
// disable QASSERT when using [] on data
#define KST_DONT_CHECK_INDEX_IN_DEBUG

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
// String interface
//

class DataInterfaceAsciiString : public DataSource::DataInterface<DataString>
{
public:
  DataInterfaceAsciiString(AsciiSource& s) : ascii(s) {}

  // read one element
  int read(const QString&, DataString::ReadInfo&);

  // named elements
  QStringList list() const { return ascii._strings.keys(); }
  bool isListComplete() const { return true; }
  bool isValid(const QString&) const;

  // T specific
  const DataString::DataInfo dataInfo(const QString&) const { return DataString::DataInfo(); }
  void setDataInfo(const QString&, const DataString::DataInfo&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&) { return QMap<QString, double>(); }
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


private:
  AsciiSource& ascii;
};


//-------------------------------------------------------------------------------------------
int DataInterfaceAsciiString::read(const QString& string, DataString::ReadInfo& p)
{
  if (isValid(string) && p.value) {
    *p.value = ascii._strings[string];
    return 1;
  }
  return 0;
}


//-------------------------------------------------------------------------------------------
bool DataInterfaceAsciiString::isValid(const QString& string) const
{
  return  ascii._strings.contains( string );
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
  _tmpBuffer.clear();
  _rowIndex.clear();

  _valid = false;
  _byteLength = 0;
  _numFrames = 0;
  _haveHeader = false;
  _fieldListComplete = false;

  _fieldList.clear();
  _scalarList.clear();
  _strings.clear();

  Object::reset();

  _strings = fileMetas();
}


//-------------------------------------------------------------------------------------------
bool AsciiSource::openFile(QFile &file) 
{
  // Don't use 'QIODevice::Text'!
  // Because CR LF line ending breaks row offset calculation
  return file.open(QIODevice::ReadOnly);
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
      _strings[QString("Header %1").arg(header_row, 2, 10, QChar('0'))] = QString::fromAscii(line).trimmed();
      header_row++;
    }
    _rowIndex[0] = didRead;
  }

  return true;
}


//-------------------------------------------------------------------------------------------
AsciiSource::LineEndingType AsciiSource::detectLineEndingType(QFile& file) const
{
  LineEndingType end;
  QByteArray line = file.readLine();
  file.seek(0);
  int lsize = line.size();
  end.is_crlf = line[lsize - 2] == '\r' && line[lsize - 1] == '\n' ;
  end.character =  end.is_crlf ? line[lsize - 2] : line[lsize - 1];
  return end;
}


//-------------------------------------------------------------------------------------------
Kst::Object::UpdateType AsciiSource::internalDataSourceUpdate()
{
  return internalDataSourceUpdate(true);
}


//-------------------------------------------------------------------------------------------
#define MAXBUFREADLEN KST_PREALLOC
Kst::Object::UpdateType AsciiSource::internalDataSourceUpdate(bool read_completely)
{
  MeasureTime t("AsciiSource::internalDataSourceUpdate");

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
  }

  QFile file(_filename);
  if (!openValidFile(file)) {
    // Qt: If the device is closed, the size returned will not reflect the actual size of the device.
    return NoChange;
  }

  LineEndingType lineending = detectLineEndingType(file);

  bool new_data = false;
  bool force_update = true;
  if (_byteLength == file.size()) {
    force_update = false;
  }
  _byteLength = file.size();

  int bufread = 0;
  int bufstart = _rowIndex[_numFrames];
  do {
    // Read the tmpbuffer, starting at row_index[_numFrames]
    QVarLengthArray<char, MAXBUFREADLEN + 1> varBuffer;
    varBuffer.resize(varBuffer.capacity());

    bufstart += bufread;
    bufread = readFromFile(file, varBuffer, bufstart, _byteLength - bufstart, MAXBUFREADLEN);

#ifdef KST_DONT_CHECK_INDEX_IN_DEBUG
    const char* buffer = varBuffer.constData();
    const char* bufferData = buffer;
#else
    QVarLengthArray<char, MAXBUFREADLEN + 1>& buffer = varBuffer;
    const char* bufferData = buffer.data();
#endif


    if (_config._delimiters.value().size() == 0) {
      const NoDelimiter comment_del;
      if (lineending.isLF()) {
        new_data = findDataRows(buffer, bufstart, bufread, IsLineBreakLF(lineending), comment_del);
      } else {
        new_data = findDataRows(buffer, bufstart, bufread, IsLineBreakCR(lineending), comment_del);
      }
    } else if (_config._delimiters.value().size() == 1) {
      const IsCharacter comment_del(_config._delimiters.value()[0].toAscii());
      if (lineending.isLF()) {
        new_data = findDataRows(buffer, bufstart, bufread, IsLineBreakLF(lineending), comment_del);
      } else {
        new_data = findDataRows(buffer, bufstart, bufread, IsLineBreakCR(lineending), comment_del);
      }
    } else if (_config._delimiters.value().size() > 1) {
      const IsInString comment_del(_config._delimiters.value());
      if (lineending.isLF()) {
        new_data = findDataRows(buffer, bufstart, bufread, IsLineBreakLF(lineending), comment_del);
      } else {
        new_data = findDataRows(buffer, bufstart, bufread, IsLineBreakCR(lineending), comment_del);
      }
    }

  } while (bufread == MAXBUFREADLEN  && read_completely);

  _rowIndex.resize(_numFrames + 1);

  return (!new_data && !force_update ? NoChange : Updated);
}


template<typename IsLineBreak, typename CommentDelimiter>
bool AsciiSource::findDataRows(const char* buffer, int bufstart, int bufread, const IsLineBreak& isLineBreak, const CommentDelimiter& comment_del)
{
  const IsWhiteSpace isWhiteSpace;
  
  bool new_data = false;
  bool is_data = false;
  bool is_comment = false;

  const int row_offset = bufstart + isLineBreak.size;

  for (int i = 0; i < bufread; i++) {
      if (comment_del(buffer[i])) {
        is_comment = true;
      } else if (isLineBreak(buffer[i])) {
        is_comment = false;
        if (is_data) {
          is_data = false;
          ++_numFrames;
          if (_numFrames >= _rowIndex.size()) {
            _rowIndex.resize(_rowIndex.size() + MAXBUFREADLEN);
          }
          _rowIndex[_numFrames] = row_offset + i;
          new_data = true;
        }
      } else if (!is_data && !isWhiteSpace(buffer[i]) && !comment_del(buffer[i])) {
        is_data = is_comment ? false : true;
      }
  }
  return new_data;
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
int AsciiSource::readField(double *v, const QString& field, int s, int n) 
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
  
  LineEndingType lineending = detectLineEndingType(file);

  bufread = readFromFile(file, _tmpBuffer, bufstart, bufread);

#ifdef KST_DONT_CHECK_INDEX_IN_DEBUG
  const char* buffer = _tmpBuffer.constData();
#else
  const QVarLengthArray<char, KST_PREALLOC>& buffer = _tmpBuffer;
#endif

  if (_config._columnType == AsciiSourceConfig::Fixed) {
    MeasureTime t("AsciiSource::readField: same width for all columns");
    LexicalCast lexc;
    lexc.setDecimalSeparator(_config._useDot);
    const char* col_start = &buffer[0] + _config._columnWidth * (col - 1);
    for (int i = 0; i < n; ++i) {
      /* Read appropriate column and convert to double
      v[i] = lexc.toDouble(&buffer[0] + _rowIndex[i] + _config._columnWidth * (col - 1));*/
      v[i] = lexc.toDouble(col_start + _rowIndex[i]);
    }
    return n;
  } else if (_config._columnType == AsciiSourceConfig::Custom) {
    if (_config._columnDelimiter.value().size() == 1) {
      MeasureTime t("AsciiSource::readField: 1 custom column delimiter");
      const IsCharacter column_del(_config._columnDelimiter.value()[0].toAscii());
      return readColumns(v, buffer, bufstart, bufread, col, s, n, lineending, column_del);
    } if (_config._columnDelimiter.value().size() > 1) {
      MeasureTime t(QString("AsciiSource::readField: %1 custom column delimiters").arg(_config._columnDelimiter.value().size()));
      const IsInString column_del(_config._columnDelimiter.value());
      return readColumns(v, buffer, bufstart, bufread, col, s, n, lineending, column_del);
    }
  } else if (_config._columnType == AsciiSourceConfig::Whitespace) {
    MeasureTime t("AsciiSource::readField: whitespace separated columns");
    const IsWhiteSpace column_del;
    return readColumns(v, buffer, bufstart, bufread, col, s, n, lineending, column_del);
  }

  return 0;
}


//-------------------------------------------------------------------------------------------
template<typename ColumnDelimiter>
int AsciiSource::readColumns(double* v, const char* buffer, int bufstart, int bufread, int col, int s, int n,
                              const LineEndingType& lineending, const ColumnDelimiter& column_del)
{

  if (_config._delimiters.value().size() == 0) {
    const NoDelimiter comment_del;
    return readColumns(v, buffer, bufstart, bufread, col, s, n, lineending, column_del, comment_del);
  } else if (_config._delimiters.value().size() == 1) {
    const IsCharacter comment_del(_config._delimiters.value()[0].toAscii());
    return readColumns(v, buffer, bufstart, bufread, col, s, n, lineending, column_del, comment_del);
  } else if (_config._delimiters.value().size() > 1) {
    const IsInString comment_del(_config._delimiters.value());
    return readColumns(v, buffer, bufstart, bufread, col, s, n, lineending, column_del, comment_del);
  }

  return 0;
}

template<typename ColumnDelimiter, typename CommentDelimiter>
int AsciiSource::readColumns(double* v, const char* buffer, int bufstart, int bufread, int col, int s, int n,
                              const LineEndingType& lineending, const ColumnDelimiter& column_del, const CommentDelimiter& comment_del)
{
  if (_config._columnWidthIsConst) {
    const AlwaysTrue column_withs_const;
    if (lineending.isLF()) {
      return readColumns(v, buffer, bufstart, bufread, col, s, n, IsLineBreakLF(lineending), column_del, comment_del, column_withs_const);
    } else {
      return readColumns(v, buffer, bufstart, bufread, col, s, n, IsLineBreakCR(lineending), column_del, comment_del, column_withs_const);
    }
  } else {
    const AlwaysFalse column_withs_const;
    if (lineending.isLF()) {
      return readColumns(v, buffer, bufstart, bufread, col, s, n, IsLineBreakLF(lineending), column_del, comment_del, column_withs_const);
    } else {
      return readColumns(v, buffer, bufstart, bufread, col, s, n, IsLineBreakCR(lineending), column_del, comment_del, column_withs_const);
    }
  }
}


template<typename IsLineBreak, typename ColumnDelimiter, typename CommentDelimiter, typename ColumnWidthsAreConst>
int AsciiSource::readColumns(double* v, const char* buffer, int bufstart, int bufread, int col, int s, int n,
                              const IsLineBreak& isLineBreak,
                              const ColumnDelimiter& column_del, const CommentDelimiter& comment_del,
                              const ColumnWidthsAreConst& are_column_widths_const)
{
  LexicalCast lexc;
  lexc.setDecimalSeparator(_config._useDot);
  const QString delimiters = _config._delimiters.value();

  int col_start = -1;
  for (int i = 0; i < n; i++, s++) {
    bool incol = false;
    int i_col = 0;

    if (are_column_widths_const()) {
      if (col_start != -1) {
        v[i] = lexc.toDouble(&buffer[0] + _rowIndex[s] + col_start);
        continue;
      }
    }

    v[i] = Kst::NOPOINT;
    for (int ch = _rowIndex[s] - bufstart; ch < bufread; ++ch) {
      if (isLineBreak(buffer[ch])) {
        break;
      } else if (column_del(buffer[ch])) { //<- check for column start
        incol = false;
      } else if (comment_del(buffer[ch])) {
        break;
      } else {
        if (!incol) {
          incol = true;
          ++i_col;
          if (i_col == col) {
            toDouble(lexc, buffer, bufread, ch, &v[i], i);
            if (are_column_widths_const()) {
              if (col_start == -1) {
                col_start = ch - _rowIndex[s];
              }
            }
            break;
          }
        }
      }
    }
  }
  return n;
}



//-------------------------------------------------------------------------------------------
void AsciiSource::toDouble(const LexicalCast& lexc, const char* buffer, int bufread, int ch, double* v, int row)
{
  if (   isDigit(buffer[ch])
      || buffer[ch] == '-'
      || buffer[ch] == '.'
      || buffer[ch] == '+'
      || isWhiteSpace(buffer[ch])) {
    *v = lexc.toDouble(&buffer[0] + ch);
  } else if ( ch + 2 < bufread
              && tolower(buffer[ch]) == 'i'
              && tolower(buffer[ch + 1]) == 'n'
              && tolower(buffer[ch + 2]) == 'f') {
    *v = INF;
  }

#if 0
  // TODO enable by option: "Add unparsable lines as strings"
  else {
    if (_rowIndex.size() > row + 1) {
      QString unparsable = QString::fromAscii(&buffer[_rowIndex[row]], _rowIndex[row + 1] - _rowIndex[row]);
      _strings[QString("Unparsable %1").arg(row)] = unparsable.trimmed();
    }
  }
#endif

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
  QFile file(filename);
  if (!openFile(file)) {
    return QStringList();
  }
  return QStringList() << "FRAMES";
}


//-------------------------------------------------------------------------------------------
QStringList AsciiSource::stringListFor(const QString& filename, AsciiSourceConfig*) 
{
  QFile file(filename);
  if (!openFile(file)) {
    return QStringList();
  }
  return QStringList() << "FILE";
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
  const QRegExp regexColumnDelimiter(QString("[%1]").arg(QRegExp::escape(columnDelimiter)));
  if (cfg->_readFields) {
    int l = cfg->_fieldsLine;
    while (!file.atEnd()) {
      const QByteArray line = file.readLine();
      int r = line.size();
      if (l-- == 0) {
        if (r >= 0) {
          if (cfg->_columnType == AsciiSourceConfig::Custom && !columnDelimiter.isEmpty()) {
            rc += QString(line).trimmed().split(regexColumnDelimiter, QString::SkipEmptyParts);
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
    QStringList trimmed;
    foreach(const QString& str, rc) {
      trimmed << str.trimmed();
    }
    return trimmed;
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
            cnt = QString(line).split(regexColumnDelimiter, QString::SkipEmptyParts).count();
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
        maxcnt = QString(line).split(regexColumnDelimiter, QString::SkipEmptyParts).count();
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
    rc += i18n("Column %1").arg(i).trimmed();
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
