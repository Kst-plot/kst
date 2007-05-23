/***************************************************************************
                   kstplotdrag.h: class for drag objects
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

#ifndef KSTPLOTDRAG_H
#define KSTPLOTDRAG_H

#include "kstdrag.h"
#include "kstplotbase.h"

class KstPlotDrag : public KstDrag {
  public:
    KstPlotDrag(QWidget *dragSource);
    virtual ~KstPlotDrag();

    static const char *mimeType();
    void setPlots(const KstViewObjectList& l);

    static KstViewObjectList decodedContents(QByteArray& a);
};

KstPlotDrag& operator<<(KstPlotDrag& drag, KstViewObject obj);
KstPlotDrag& operator<<(KstPlotDrag& drag, KstViewObjectList objs);


#endif
// vim: ts=2 sw=2 et
