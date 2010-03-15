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


#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qtextdocument.h>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QLocale>

#include <math_kst.h>
#include <kst_inf.h>
#include "ascii.h"
#include "ui_asciiconfig.h"
#include "kst_i18n.h"
#include "measuretime.h"

#ifdef Q_OS_WIN
#define USE_KST_ATOF
#endif

#ifdef USE_KST_ATOF
#include "kst_atof.h"
#define atof(X, Y) kst_atof(X, Y)
#else
#define atof(X, Y) atof(X)
#endif

#define DEFAULT_DELIMITERS "#/c!;"
#define DEFAULT_COLUMN_WIDTH 16

static const QString asciiTypeString = I18N_NOOP("ASCII file");

//const QString AsciiSource::staticTypeString = I18N_NOOP("Ascii Source");
//const QString AsciiSource::staticTypeTag = I18N_NOOP("source");

class AsciiSource::Config {
  public:
    Config();

    void read(QSettings *cfg, const QString& fileName = QString::null);
    void save(QXmlStreamWriter& s);
    void parseProperties(QXmlStreamAttributes &properties);
    void load(const QDomElement& e);

    QString _delimiters;
    QString _indexVector;
    QString _fileNamePattern;

    enum Interpretation { Unknown = 0, INDEX, CTime, Seconds, IntEnd = 0xffff };
    Interpretation _indexInterpretation;

    enum ColumnType { Whitespace = 0, Fixed, Custom, ColEnd = 0xffff };
    ColumnType _columnType;

    QString _columnDelimiter;
    int _columnWidth;
    int _dataLine;
    bool _readFields;
    int _fieldsLine;
    bool _useDot;
    char _localSeparator;
  };


AsciiSource::Config::Config() {
      _indexInterpretation = Unknown;
      _indexVector = "INDEX";
      _delimiters = DEFAULT_DELIMITERS;
      _columnType = Whitespace;
      _columnWidth = DEFAULT_COLUMN_WIDTH;
      _dataLine = 0;
      _readFields = false;
      _fieldsLine = 0;
      _useDot = true;
      _localSeparator = QLocale().decimalPoint().toAscii();
    }

    void AsciiSource::Config::read(QSettings *cfg, const QString& fileName) {
      cfg->beginGroup(asciiTypeString);
      _fileNamePattern = cfg->value("Filename Pattern").toString();
      _delimiters = cfg->value("Comment Delimiters", "#/c!;").toString().toLatin1();
      _indexInterpretation = (Interpretation)cfg->value("Default INDEX Interpretation", (int)Unknown).toInt();
      _columnType = (ColumnType)cfg->value("Column Type", (int)Whitespace).toInt();
      _columnDelimiter = cfg->value("Column Delimiter", QString()).toString().toLatin1();
      _columnWidth = cfg->value("Column Width", DEFAULT_COLUMN_WIDTH).toInt();
      _dataLine = cfg->value("Data Start", 0).toInt();
      _readFields = cfg->value("Read Fields", false).toBool();
      _useDot = cfg->value("Use Dot", true).toBool();
      _fieldsLine = cfg->value("Fields Line", 0).toInt();
      if (!fileName.isEmpty()) {
        cfg->beginGroup(fileName);
        _delimiters = cfg->value("Comment Delimiters", _delimiters).toString().toLatin1();
        _indexInterpretation = (Interpretation)cfg->value("Default INDEX Interpretation", (int)_indexInterpretation).toInt();
        _columnType = (ColumnType)cfg->value("Column Type", (int)_columnType).toInt();
        _columnDelimiter = cfg->value("Column Delimiter", _columnDelimiter).toString().toLatin1();
        _columnWidth = cfg->value("Column Width", _columnWidth).toInt();
        _dataLine = cfg->value("Data Start", _dataLine).toInt();
        _readFields = cfg->value("Read Fields", _readFields).toBool();
        _useDot = cfg->value("Use Dot", _useDot).toBool();
        _fieldsLine = cfg->value("Fields Line", _fieldsLine).toInt();
        cfg->endGroup();
      }
      _delimiters = QRegExp::escape(_delimiters).toLatin1();
      cfg->endGroup();
    }


    void AsciiSource::Config::save(QXmlStreamWriter& s) {
      s.writeStartElement("properties");
      if (_indexInterpretation != AsciiSource::Config::Unknown) {
        s.writeAttribute("vector", _indexVector);
        s.writeAttribute("interpretation", QString::number(int(_indexInterpretation)));
      }
      s.writeAttribute("delimiters", _delimiters);

      s.writeAttribute("columntype", QString::number(int(_columnType)));
      if (_columnType == Fixed) {
        s.writeAttribute("columnwidth", QString::number(_columnWidth));
      } else if (_columnType == Custom) {
        s.writeAttribute("columndelimiters", _columnDelimiter);
      }

      s.writeAttribute("headerstart", QString::number(_dataLine));
      s.writeAttribute("fields", QString::number(_fieldsLine));
      s.writeAttribute("readfields", QVariant(_readFields).toString());
      s.writeAttribute("usedot", QVariant(_useDot).toString());
      s.writeEndElement();
    }

    void AsciiSource::Config::parseProperties(QXmlStreamAttributes &properties) {
      _indexVector = properties.value("vector").toString();
      _indexInterpretation = (Interpretation)properties.value("interpretation").toString().toInt();

      _delimiters = properties.value("delimiters").toString();
      _columnType = (ColumnType)properties.value("columntype").toString().toInt();
      _columnDelimiter = properties.value("columndelimiters").toString().toInt();

      _dataLine = properties.value("headerstart").toString().toInt();
      _fieldsLine = properties.value("fields").toString().toInt();
      _readFields = QVariant(properties.value("readfields").toString()).toBool();
      _useDot = QVariant(properties.value("usedot").toString()).toBool();
    }

    void AsciiSource::Config::load(const QDomElement& e) {
       QDomNode n = e.firstChild();
       while (!n.isNull()) {
         QDomElement e = n.toElement();
         if (!e.isNull()) {
           if (e.tagName() == "index") {
             if (e.hasAttribute("vector")) {
               _indexVector = e.attribute("vector");
             }
             if (e.hasAttribute("interpretation")) {
               _indexInterpretation = Interpretation(e.attribute("interpretation").toInt());
             }
           } else if (e.tagName() == "comment") {
             if (e.hasAttribute("delimiters")) {
               _delimiters = e.attribute("delimiters").toLatin1();
             }
           } else if (e.tagName() == "columns") {
             if (e.hasAttribute("type")) {
               _columnType = ColumnType(e.attribute("type").toInt());
             }
             if (e.hasAttribute("width")) {
               _columnWidth = e.attribute("width").toInt();
             }
             if (e.hasAttribute("delimiters")) {
               _columnDelimiter = e.attribute("delimiters").toLatin1();
             }
           } else if (e.tagName() == "header") {
             if (e.hasAttribute("start")) {
               _dataLine = e.attribute("start").toInt();
             }
             if (e.hasAttribute("fields")) {
               _fieldsLine = e.attribute("fields").toInt();
             }
           }
         }
         n = n.nextSibling();
       }
    }



AsciiSource::AsciiSource(Kst::ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const QDomElement& e)
: Kst::DataSource(store, cfg, filename, type, File), _rowIndex(0L), _config(0L), _tmpBuf(0L), _tmpBufSize(0) {

  //TIME_IN_SCOPE(Ctor_AsciiSource);

  _valid = false;
  _haveHeader = false;
  _fieldListComplete = false;
  _source = asciiTypeString;
  if (!type.isEmpty() && type != asciiTypeString) {
    return;
  }
  _config = new AsciiSource::Config;
  _config->read(cfg, filename);
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
  _matrixList.clear();
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
  QByteArray delbytes = _config->_delimiters.toLatin1();
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


bool AsciiSource::fieldListIsComplete() const {
  return _fieldListComplete;
}


int AsciiSource::readScalar(double &S, const QString& scalar) {
  if (scalar == "FRAMES") {
    S = _numFrames;
    return 1;
  }
  return 0;
}


int AsciiSource::readString(QString &S, const QString& string) {
  if (string == "FILE") {
    S = _filename;
    return 1;
  }
  return 0;
}


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

  QStringList fieldList = this->fieldList();
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

  if (_config->_columnType == AsciiSource::Config::Fixed) {
    for (int i = 0; i < n; ++i, ++s) {
      // Read appropriate column and convert to double
      v[i] = atof(_tmpBuf + _rowIndex[i] - _rowIndex[0] + _config->_columnWidth * (col - 1), sep);
    }
  } else if (_config->_columnType == AsciiSource::Config::Custom) {
    for (int i = 0; i < n; ++i, ++s) {
      bool incol = false;
      int i_col = 0;
      v[i] = Kst::NOPOINT;
      for (int ch = _rowIndex[s] - bufstart; ch < bufread; ++ch) {
        if (_config->_columnDelimiter.contains(_tmpBuf[ch])) {
          incol = false;
        } else if (_tmpBuf[ch] == '\n' || _tmpBuf[ch] == '\r') {
          break;
        } else if (_config->_delimiters.contains(_tmpBuf[ch])) {
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
        } else if (_config->_delimiters.contains(_tmpBuf[ch])) {
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

int AsciiSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 1;
}


int AsciiSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return _numFrames;
}


QString AsciiSource::fileType() const {
  return asciiTypeString;
}


bool AsciiSource::isEmpty() const {
  return _numFrames < 1;
}

QStringList AsciiSource::scalarListFor(const QString& filename, AsciiSource::Config *cfg) {
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

QStringList AsciiSource::stringListFor(const QString& filename, AsciiSource::Config *cfg) {
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

QStringList AsciiSource::fieldListFor(const QString& filename, AsciiSource::Config *cfg) {
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
          if (cfg->_columnType == AsciiSource::Config::Custom && !cfg->_columnDelimiter.isEmpty()) {
            rc += QString(line).trimmed().split(QRegExp(QString("[%1]").arg(QRegExp::escape(cfg->_columnDelimiter))), QString::SkipEmptyParts);
          } else if (cfg->_columnType == AsciiSource::Config::Fixed) {
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
  if (cfg->_columnType == AsciiSource::Config::Custom && !cfg->_columnDelimiter.isEmpty()) {
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
          if (cfg->_columnType == AsciiSource::Config::Custom && !cfg->_columnDelimiter.isEmpty()) {
            cnt = QString(line).split(QRegExp(QString("[%1]").arg(QRegExp::escape(cfg->_columnDelimiter))), QString::SkipEmptyParts).count();
          } else if (cfg->_columnType == AsciiSource::Config::Fixed) {
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
      if (cfg->_columnType == AsciiSource::Config::Custom && !cfg->_columnDelimiter.isEmpty()) {
        maxcnt = QString(line).split(QRegExp(QString("[%1]").arg(QRegExp::escape(cfg->_columnDelimiter))), QString::SkipEmptyParts).count();
      } else if (cfg->_columnType == AsciiSource::Config::Fixed) {
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
  return false; //fieldList().contains(_config->_indexVector) && _config->_indexInterpretation != AsciiSource::Config::Unknown && _config->_indexInterpretation != AsciiSource::Config::INDEX;
}


int AsciiSource::sampleForTime(double ms, bool *ok) {
  switch (_config->_indexInterpretation) {
    case AsciiSource::Config::Seconds:
      // FIXME: make sure "seconds" exists in _indexVector
      if (ok) {
        *ok = true;
      }
      return 0;
    case AsciiSource::Config::CTime:
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
    case AsciiSource::Config::Seconds:
      // FIXME: make sure "time" exists in _indexVector
      if (ok) {
        *ok = true;
      }
      return time.toTime_t();
    case AsciiSource::Config::CTime:
      // FIXME: make sure "time" exists in _indexVector (different than above?)
      if (ok) {
        *ok = true;
      }
      return time.toTime_t();
    default:
      return Kst::DataSource::sampleForTime(time, ok);
  }
}


class ConfigWidgetAsciiInternal : public QWidget, public Ui_AsciiConfig {
  public:
    ConfigWidgetAsciiInternal(QWidget *parent) : QWidget(parent), Ui_AsciiConfig() { setupUi(this); }
};


class ConfigWidgetAscii : public Kst::DataSourceConfigWidget {
  public:
    ConfigWidgetAscii() : Kst::DataSourceConfigWidget() {
      QGridLayout *layout = new QGridLayout(this);
      _ac = new ConfigWidgetAsciiInternal(this);
      layout->addWidget(_ac, 0, 0);
      layout->activate();
    }

    ~ConfigWidgetAscii() {}

    void setConfig(QSettings *cfg) {
      Kst::DataSourceConfigWidget::setConfig(cfg);
    }

    void load() {
      _cfg->beginGroup(asciiTypeString);
      _ac->_delimiters->setText(_cfg->value("Comment Delimiters", DEFAULT_DELIMITERS).toString());
      _ac->_fileNamePattern->setText(_cfg->value("Filename Pattern").toString());
      _ac->_columnDelimiter->setText(_cfg->value("Column Delimiter").toString());
      _ac->_columnWidth->setValue(_cfg->value("Column Width", DEFAULT_COLUMN_WIDTH).toInt());
      _ac->_startLine->setValue(_cfg->value("Data Start", 0).toInt());
      _ac->_readFields->setChecked(_cfg->value("Read Fields", false).toBool());
      _ac->_useDot->setChecked(_cfg->value("Use Dot", true).toBool());
      _ac->_fieldsLine->setValue(_cfg->value("Fields Line", 0).toInt());
      AsciiSource::Config::ColumnType ct = (AsciiSource::Config::ColumnType)_cfg->value("Column Type", 0).toInt();
      if (ct == AsciiSource::Config::Fixed) {
        _ac->_fixed->setChecked(true);
      } else if (ct == AsciiSource::Config::Custom) {
        _ac->_custom->setChecked(true);
      } else {
        _ac->_whitespace->setChecked(true);
      }
      bool hasInstance = (_instance != 0L);
      _ac->_indexVector->clear();
      if (hasInstance) {
        _ac->_indexVector->addItems(_instance->fieldList());
        Kst::SharedPtr<AsciiSource> src = Kst::kst_cast<AsciiSource>(_instance);
        assert(src);
        _ac->_indexType->setCurrentIndex(src->_config->_indexInterpretation - 1);
        if (_instance->fieldList().contains(src->_config->_indexVector)) {
          _ac->_indexVector->setEditText(src->_config->_indexVector);
        }

        _cfg->beginGroup(src->fileName());

        _ac->_delimiters->setText(_cfg->value("Comment Delimiters", _ac->_delimiters->text()).toString());
        _ac->_columnDelimiter->setText(_cfg->value("Column Delimiter", _ac->_columnDelimiter->text()).toString());
        _ac->_columnWidth->setValue(_cfg->value("Column Width", _ac->_columnWidth->value()).toInt());
        _ac->_startLine->setValue(_cfg->value("Data Start", _ac->_startLine->value()).toInt());
        _ac->_readFields->setChecked(_cfg->value("Read Fields", _ac->_readFields->isChecked()).toBool());
        _ac->_useDot->setChecked(_cfg->value("Use Dot", _ac->_useDot->isChecked()).toBool());
        _ac->_fieldsLine->setValue(_cfg->value("Fields Line", _ac->_fieldsLine->value()).toInt());
        ct = (AsciiSource::Config::ColumnType)_cfg->value("Column Type", (int)ct).toInt();
        if (ct == AsciiSource::Config::Fixed) {
          _ac->_fixed->setChecked(true);
        } else if (ct == AsciiSource::Config::Custom) {
          _ac->_custom->setChecked(true);
        } else {
          _ac->_whitespace->setChecked(true);
        }
        _cfg->endGroup();
      } else {
        _ac->_indexVector->addItem("INDEX");
        int x = _cfg->value("Default INDEX Interpretation", (int)AsciiSource::Config::INDEX).toInt();
        if (x > 0 && x <= _ac->_indexType->count()) {
          _ac->_indexType->setCurrentIndex(x - 1);
        } else {
          _ac->_indexType->setCurrentIndex(0);
        }
      }
      _ac->_indexVector->setEnabled(hasInstance);
      _cfg->endGroup();
    }

    void save() {
      assert(_cfg);
      _cfg->beginGroup(asciiTypeString);
      if (_ac->_applyDefault->isChecked()) {
        _cfg->setValue("Filename Pattern", _ac->_fileNamePattern->text());

        _cfg->setValue("Default INDEX Interpretation", 1 + _ac->_indexType->currentIndex());
        _cfg->setValue("Comment Delimiters", _ac->_delimiters->text());
        AsciiSource::Config::ColumnType ct = AsciiSource::Config::Whitespace;
        if (_ac->_fixed->isChecked()) {
          ct = AsciiSource::Config::Fixed;
        } else if (_ac->_custom->isChecked()) {
          ct = AsciiSource::Config::Custom;
        }
        _cfg->setValue("Column Type", (int)ct);
        _cfg->setValue("Column Delimiter", _ac->_columnDelimiter->text());
        _cfg->setValue("Column Width", _ac->_columnWidth->value());
        _cfg->setValue("Data Start", _ac->_startLine->value());
        _cfg->setValue("Read Fields", _ac->_readFields->isChecked());
        _cfg->setValue("Use Dot", _ac->_useDot->isChecked());
        _cfg->setValue("Fields Line", _ac->_fieldsLine->value());
      }

      // If we have an instance, save settings for that instance as well
      Kst::SharedPtr<AsciiSource> src = Kst::kst_cast<AsciiSource>(_instance);
      if (src) {
        _cfg->beginGroup(src->fileName());
        _cfg->setValue("Default INDEX Interpretation", 1 + _ac->_indexType->currentIndex());
        _cfg->setValue("Comment Delimiters", _ac->_delimiters->text());
        AsciiSource::Config::ColumnType ct;
        if (_ac->_fixed->isChecked())
          ct = AsciiSource::Config::Fixed;
        else if (_ac->_custom->isChecked())
          ct = AsciiSource::Config::Custom;
        else
          ct = AsciiSource::Config::Whitespace;
        _cfg->setValue("Column Type", (int)ct);
        _cfg->setValue("Column Delimiter", _ac->_columnDelimiter->text());
        _cfg->setValue("Column Width", _ac->_columnWidth->value());
        _cfg->setValue("Data Start", _ac->_startLine->value());
        _cfg->setValue("Read Fields", _ac->_readFields->isChecked());
        _cfg->setValue("Use Dot", _ac->_useDot->isChecked());
        _cfg->setValue("Fields Line", _ac->_fieldsLine->value());
        _cfg->endGroup();
      }
      _cfg->endGroup();

      // Update the instance from our new settings
      if (src && src->reusable()) {
        src->_config->read(_cfg, src->fileName());
        src->reset();
      }
    }

    ConfigWidgetAsciiInternal *_ac;
};


QString AsciiPlugin::pluginName() const { return "ASCII File Reader"; }
QString AsciiPlugin::pluginDescription() const { return "ASCII File Reader"; }


Kst::DataSource *AsciiPlugin::create(Kst::ObjectStore *store, QSettings *cfg,
                                            const QString &filename,
                                            const QString &type,
                                            const QDomElement &element) const {

  return new AsciiSource(store, cfg, filename, type, element);
}

QStringList AsciiPlugin::matrixList(QSettings *cfg,
                                             const QString& filename,
                                             const QString& type,
                                             QString *typeSuggestion,
                                             bool *complete) const {


  if (typeSuggestion) {
    *typeSuggestion = asciiTypeString;
  }
  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }
  return QStringList();
}

QStringList AsciiPlugin::fieldList(QSettings *cfg,
                                            const QString& filename,
                                            const QString& type,
                                            QString *typeSuggestion,
                                            bool *complete) const {

  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = asciiTypeString;
  }

  AsciiSource::Config config;
  config.read(cfg, filename);
  QStringList rc = AsciiSource::fieldListFor(filename, &config);

  if (complete) {
    *complete = rc.count() > 1;
  }

  return rc;

}

QStringList AsciiPlugin::scalarList(QSettings *cfg,
                                    const QString& filename,
                                    const QString& type,
                                    QString *typeSuggestion,
                                    bool *complete) const {

  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = asciiTypeString;
  }

  AsciiSource::Config config;
  config.read(cfg, filename);
  QStringList rc = AsciiSource::scalarListFor(filename, &config);

  if (complete) {
    *complete = rc.count() > 1;
  }

  return rc;

}

QStringList AsciiPlugin::stringList(QSettings *cfg,
                                    const QString& filename,
                                    const QString& type,
                                    QString *typeSuggestion,
                                    bool *complete) const {

  if ((!type.isEmpty() && !provides().contains(type)) ||
      0 == understands(cfg, filename)) {
    if (complete) {
      *complete = false;
    }
    return QStringList();
  }

  if (typeSuggestion) {
    *typeSuggestion = asciiTypeString;
  }

  AsciiSource::Config config;
  config.read(cfg, filename);
  QStringList rc = AsciiSource::stringListFor(filename, &config);

  if (complete) {
    *complete = rc.count() > 1;
  }

  return rc;

}

int AsciiPlugin::understands(QSettings *cfg, const QString& filename) const {
  AsciiSource::Config config;
  config.read(cfg, filename);

  if (!QFile::exists(filename) || QFileInfo(filename).isDir()) {
    return 0;
  }

  if (!config._fileNamePattern.isEmpty()) {
    QRegExp filenamePattern(config._fileNamePattern);
    filenamePattern.setPatternSyntax(QRegExp::Wildcard);
    if (filenamePattern.exactMatch(filename)) {
      return 100;
    }
  }

  QFile f(filename);
  if (f.open(QIODevice::ReadOnly)) {
    QByteArray s;
    qint64 rc = 0;
    bool done = false;

    QRegExp commentRE, dataRE;
    if (config._columnType == AsciiSource::Config::Custom && !config._columnDelimiter.isEmpty()) {
      commentRE.setPattern(QString("^[%1]*[%2].*").arg(QRegExp::escape(config._columnDelimiter)).arg(config._delimiters));
      dataRE.setPattern(QString("^[%1]*(([Nn][Aa][Nn]|(\\-\\+)?[Ii][Nn][Ff]|[0-9\\+\\-\\.eE]+)[\\s]*)+").arg(QRegExp::escape(config._columnDelimiter)));
    } else {
      commentRE.setPattern(QString("^\\s*[%1].*").arg(config._delimiters));
      dataRE.setPattern(QString("^[\\s]*(([Nn][Aa][Nn]|(\\-\\+)?[Ii][Nn][Ff]|[0-9\\+\\-\\.eE]+)[\\s]*)+"));
    }

    int skip = config._dataLine;

    while (!done) {
      rc = AsciiSource::readFullLine(f, s);
      if (skip > 0) {
        --skip;
        if (rc <= 0) {
          done = true;
        }
        continue;
      }
      if (rc <= 0) {
        done = true;
      } else if (rc == 1) {
        // empty line; do nothing
      } else if (commentRE.exactMatch(s)) {
        // comment; do nothing
      } else if (dataRE.exactMatch(s)) {
        // a number - this may be an ascii file - assume that it is
        // This line checks for an indirect file and gives that a chance too.
        // Indirect files look like ascii files.
        return 75;
        //return QFile::exists(s.trimmed()) ? 49 : 75;
      } else {
        return 20;
      }
    }
  } else {
    return 0;
  }

  return 1; // still might be ascii - ex: header with no data yet.
}


bool AsciiPlugin::supportsTime(QSettings *cfg, const QString& filename) const {
  //FIXME
  Q_UNUSED(cfg)
  Q_UNUSED(filename)
  return true;
}


QStringList AsciiPlugin::provides() const {
  QStringList rc;
  rc += asciiTypeString;
  return rc;
}


Kst::DataSourceConfigWidget *AsciiPlugin::configWidget(QSettings *cfg, const QString& filename) const {
  Q_UNUSED(filename)
  ConfigWidgetAscii *config = new ConfigWidgetAscii;
  config->setConfig(cfg);
  config->load();
  return config;
}

Q_EXPORT_PLUGIN2(kstdata_ascii, AsciiPlugin)

// vim: ts=2 sw=2 et
