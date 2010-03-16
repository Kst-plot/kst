/***************************************************************************
                     asciispource_p.cpp
                             -------------------
    begin                : Mar 16 2010
    copyright            : (C) 2010 The University of Toronto
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


#include "asciisource_p.h"


//
// AsciiSource::Config
//


#define DEFAULT_DELIMITERS "#/c!;"
#define DEFAULT_COLUMN_WIDTH 16


const char AsciiSource::Config::Key_fileNamePattern[] = "Filename Pattern";
const char AsciiSource::Config::Tag_fileNamePattern[] = "filenamepattern";
const char AsciiSource::Config::Key_delimiters[] = "Comment Delimiters";
const char AsciiSource::Config::Tag_delimiters[] = "delimiters";
const char AsciiSource::Config::Key_indexVector[] ="Index";
const char AsciiSource::Config::Tag_indexVector[] ="vector";
const char AsciiSource::Config::Key_indexInterpretation[] = "Default INDEX Interpretation";
const char AsciiSource::Config::Tag_indexInterpretation[] = "interpretation";
const char AsciiSource::Config::Key_columnType[] = "Column Type";
const char AsciiSource::Config::Tag_columnType[] = "columntype";
const char AsciiSource::Config::Key_columnDelimiter[] = "Column Delimiter";
const char AsciiSource::Config::Tag_columnDelimiter[] = "columndelimiter";
const char AsciiSource::Config::Key_columnWidth[] = "Column Width";
const char AsciiSource::Config::Tag_columnWidth[] = "columnwidth";
const char AsciiSource::Config::Key_dataLine[] = "Data Start";
const char AsciiSource::Config::Tag_dataLine[] = "headerstart";
const char AsciiSource::Config::Key_readFields[] = "Read Fields";
const char AsciiSource::Config::Tag_readFields[] = "readfields";
const char AsciiSource::Config::Key_useDot[] = "Use Dot";
const char AsciiSource::Config::Tag_useDot[] = "usedot";
const char AsciiSource::Config::Key_fieldsLine[] = "Fields Line";
const char AsciiSource::Config::Tag_fieldsLine[] = "fields";


AsciiSource::Config::Config() :
  _delimiters(DEFAULT_DELIMITERS),
  _indexVector("INDEX"),
  _fileNamePattern(""),
  _indexInterpretation(Unknown),
  _columnType(Whitespace),
  _columnDelimiter(""),
  _columnWidth(DEFAULT_COLUMN_WIDTH),
  _dataLine(0),
  _readFields(false),
  _fieldsLine(0),
  _useDot(true),
  _localSeparator(QLocale().decimalPoint().toAscii())
{
}



void AsciiSource::Config::save(QSettings& cfg) {
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
}


void AsciiSource::Config::saveGroup(QSettings& cfg, const QString& fileName) {
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


void AsciiSource::Config::read(QSettings& cfg) {
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
}


void AsciiSource::Config::readGroup(QSettings& cfg, const QString& fileName) {
  cfg.beginGroup(AsciiSource::asciiTypeKey());
  if (fileName.isEmpty()) {
    read(cfg);
  } else {
    cfg.beginGroup(fileName);
    read(cfg);
    cfg.endGroup();
  }
  _delimiters = QRegExp::escape(_delimiters).toLatin1();
  cfg.endGroup();
}


void AsciiSource::Config::save(QXmlStreamWriter& s) {
  s.writeStartElement("properties");
  if (_indexInterpretation != AsciiSource::Config::Unknown) {
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

  s.writeEndElement();
}


void AsciiSource::Config::parseProperties(QXmlStreamAttributes& attributes) {
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
}


void AsciiSource::Config::load(const QDomElement& e) {
  // TODO use tags
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


