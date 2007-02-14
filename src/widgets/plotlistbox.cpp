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

PlotListBox::PlotListBox(QWidget *parent, const char *name)
: DraggableListBox(parent, name) {
  setDragEnabled(true);
  setAcceptDrops(true);
  setSelectionMode(QListBox::Extended);
}


PlotListBox::~PlotListBox() {
}


QDragObject *PlotListBox::dragObject() {
  QStoredDrag *drag = new QStoredDrag("application/x-kst-plot-list", this);

  QStringList entries;
  for (QListBoxItem *entry = firstItem(); entry; entry = entry->next()) {
    if (entry->isSelected()) {
      entries << entry->text();
    }
  }

  QByteArray data;
  QDataStream ds(data, IO_WriteOnly);
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
  QDataStream ds(data, IO_ReadOnly);
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
  QDragObject *o = dragObject();
  if (o && o->dragMove()) {
    QByteArray data = o->encodedData("application/x-kst-plot-list");
    QDataStream ds(data, IO_ReadOnly);
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
