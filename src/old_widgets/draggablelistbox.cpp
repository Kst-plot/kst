/***************************************************************************
                             draggablelistbox.cpp
                             -------------------
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

#include "draggablelistbox.h"

#include <qapplication.h>
//Added by qt3to4:
#include <QMouseEvent>

DraggableListBox::DraggableListBox(QWidget *parent, const char *name)
: Q3ListBox(parent, name), _pressPos(-1, -1), _dragEnabled(false) {
}


DraggableListBox::~DraggableListBox() {
}


bool DraggableListBox::dragEnabled() const {
  return _dragEnabled;
}


void DraggableListBox::setDragEnabled(bool en) {
  _dragEnabled = en;
}


Q3DragObject *DraggableListBox::dragObject() {
  return 0L;
}


void DraggableListBox::mousePressEvent(QMouseEvent *e) {
  if (_dragEnabled) {
    _pressPos = QPoint(-1, -1);

    if (e->button() & Qt::LeftButton && !isRubberSelecting()) {
      Q3ListBoxItem *item;
      if ((item = itemAt(e->pos()))) {
        setCurrentItem(item);
        if (!item->isSelected()) {
          clearSelection();
          setSelected(item, true);
        }
        _pressPos = e->pos();
        e->accept();
        return;
      }
    }
  }

  Q3ListBox::mousePressEvent(e);
}


void DraggableListBox::mouseReleaseEvent(QMouseEvent *e) {
  _pressPos = QPoint(-1, -1);
  Q3ListBox::mouseReleaseEvent(e);
}


void DraggableListBox::mouseMoveEvent(QMouseEvent *e) {
  if (_dragEnabled && e->buttons() & Qt::LeftButton && _pressPos != QPoint(-1, -1)) {
    QPoint delta = e->pos() - _pressPos;
    if (delta.manhattanLength() > QApplication::startDragDistance()) {
      _pressPos = QPoint(-1, -1);
      startDrag();
    }
    e->accept();
  } else {
    Q3ListBox::mouseMoveEvent(e);
  }
}


void DraggableListBox::startDrag() {
  Q3DragObject *o = dragObject();
  if (o) {
    o->drag();
  }
}

void DraggableListBox::up() {
  if (count() > 1) {
    QString C;
    for (unsigned i=1; i<count(); i++) {
      if (isSelected(i)) {
        C = text(i);
        removeItem(i);
        --i;
        insertItem(C, i);
        setSelected(i, true);
        while (isSelected(i) && (i<count())) ++i;
      }
    }
  }
}

void DraggableListBox::down() {
  if (count() > 1) {
    QString C;
    for (int i=int(count())-2; i>=0; i--) {
      if (isSelected(i)) {
        C = text(i);
        removeItem(i);
        ++i;
        insertItem(C, i);
        setSelected(i, true);
        while (isSelected(i) && (i>0)) --i;
      }
    }
  }
}

#include "draggablelistbox.moc"
// vim: ts=2 sw=2 et
