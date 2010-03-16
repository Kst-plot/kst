/***************************************************************************
                     asciisource_p.h
                             -------------------
    begin                :  Mar 16 2010
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
#ifndef KST_ASCII_SOUTCE_P_H
#define KST_ASCII_SOUTCE_P_H

#include "asciisource.h"
#include "namedparameter.h"


class AsciiSource::Config {

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
    static const char Key_dataLine[];
    static const char Tag_dataLine[];
    static const char Key_readFields[];
    static const char Tag_readFields[];
    static const char Key_useDot[];
    static const char Tag_useDot[];
    static const char Key_fieldsLine[];
    static const char Tag_fieldsLine[];

  public:
    Config();

    static const QString asciiTypeKey();

    void save(QSettings& cfg);
    void read(QSettings& cfg);
    void readGroup(QSettings& cfg, const QString& fileName = QString());

    void save(QXmlStreamWriter& s);
    void parseProperties(QXmlStreamAttributes &properties);

    void load(const QDomElement& e);

    Parameter<QString, Key_delimiters, Tag_delimiters> _delimiters;
    Parameter<QString, Key_indexVector, Tag_indexVector> _indexVector;
    Parameter<QString, Key_fileNamePattern, Tag_fileNamePattern> _fileNamePattern;

    enum Interpretation { Unknown = 0, INDEX, CTime, Seconds, IntEnd = 0xffff };
    Parameter<Interpretation, Key_indexInterpretation, Tag_indexInterpretation> _indexInterpretation;

    enum ColumnType { Whitespace = 0, Fixed, Custom, ColEnd = 0xffff };
    Parameter<ColumnType, Key_columnType, Tag_columnType> _columnType;

    Parameter<QString, Key_columnDelimiter, Tag_columnDelimiter> _columnDelimiter;
    Parameter<int, Key_columnWidth, Tag_columnWidth> _columnWidth;
    Parameter<int, Key_dataLine, Tag_dataLine> _dataLine;
    Parameter<bool, Key_readFields, Tag_readFields> _readFields;
    Parameter<int, Key_fieldsLine, Tag_fieldsLine> _fieldsLine;
    Parameter<bool, Key_useDot, Tag_useDot> _useDot;

    char _localSeparator;


};

Q_DECLARE_METATYPE(AsciiSource::Config::Interpretation)
Q_DECLARE_METATYPE(AsciiSource::Config::ColumnType)

#endif
