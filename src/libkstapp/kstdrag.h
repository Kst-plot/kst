/***************************************************************************
                   kstdrag.h: base class for drag objects
                             -------------------
    begin                : Apr 06, 2004
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

#ifndef KSTDRAG_H
#define KSTDRAG_H

#include <qdragobject.h>


class KstDrag : public QStoredDrag {
  public:
    KstDrag(const char *mimeType, QWidget *dragSource);
    virtual ~KstDrag();

    // FIXME: add dcop ID here
};


#endif
// vim: ts=2 sw=2 et
