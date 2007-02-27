/***************************************************************************
                               plotlistbox.cpp
                              -----------------
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

#include "plotlistbox.h"
//Added by qt3to4:
#include <QDragMoveEvent>
#include <QDropEvent>

PlotListBox::PlotListBox(QWidget *parent, const char *name)
: DraggableListBox(parent, name) {
  setDragEnabled(true);
  setAcceptDrops(true);
  setSelectionMode(Q3ListBox::Extended);
}


PlotListBox::~PlotListBox() {
}


Q3DragObject *PlotListBox::dragObject() {
  Q3StoredDrag *drag = new Q3StoredDrag("application/x-kst-plot-list", this);

  QStringList entries;
  for (Q3ListBoxItem *entry = firstItem(); entry; entry = entry->next()) {
    if (entry->isSelected()) {
      entries << entry->text();
    }
  }

  QByteArray data;
  QDataStream ds(data, QIODevice::WriteOnly);
  ds << entries;
  drag->setEncodedData(data);

  return drag;
}


void PlotListBox::dragMoveEvent(QDragMoveEvent *e) {
  e->accept(e->provides("application/x-kst-plot-list") && e->source() != this);
}


void PlotListBox::dropEvent(QDropEvent *e) {
  if (!e->provides("application/x-kst-plot-list") || e->source() == this) {
    e->accept(false);
    return;
  }

  QByteArray data = e->encodedData("application/x-kst-plot-list");
  QDataStream ds(data, QIODevice::ReadOnly);
  QStringList entries;
  ds >> entries;
  for (QStringList::ConstIterator i = entries.begin(); i != entries.end(); ++i) {
    insertItem(*i);
  }
  if (entries.count() > 0) {
    emit changed();
  }
  clearSelection();
  e->accept(true);
}


void PlotListBox::startDrag() {
  Q3DragObject *o = dragObject();
  if (o && o->dragMove()) {
    QByteArray data = o->encodedData("application/x-kst-plot-list");
    QDataStream ds(data, QIODevice::ReadOnly);
    QStringList entries;
    ds >> entries;
    for (QStringList::ConstIterator i = entries.begin(); i != entries.end(); ++i) {
      delete findItem(*i);
    }
    if (entries.count() > 0) {
      emit changed();
    }
    clearSelection();
  }
}

#include "plotlistbox.moc"
// vim: ts=2 sw=2 et
