/***************************************************************************
                             draggablelistbox.h
                             -------------------
    begin                : Jul 19, 2006
    copyright            : (C) 2004 The University of Toronto
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

#ifndef DRAGGABLELISTVIEW_H
#define DRAGGABLELISTVIEW_H

class QColorGroup;

#include <q3listview.h>
#include <q3dragobject.h>
//Added by qt3to4:
#include <QMouseEvent>

#include "kst_export.h"

class DraggableListView : public Q3ListView {
  Q_OBJECT
  public:
    DraggableListView(QWidget *parent = 0L, const char *name = 0L);
    virtual ~DraggableListView();

    virtual Q3DragObject *dragObject();

    bool dragEnabled() const;
    virtual void setDragEnabled(bool enabled);

  protected:
    virtual void startDrag();
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

  private:
    QPoint _pressPos;
    bool _dragEnabled;
} KST_EXPORT;

#endif
// vim: ts=2 sw=2 et
