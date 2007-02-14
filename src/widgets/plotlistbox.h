/***************************************************************************
                              plotlistbox.h
                             ---------------
    begin                : Jul 19, 2004
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

#ifndef PLOTLISTBOX_H
#define PLOTLISTBOX_H

#include "draggablelistbox.h"
#include "kst_export.h"

class PlotListBox : public DraggableListBox {
  Q_OBJECT
  public:
    KST_EXPORT PlotListBox(QWidget *parent = 0L, const char *name = 0L);
    virtual ~PlotListBox();

    virtual QDragObject *dragObject();

  protected:
    virtual void dragMoveEvent(QDragMoveEvent *e);
    virtual void dropEvent(QDropEvent *e);
    virtual void startDrag();

  signals:
    void changed();
};


#endif
// vim: ts=2 sw=2 et
