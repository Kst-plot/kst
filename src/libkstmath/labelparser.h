/***************************************************************************
                              labelparser.h
                             ----------------
    begin                : Dec 14 2004
                           Copyright (C) 2004, The University of Toronto
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

#ifndef LABELPARSER_H
#define LABELPARSER_H

#include "kstscalar.h"
#include "kststring.h"
#include "kst_export.h"

#include <qcolor.h>

typedef Q_UINT16 KstLJustifyType;
typedef Q_UINT8  KstLHJustifyType;
typedef Q_UINT8  KstLVJustifyType;
#define KST_JUSTIFY_H(x)     (x & 0x000000ffL)
#define KST_JUSTIFY_H_NONE    0
#define KST_JUSTIFY_H_LEFT    1
#define KST_JUSTIFY_H_RIGHT   2
#define KST_JUSTIFY_H_CENTER  3

#define KST_JUSTIFY_V(x)     ((x >> 8) & 0x000000ffL)
#define KST_JUSTIFY_V_NONE    0
#define KST_JUSTIFY_V_TOP     1
#define KST_JUSTIFY_V_BOTTOM  2
#define KST_JUSTIFY_V_CENTER  3

#define SET_KST_JUSTIFY(h,v) ( ( h & 0x000000ffL ) | ( ( v & 0x000000ffL ) << 8 ) )


namespace Label {
  struct KST_EXPORT ChunkAttributes {
    ChunkAttributes() : bold(false), italic(false), underline(false) {}
    inline bool empty() const { return !bold && !italic && !underline && !color.isValid(); }
    bool bold;
    bool italic;
    bool underline;
    QColor color;
  };

  struct KST_EXPORT Chunk {
    enum VOffset { None = 0, Up = 1, Down = 2 };
    Chunk(Chunk *parent, VOffset = None, bool isGroup = false, bool inherit = false);
    ~Chunk();

    bool locked() const;
    Chunk *next, *prev, *up, *down, *group;
    bool scalar : 1;
    bool linebreak : 1;
    bool tab : 1;
    bool vector : 1;
    ChunkAttributes attributes;
    VOffset vOffset : 2;
    QString text;
    QString expression;
  };


  struct KST_EXPORT Parsed {
    Parsed();
    ~Parsed();

    Chunk *chunk;
  };


  extern KST_EXPORT Parsed *parse(const QString&, bool interpret = true, bool interpretNewLine = true);
}

#endif
// vim: ts=2 sw=2 et
