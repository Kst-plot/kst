/***************************************************************************
                     asciispource_p.cpp
                             -------------------
    begin                : Mar 16 2010
    copyright            : (C) 2010 The University of Toronto
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


#include "asciisourceconfig.h"

#include "asciisource.h"

#include <QLocale>

//
// AsciiSourceConfig
//


#define DEFAULT_COMMENT_DELIMITERS "#"
#define DEFAULT_COLUMN_WIDTH 16


const char AsciiSourceConfig::Key_fileNamePattern[] = "Filename Pattern";
const char AsciiSourceConfig::Tag_fileNamePattern[] = "filenamepattern";
const char AsciiSourceConfig::Key_delimiters[] = "Comment Delimiters";
const char AsciiSourceConfig::Tag_delimiters[] = "delimiters";
const char AsciiSourceConfig::Key_indexVector[] ="Index";
const char AsciiSourceConfig::Tag_indexVector[] ="vector";
const char AsciiSourceConfig::Key_indexInterpretation[] = "Default INDEX Interpretation";
const char AsciiSourceConfig::Tag_indexInterpretation[] = "interpretation";
const char AsciiSourceConfig::Key_columnType[] = "Column Type";
const char AsciiSourceConfig::Tag_columnType[] = "columntype";
const char AsciiSourceConfig::Key_columnDelimiter[] = "Column Delimiter";
const char AsciiSourceConfig::Tag_columnDelimiter[] = "columndelimiter";
const char AsciiSourceConfig::Key_columnWidth[] = "Column Width";
const char AsciiSourceConfig::Tag_columnWidth[] = "columnwidth";
const char AsciiSourceConfig::Key_columnWidthIsConst[] = "Column Width is const";
const char AsciiSourceConfig::Tag_columnWidthIsConst[] = "columnwidthisconst";
const char AsciiSourceConfig::Key_dataLine[] = "Data Start";
const char AsciiSourceConfig::Tag_dataLine[] = "headerstart";
const char AsciiSourceConfig::Key_readFields[] = "Read Fields";
const char AsciiSourceConfig::Tag_readFields[] = "readfields";
const char AsciiSourceConfig::Key_readUnits[] = "Read Units";
const char AsciiSourceConfig::Tag_readUnits[] = "readunits";
const char AsciiSourceConfig::Key_useDot[] = "Use Dot";
const char AsciiSourceConfig::Tag_useDot[] = "usedot";
const char AsciiSourceConfig::Key_fieldsLine[] = "Fields Line";
const char AsciiSourceConfig::Tag_fieldsLine[] = "fields";
const char AsciiSourceConfig::Key_unitsLine[] = "Units Line";
const char AsciiSourceConfig::Tag_unitsLine[] = "units";
const char AsciiSourceConfig::Key_zeroStart[] = "Line number start at 0";
const char AsciiSourceConfig::Tag_zeroStart[] = "zerostart";

AsciiSourceConfig::AsciiSourceConfig() :
  _delimiters(DEFAULT_COMMENT_DELIMITERS),
  _indexVector("INDEX"),
  _fileNamePattern(""),
  _indexInterpretation(Unknown),
  _columnType(Whitespace),
  _columnDelimiter(","),
  _columnWidth(DEFAULT_COLUMN_WIDTH),
  _columnWidthIsConst(false),
  _dataLine(0),
  _readFields(false),
  _readUnits(false),
  _fieldsLine(0),
  _unitsLine(0),
  _useDot(true),
  _zeroStart(false),
  _localSeparator(QLocale().decimalPoint().toAscii())
{
}



void AsciiSourceConfig::save(QSettings& cfg) {
  _fileNamePattern >> cfg;
  _indexVector >> cfg;
  _delimiters >> cfg;
  _indexInterpretation >> cfg;
  _columnType >> cfg;
  _columnDelimiter >> cfg;
  _columnWidth >> cfg;
  _dataLine >> cfg;
  _readFields >> cfg;
  _useDot >> cfg;
  _fieldsLine >> cfg;
  _columnWidthIsConst >> cfg;
  _readUnits >> cfg;
  _unitsLine >> cfg;
  _zeroStart >> cfg;
}


void AsciiSourceConfig::saveGroup(QSettings& cfg, const QString& fileName) {
  cfg.beginGroup(AsciiSource::asciiTypeKey());
  if (fileName.isEmpty()) {
    save(cfg);
  } else {
    cfg.beginGroup(fileName);
    save(cfg);
    cfg.endGroup();
  }
  cfg.endGroup();
}


void AsciiSourceConfig::read(QSettings& cfg) {
  _fileNamePattern << cfg;
  _indexVector << cfg;
  _delimiters << cfg;
  _indexInterpretation << cfg;
  _columnType << cfg;
  _columnDelimiter << cfg;
  _columnWidth << cfg;
  _dataLine << cfg;
  _readFields << cfg;
  _useDot << cfg;
  _fieldsLine << cfg;
  _columnWidthIsConst << cfg;
  _readUnits << cfg;
  _unitsLine << cfg;
  _zeroStart << cfg;
}


void AsciiSourceConfig::readGroup(QSettings& cfg, const QString& fileName) {
  cfg.beginGroup(AsciiSource::asciiTypeKey());
  read(cfg);
  if (!fileName.isEmpty()) {
    cfg.beginGroup(fileName);
    read(cfg);
    cfg.endGroup();
  }
  _delimiters = QRegExp::escape(_delimiters).toLatin1();
  cfg.endGroup();
}


void AsciiSourceConfig::save(QXmlStreamWriter& s) {
  s.writeStartElement("properties");
  if (_indexInterpretation != AsciiSourceConfig::Unknown) {
    _indexVector >> s;
    _indexInterpretation >> s;
  }
  _delimiters >> s;

  _columnType >> s;
  if (_columnType == Fixed) {
    _columnWidth >> s;
  } else if (_columnType == Custom) {
      _columnDelimiter >> s;
  }

  _dataLine >> s;
  _fieldsLine >> s;
  _readFields >> s;
  _useDot >> s;
  _columnWidthIsConst >> s;
  _readUnits >> s;
  _unitsLine >> s;
  _zeroStart >> s;

  s.writeEndElement();
}


void AsciiSourceConfig::parseProperties(QXmlStreamAttributes& attributes) {
  _fileNamePattern << attributes;
  _indexVector << attributes;
  _delimiters << attributes;
  _indexInterpretation << attributes;
  _columnType << attributes;
  _columnDelimiter << attributes;
  _columnWidth << attributes;
  _dataLine << attributes;
  _readFields << attributes;
  _useDot << attributes;
  _fieldsLine << attributes;
  _columnWidthIsConst << attributes;
  _readUnits << attributes;
  _unitsLine << attributes;
  _zeroStart << attributes;
}


void AsciiSourceConfig::load(const QDomElement& e) {
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement elem = n.toElement();
    if (!elem.isNull()) {
      if (elem.tagName() == "properties") {
        _fileNamePattern << elem;
        _indexVector << elem;
        _delimiters << elem;
        _indexInterpretation << elem;
        _columnType << elem;
        _columnDelimiter << elem;
        _columnWidth << elem;
        _dataLine << elem;
        _readFields << elem;
        _useDot << elem;
        _fieldsLine << elem;
        _columnWidthIsConst << elem;
        _readUnits << elem;
        _unitsLine << elem;
        _zeroStart << elem;
      }
    }
    n = n.nextSibling();
  }
}

