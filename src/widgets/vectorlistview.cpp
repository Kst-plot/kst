/***************************************************************************
                              vectorlistview.cpp
                              -----------------
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

#include <ksdebug.h>
#include "vectorlistview.h"

VectorListView::VectorListView(QWidget *parent, const char *name)
: DraggableListView(parent, name) {
  setAcceptDrops(true);
  setSelectionMode(QListView::Extended);
}


VectorListView::~VectorListView() {
}


QDragObject *VectorListView::dragObject() {
  QStoredDrag *drag = new QStoredDrag("application/x-kst-vector-list", this);

  QStringList entries;
  for (QListViewItem *entry = firstChild(); entry; entry = entry->nextSibling()) {
    if (entry->isSelected()) {
      entries << entry->text(0);
    }
  }

  QByteArray data;
  QDataStream ds(data, IO_WriteOnly);
  ds << entries;
  drag->setEncodedData(data);

  return drag;
}


void VectorListView::dragMoveEvent(QDragMoveEvent *e) {
  e->accept(e->provides("application/x-kst-vector-list") && e->source() != this);
}


void VectorListView::dropEvent(QDropEvent *e) {
  if (!e->provides("application/x-kst-vector-list") || e->source() == this) {
    e->accept(false);
    return;
  }

  QByteArray data = e->encodedData("application/x-kst-vector-list");
  QDataStream ds(data, IO_ReadOnly);
  QStringList entries;
  ds >> entries;
  QListViewItem *last = lastItem();
  for (QStringList::ConstIterator i = entries.begin(); i != entries.end(); ++i) {
    QListViewItem *j = new QListViewItem(this, *i);
    j->setDragEnabled(true);
    j->moveItem(last);
    last = j;
  }

  clearSelection();
  e->accept(true);
  emit dropped(e);
}


void VectorListView::startDrag() {
  QDragObject *o = dragObject();
  if (o && o->dragMove()) {
    QByteArray data = o->encodedData("application/x-kst-vector-list");
    QDataStream ds(data, IO_ReadOnly);
    QStringList entries;
    ds >> entries;
    for (QStringList::ConstIterator i = entries.begin(); i != entries.end(); ++i) {
      delete findItem(*i, 0);
    }

    clearSelection();
  }
}

#include "vectorlistview.moc"
// vim: ts=2 sw=2 et
