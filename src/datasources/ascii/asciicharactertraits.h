/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2010 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ASCII_CHARACTER_TRAITS_H
#define ASCII_CHARACTER_TRAITS_H

#include <QString>

namespace AsciiCharacterTraits
{

struct LineEndingType {
  bool is_crlf;
  char character;
  inline bool isCR() const { return character == '\r'; }
  inline bool isLF() const { return character == '\n'; }
};

// column and comment delimiter functions

struct AlwaysTrue {
  inline bool operator()() const {
    return true;
  }
};

struct AlwaysFalse {
  inline bool operator()() const {
    return false;
  }
};

struct NoDelimiter {
  inline bool operator()(const char) const {
    return false;
  }
};

struct  IsWhiteSpace {
  inline bool operator()(const char c) const {
    return c == ' ' || c == '\t';
  }
};

struct IsDigit {
  inline bool operator()(const char c) const {
    return (c >= 48) && (c <= 57) ? true : false;
  }
};

struct IsCharacter {
  inline IsCharacter(char c) : character(c) {
  }
  const char character;
  inline bool operator()(const char c) const {
    return character == c;
  }
};

struct IsInString {
  inline IsInString(const QString& s) : str(s), chars(s.size()) {
    QByteArray ascii = str.toLatin1();
    for (int i = 0; i < 6 && i < chars; i++) {
      ch[i] = ascii[i];
    }
  }
  const QString str;
  const int chars;
  char ch[6];
  inline bool operator()(const char c) const {
    switch (chars) {
    case 0: return false;
    case 1: return ch[0]==c;
    case 2: return ch[0]==c || ch[1]==c;
    case 3: return ch[0]==c || ch[1]==c || ch[2]==c;
    case 4: return ch[0]==c || ch[1]==c || ch[2]==c || ch[3]==c;
    case 5: return ch[0]==c || ch[1]==c || ch[2]==c || ch[3]==c || ch[4]==c;
    case 6: return ch[0]==c || ch[1]==c || ch[2]==c || ch[3]==c || ch[4]==c || ch[5]==c;
    default: return str.contains(c);
    }
  }
};

struct IsLineBreakLF {
  inline IsLineBreakLF(const LineEndingType&) : size(1) {
  }
  const int size;
  inline bool operator()(const char c) const {
    return c == '\n';
  }
};

struct IsLineBreakCR {
  inline IsLineBreakCR(const LineEndingType& t) : size( t.is_crlf ? 2 : 1 ) {
  }
  const int size;
  inline bool operator()(const char c) const {
    return c == '\r';
  }
};

}

#endif
// vim: ts=2 sw=2 et
