/***************************************************************************
                     asciisource_p.h
                             -------------------
    begin                :  Mar 16 2010
    copyright            : (C) 2010 The University of Toronto
    email                :  netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef KST_ASCII_SOURCE_CONFIG_H
#define KST_ASCII_SOURCE_CONFIG_H

#include "namedparameter.h"

#include <QDomElement>


class AsciiSourceConfig {

    // TODO translate keys?
    static const char Key_fileNamePattern[];
    static const char Tag_fileNamePattern[];
    static const char Key_delimiters[];
    static const char Tag_delimiters[];
    static const char Key_indexVector[];
    static const char Tag_indexVector[];
    static const char Key_indexInterpretation[];
    static const char Tag_indexInterpretation[];
    static const char Key_columnType[];
    static const char Tag_columnType[];
    static const char Key_columnDelimiter[];
    static const char Tag_columnDelimiter[];
    static const char Key_columnWidth[];
    static const char Tag_columnWidth[];
    static const char Key_columnWidthIsConst[];
    static const char Tag_columnWidthIsConst[];
    static const char Key_dataLine[];
    static const char Tag_dataLine[];
    static const char Key_readFields[];
    static const char Tag_readFields[];
    static const char Key_readUnits[];
    static const char Tag_readUnits[];
    static const char Key_useDot[];
    static const char Tag_useDot[];
    static const char Key_fieldsLine[];
    static const char Tag_fieldsLine[];
    static const char Key_unitsLine[];
    static const char Tag_unitsLine[];

  public:
    AsciiSourceConfig();

    void saveGroup(QSettings& cfg, const QString& fileName = QString());
    void readGroup(QSettings& cfg, const QString& fileName = QString());

    void save(QXmlStreamWriter& s);
    void parseProperties(QXmlStreamAttributes &properties);

    void load(const QDomElement& e);

    enum Interpretation { Unknown = 0, INDEX, CTime, Seconds, IntEnd = 0xffff };
    enum ColumnType { Whitespace = 0, Fixed, Custom, ColEnd = 0xffff };

    NamedParameter<QString, Key_delimiters, Tag_delimiters> _delimiters;
    NamedParameter<QString, Key_indexVector, Tag_indexVector> _indexVector;
    NamedParameter<QString, Key_fileNamePattern, Tag_fileNamePattern> _fileNamePattern;
    NamedParameter<int, Key_indexInterpretation, Tag_indexInterpretation> _indexInterpretation;
    NamedParameter<int, Key_columnType, Tag_columnType> _columnType;
    NamedParameter<QString, Key_columnDelimiter, Tag_columnDelimiter> _columnDelimiter;
    NamedParameter<int, Key_columnWidth, Tag_columnWidth> _columnWidth;
    NamedParameter<bool, Key_columnWidthIsConst, Tag_columnWidthIsConst> _columnWidthIsConst;
    NamedParameter<int, Key_dataLine, Tag_dataLine> _dataLine;
    NamedParameter<bool, Key_readFields, Tag_readFields> _readFields;
    NamedParameter<int, Key_fieldsLine, Tag_fieldsLine> _fieldsLine;
    NamedParameter<bool, Key_readUnits, Tag_readUnits> _readUnits;
    NamedParameter<int, Key_unitsLine, Tag_unitsLine> _unitsLine;
    NamedParameter<bool, Key_useDot, Tag_useDot> _useDot;


    char _localSeparator;

  private:
    void save(QSettings& cfg);
    void read(QSettings& cfg);
};

Q_DECLARE_METATYPE(AsciiSourceConfig::Interpretation)
Q_DECLARE_METATYPE(AsciiSourceConfig::ColumnType)

#endif
