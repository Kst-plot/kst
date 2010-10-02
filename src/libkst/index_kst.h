/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2006 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INDEX_KST_H
#define INDEX_KST_H

namespace Kst {

class Index {
  public:
    Index() : isTime(false), index(0), time(0.0) {}
    bool isTime;
    union {
      int index;
      double time;
    };
};

}
#endif

// vim: ts=2 sw=2 et
