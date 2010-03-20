/***************************************************************************
                     ascii.cpp  -  ASCII file data source
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 The University of Toronto
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


#include "asciisource.h"
#include "asciisourceconfig.h"

#include "math_kst.h"
#include "kst_inf.h"
#include "kst_i18n.h"
#include "measuretime.h"

#include <QFile>
#include <QLocale>

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>


#ifdef Q_OS_WIN
#define USE_KST_ATOF
#endif

#ifdef USE_KST_ATOF
#include "kst_atof.h"
#define atof(X, Y) kst_atof(X, Y)
#else
#define atof(X, Y) atof(X)
#endif

using namespace Kst;


//
// Vector interface
//

class DataInterfaceAsciiVector : public DataSource::DataInterface<DataVector>
{
public:
  DataInterfaceAsciiVector(AsciiSource& a) : ascii(a) {}

  // read one element
  int read(const QString&, const DataVector::Param&);

  // named elements
  QStringList list() const { return ascii._fieldList; }
  bool isListComplete() const { return ascii._fieldListComplete; }
  bool isValid(const QString& field) const { return ascii._fieldList.contains( field ); }

  // T specific
  const DataVector::Optional optional(const QString&) const;
  void setOptional(const QString&, const DataVector::Optional&) {}

  // meta data
  QMap<QString, double> metaScalars(const QString&);
  QMap<QString, QString> metaStrings(const QString&) { return QMap<QString, QString>(); }


  AsciiSource& ascii;
};



const DataVector::Optional DataInterfaceAsciiVector::optional(const QString &field) const
{
  DataVector::Optional opt = {-1, -1, -1};
  if (!ascii._fieldList.contains(field))
    return opt;

  opt.samplesPerFrame = 1;
  opt.frameCount = ascii._numFrames;
  return opt;
}


int DataInterfaceAsciiVector::read(const QString& field, const DataVector::Param& p)
{
  return ascii.readField(p.data, field, p.startingFrame, p.numberOfFrames);
}


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

static const QString asciiTypeString = I18N_NOOP("ASCII file");

const QString AsciiSource::asciiTypeKey()
{
  return asciiTypeString;
}


AsciiSource::AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e) :
  Kst::DataSource(store, cfg, filename, type),
    _rowIndex(0L),
    _config(0L),
    _tmpBuf(0L),
    _tmpBufSize(0),
    iv(new DataInterfaceAsciiVector(*this))
{
  setInterface(iv);

  //TIME_IN_SCOPE(Ctor_AsciiSource);

  setUpdateType(File);

  _valid = false;
  _haveHeader = false;
  _fieldListComplete = false;
  _source = asciiTypeString;
  if (!type.isEmpty() && type != asciiTypeString) {
    return;
  }
  _config = new AsciiSourceConfig;
  _config->readGroup(*cfg, filename);
  if (!e.isNull()) {
    _config->load(e);
  }

  _valid = true;
  registerChange();
  internalDataSourceUpdate();
}


AsciiSource::~AsciiSource() {
  if (_tmpBuf) {
    free(_tmpBuf);
    _tmpBuf = 0L;
    _tmpBufSize = 0;
  }

  if (_rowIndex) {
    free(_rowIndex);
    _rowIndex = 0L;
    _numLinesAlloc = 0;
  }

  delete _config;
  _config = 0L;
}


void AsciiSource::reset() {
  if (_tmpBuf) {
    free(_tmpBuf);
    _tmpBuf = 0L;
    _tmpBufSize = 0;
  }

  if (_rowIndex) {
    free(_rowIndex);
    _rowIndex = 0L;
    _numLinesAlloc = 0;
  }
  _numFrames = 0;
  _haveHeader = false;
  _fieldListComplete = false;
  _fieldList.clear();
  _scalarList.clear();
  _stringList.clear();

  Object::reset();
}


int AsciiSource::readFullLine(QFile &file, QByteArray &str) {
  str = file.readLine(1000);
  if (str.isEmpty())
    return str.size();

  QByteArray strExtra;
  while (str[str.size()-1] != '\n') {
    strExtra = file.readLine(1000);
    if (!strExtra.isEmpty()) {
      str += strExtra;
    } else {
      break;
    }
  }

  return str.size();
}


bool AsciiSource::initRowIndex() {
  if (!_rowIndex) {
    _rowIndex = (int *)malloc(32768 * sizeof(int));
    _numLinesAlloc = 32768;
  }
  _rowIndex[0] = 0;
  _byteLength = 0;
  _numFrames = 0;

  if (_config->_dataLine > 0) {
    QFile file(_filename);
    if (!file.open(QIODevice::ReadOnly)) {
      return false;
    }
    int left = _config->_dataLine;
    int didRead = 0;
    QByteArray ignore;
    while (left > 0) {
      int thisRead = AsciiSource::readFullLine(file, ignore);
      if (thisRead <= 0 || file.atEnd()) {
        return false;
      }
      didRead += thisRead;
      --left;
    }
    _rowIndex[0] = didRead;
  }

  return true;
}


#define MAXBUFREADLEN 32768
Kst::Object::UpdateType AsciiSource::internalDataSourceUpdate() {
  if (!_haveHeader) {
    _haveHeader = initRowIndex();
    if (!_haveHeader) {
      return NoChange;
    }
    // Re-update the field list since we have one now
    _fieldList = fieldListFor(_filename, _config);
    _fieldListComplete = _fieldList.count() > 1;

    // Re-update the scalar list since we have one now
    _scalarList = scalarListFor(_filename, _config);
    _stringList = stringListFor(_filename, _config);
  }

  bool forceUpdate = false;
  QFile file(_filename);
  if (file.exists()) {
    if (uint(_byteLength) != file.size() || !_valid) {
      forceUpdate = true;
    }
    _byteLength = file.size();
  } else {
    _valid = false;
    return NoChange;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    // quietly fail - no data to be had here
    _valid = false;
    return NoChange;
  }

  _valid = true;

  int bufstart, bufread;
  bool new_data = false;
  char tmpbuf[MAXBUFREADLEN+1];
  QByteArray delbytes = _config->_delimiters.value().toLatin1();
  const char *del = delbytes.constData();

  bool first_read = (_numFrames==0);
  do {
    /* Read the tmpbuffer, starting at row_index[_numFrames] */
    if (_byteLength - _rowIndex[_numFrames] > MAXBUFREADLEN) {
      bufread = MAXBUFREADLEN;
    } else {
      bufread = _byteLength - _rowIndex[_numFrames];
    }

    bufstart = _rowIndex[_numFrames];
    file.seek(bufstart); // expensive?
    file.read(tmpbuf, bufread);
    tmpbuf[bufread] = '\0';

    bool is_comment = false, has_dat = false;
    char *comment = strpbrk(tmpbuf, del);
    for (int i = 0; i < bufread; i++) {
      if (comment == &(tmpbuf[i])) {
        is_comment = true;
      } else if (tmpbuf[i] == '\n' || tmpbuf[i] == '\r') {
        if (has_dat) {
          ++_numFrames;
          if (_numFrames >= _numLinesAlloc) {
            _numLinesAlloc += 32768;
            _rowIndex = (int *)realloc(_rowIndex, _numLinesAlloc*sizeof(int));
          }
          new_data = true;
        }
        _rowIndex[_numFrames] = bufstart + i + 1;
        has_dat = is_comment = false;
        if (comment && comment < &(tmpbuf[i])) {
          comment = strpbrk(&(tmpbuf[i]), del);
        }
      } else if (!is_comment && !isspace(tmpbuf[i])) {  // FIXME: this breaks
                                                        // custom delimiters
        has_dat = true;
      }
    }
  } while ((bufread == MAXBUFREADLEN) && (!first_read));

  file.close();
  return (forceUpdate ? Updated : (new_data ? Updated : NoChange));
}





/* TODO needed?
int AsciiSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  }
  return 0;
}
*/


struct NumberLocale
{
  NumberLocale() : use_dot(false) {
  }

  ~NumberLocale() {
    if (use_dot) {
      //printf("original LC_NUMERIC: %s\n", orig.constData());
      setlocale(LC_NUMERIC, orig.constData());
    }
  }

  void useDot() {
    use_dot = true;
    orig = QByteArray((const char*) setlocale(LC_NUMERIC, 0));
    setlocale(LC_NUMERIC, "C");
  }

private:
  bool use_dot;
  QByteArray orig;
};


int AsciiSource::readField(double *v, const QString& field, int s, int n) {

  NumberLocale dot;
  char sep = _config->_localSeparator;

  if (_config->_useDot) {
#ifdef USE_KST_ATOF
    sep = '.';
#else
    (void) sep;
    dot.useDot();
#endif
  }

  if (n < 0) {
    n = 1; /* n < 0 means read one sample, not frame - irrelevent here */
  }

  if (field == "INDEX") {
    for (int i = 0; i < n; i++) {
      v[i] = double(s + i);
    }
    return n;
  }

  QStringList fieldList = _fieldList;
  int col = 0;
  for (QStringList::ConstIterator i = fieldList.begin(); i != fieldList.end(); ++i) {
    if (*i == field) {
      break;
    }
    ++col;
  }

  if (col + 1 > fieldList.count()) {
    if (_fieldListComplete) {
      return 0;
    }
    bool ok = false;
    col = field.toInt(&ok);
    if (!ok) {
      return 0;
    }
  }

  int bufstart = _rowIndex[s];
  int bufread = _rowIndex[s + n] - bufstart;

  if (bufread <= 0) {
    return 0;
  }

  QFile file(_filename);
  if (!file.open(QIODevice::ReadOnly)) {
    _valid = false;
    return 0;
  }

  if (_tmpBufSize < unsigned(bufread)) {
    _tmpBuf = static_cast<char*>(realloc(_tmpBuf, _tmpBufSize = bufread));
    if (!_tmpBuf) {
      return -1;
    }
  }

  file.seek(bufstart);
  file.read(_tmpBuf, bufread);

  if (_config->_columnType == AsciiSourceConfig::Fixed) {
    for (int i = 0; i < n; ++i, ++s) {
      // Read appropriate column and convert to double
      v[i] = atof(_tmpBuf + _rowIndex[i] - _rowIndex[0] + _config->_columnWidth * (col - 1), sep);
    }
  } else if (_config->_columnType == AsciiSourceConfig::Custom) {
    for (int i = 0; i < n; ++i, ++s) {
      bool incol = false;
      int i_col = 0;
      v[i] = Kst::NOPOINT;
      for (int ch = _rowIndex[s] - bufstart; ch < bufread; ++ch) {
        if (_config->_columnDelimiter.value().contains(_tmpBuf[ch])) {
          incol = false;
        } else if (_tmpBuf[ch] == '\n' || _tmpBuf[ch] == '\r') {
          break;
        } else if (_config->_delimiters.value().contains(_tmpBuf[ch])) {
          break;
        } else {
          if (!incol) {
            incol = true;
            ++i_col;
            if (i_col == col) {
              if (isdigit(_tmpBuf[ch]) || _tmpBuf[ch] == '-' || _tmpBuf[ch] == '.' || _tmpBuf[ch] == '+') {
                v[i] = atof(_tmpBuf + ch, sep);
              } else if (ch + 2 < bufread && tolower(_tmpBuf[ch]) == 'i' &&
                  tolower(_tmpBuf[ch + 1]) == 'n' && tolower(_tmpBuf[ch + 2]) == 'f') {
                v[i] = INF;
              }
              break;
            }
          }
        }
      }
    }
  } else {
    for (int i = 0; i < n; i++, s++) {
      bool incol = false;
      int i_col = 0;

      v[i] = Kst::NOPOINT;
      for (int ch = _rowIndex[s] - bufstart; ch < bufread; ++ch) {
        if (isspace(_tmpBuf[ch])) {
          if (_tmpBuf[ch] == '\n' || _tmpBuf[ch] == '\r') {
            break;
          } else {
            incol = false;
          }
        } else if (_config->_delimiters.value().contains(_tmpBuf[ch])) {
          break;
        } else {
          if (!incol) {
            incol = true;
            ++i_col;
            if (i_col == col) {
              if (isdigit(_tmpBuf[ch]) || _tmpBuf[ch] == '-' || _tmpBuf[ch] == '.' || _tmpBuf[ch] == '+') {
                v[i] = atof(_tmpBuf + ch, sep);
              } else if (ch + 2 < bufread && tolower(_tmpBuf[ch]) == 'i' &&
                  tolower(_tmpBuf[ch + 1]) == 'n' && tolower(_tmpBuf[ch + 2]) == 'f') {
                v[i] = INF;
              }
              break;
            }
          }
        }
      }
    }
  }

  file.close();

  return n;
}



QString AsciiSource::fileType() const {
  return asciiTypeString;
}


bool AsciiSource::isEmpty() const {
  return _numFrames < 1;
}

QStringList AsciiSource::scalarListFor(const QString& filename, AsciiSourceConfig *cfg) {
  Q_UNUSED(cfg)
  QStringList rc;
  QFile file(filename);

  if (!file.open(QIODevice::ReadOnly)) {
    return rc;
  }

  file.close();

  rc += "FRAMES";
  return rc;

}

QStringList AsciiSource::stringListFor(const QString& filename, AsciiSourceConfig *cfg) {
  Q_UNUSED(cfg)
  QStringList rc;
  QFile file(filename);

  if (!file.open(QIODevice::ReadOnly)) {
    return rc;
  }

  file.close();

  rc += "FILE";
  return rc;

}

QStringList AsciiSource::fieldListFor(const QString& filename, AsciiSourceConfig *cfg) {
  QStringList rc;
  QFile file(filename);

  if (!file.open(QIODevice::ReadOnly)) {
    return rc;
  }

  rc += "INDEX";

  if (cfg->_readFields) {
    int l = cfg->_fieldsLine;
    QByteArray line;
    while (!file.atEnd()) {
      int r = readFullLine(file, line);
      if (l-- == 0) {
        if (r >= 0) {
          if (cfg->_columnType == AsciiSourceConfig::Custom && !cfg->_columnDelimiter.value().isEmpty()) {
            rc += QString(line).trimmed().split(QRegExp(QString("[%1]").arg(QRegExp::escape(cfg->_columnDelimiter))), QString::SkipEmptyParts);
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

  QRegExp re;
  if (cfg->_columnType == AsciiSourceConfig::Custom && !cfg->_columnDelimiter.value().isEmpty()) {
    re.setPattern(QString("^[%1]*[%2].*").arg(QRegExp::escape(cfg->_columnDelimiter)).arg(cfg->_delimiters));
  } else {
    re.setPattern(QString("^\\s*[%1].*").arg(cfg->_delimiters));
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
  QByteArray line;
  while (!file.atEnd() && !done && (nextscan < 200)) {
    int r = readFullLine(file, line);
    if (skip > 0) { //keep skipping until desired line
      --skip;
      if (r < 0) {
        return rc;
      }
      continue;
    }
    if (maxcnt >= 0) { //original skip value == 0, so scan some lines
      if (curscan >= nextscan) {
        if (r > 1 && !re.exactMatch(line)) {
          line = line.trimmed();
          if (cfg->_columnType == AsciiSourceConfig::Custom && !cfg->_columnDelimiter.value().isEmpty()) {
            cnt = QString(line).split(QRegExp(QString("[%1]").arg(QRegExp::escape(cfg->_columnDelimiter))), QString::SkipEmptyParts).count();
          } else if (cfg->_columnType == AsciiSourceConfig::Fixed) {
            cnt = line.length() / cfg->_columnWidth;
          } else {
            cnt = QString(line).split(QRegExp("\\s"), QString::SkipEmptyParts).count();
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
    if (r > 1 && !re.exactMatch(line)) { //at desired line, find count
      line = line.trimmed();
      if (cfg->_columnType == AsciiSourceConfig::Custom && !cfg->_columnDelimiter.value().isEmpty()) {
        maxcnt = QString(line).split(QRegExp(QString("[%1]").arg(QRegExp::escape(cfg->_columnDelimiter))), QString::SkipEmptyParts).count();
      } else if (cfg->_columnType == AsciiSourceConfig::Fixed) {
        maxcnt = line.length() / cfg->_columnWidth;
      } else {
        maxcnt = QString(line).split(QRegExp("\\s"), QString::SkipEmptyParts).count();
      }
      done = true;
    } else if (r < 0) {
      return rc;
    }
  }

  file.close();
  for (int i = 1; i <= maxcnt; ++i) {
    rc += i18n("Column %1").arg(i);
  }

  return rc;
}

void AsciiSource::save(QXmlStreamWriter &s) {
  Kst::DataSource::save(s);
  _config->save(s);
}


void AsciiSource::parseProperties(QXmlStreamAttributes &properties) {
  _config->parseProperties(properties);
  reset();
}


bool AsciiSource::supportsTimeConversions() const {
  return false; //fieldList().contains(_config->_indexVector) && _config->_indexInterpretation != AsciiSourceConfig::Unknown && _config->_indexInterpretation != AsciiSourceConfig::INDEX;
}


int AsciiSource::sampleForTime(double ms, bool *ok) {
  switch (_config->_indexInterpretation) {
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


const QString& AsciiSource::typeString() const {
  return asciiTypeString;
}


int AsciiSource::sampleForTime(const QDateTime& time, bool *ok) {
  switch (_config->_indexInterpretation) {
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
