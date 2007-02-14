/***************************************************************************
                                 kstindex.h
                             -------------------
    begin                : Oct 17 2006
    copyright            : (C) 2006 The University of Toronto
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

#ifndef KSTINDEX_H
#define KSTINDEX_H

class KstIndex {
  public:
    KstIndex() : isTime(false), index(0), time(0.0) {}
    bool isTime;
    union {
      int index;
      double time;
    };
};

#endif

// vim: ts=2 sw=2 et
