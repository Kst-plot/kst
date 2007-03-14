/***************************************************************************
                             vectorlistview.h
                             ---------------
    begin                : Apr 17, 2006
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

#ifndef VECTORLISTVIEW_H
#define VECTORLISTVIEW_H

#include "draggablelistview.h"
#include "kst_export.h"
//Added by qt3to4:
#include <QDragMoveEvent>
#include <QDropEvent>

class VectorListView : public DraggableListView {
  Q_OBJECT
  public:
    KST_EXPORT VectorListView(QWidget *parent = 0L, const char *name = 0L);
    virtual ~VectorListView();

    virtual Q3DragObject *dragObject();

  protected:
    virtual void dragMoveEvent(QDragMoveEvent *e);
    virtual void dropEvent(QDropEvent *e);
    virtual void startDrag();

} KST_EXPORT;

#endif
// vim: ts=2 sw=2 et
