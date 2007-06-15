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

#include "vectorlistview.h"
//Added by qt3to4:
#include <QDragMoveEvent>
#include <QDropEvent>

VectorListView::VectorListView(QWidget *parent, const char *name)
: DraggableListView(parent, name) {
  setAcceptDrops(true);
  setSelectionMode(Q3ListView::Extended);
}


VectorListView::~VectorListView() {
}


Q3DragObject *VectorListView::dragObject() {
  Q3StoredDrag *drag = new Q3StoredDrag("application/x-kst-vector-list", this);

  QStringList entries;
  for (Q3ListViewItem *entry = firstChild(); entry; entry = entry->nextSibling()) {
    if (entry->isSelected()) {
      entries << entry->text(0);
    }
  }

  QByteArray data;
  QDataStream ds(&data, QIODevice::WriteOnly);
  ds << entries;
  drag->setEncodedData(data);

  return drag;
}


void VectorListView::dragMoveEvent(QDragMoveEvent *e) {
  if (e->mimeData()->hasFormat("application/x-kst-vector-list") && e->source() != this)
    e->acceptProposedAction();
}


void VectorListView::dropEvent(QDropEvent *e) {
  if (!e->mimeData()->hasFormat("application/x-kst-vector-list") || e->source() == this) {
    return;
  }

  QByteArray data = e->encodedData("application/x-kst-vector-list");
  QDataStream ds(&data, QIODevice::ReadOnly);
  QStringList entries;
  ds >> entries;
  Q3ListViewItem *last = lastItem();
  for (QStringList::ConstIterator i = entries.begin(); i != entries.end(); ++i) {
    Q3ListViewItem *j = new Q3ListViewItem(this, *i);
    j->setDragEnabled(true);
    j->moveItem(last);
    last = j;
  }

  clearSelection();
  e->acceptProposedAction();
  emit dropped(e);
}


void VectorListView::startDrag() {
  Q3DragObject *o = dragObject();
  if (o && o->dragMove()) {
    QByteArray data = o->encodedData("application/x-kst-vector-list");
    QDataStream ds(&data, QIODevice::ReadOnly);
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
