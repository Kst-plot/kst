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

#include <qapplication.h>
#include "draggablelistbox.h"

DraggableListBox::DraggableListBox(QWidget *parent, const char *name)
: QListBox(parent, name), _pressPos(-1, -1), _dragEnabled(false) {
}


DraggableListBox::~DraggableListBox() {
}


bool DraggableListBox::dragEnabled() const {
  return _dragEnabled;
}


void DraggableListBox::setDragEnabled(bool en) {
  _dragEnabled = en;
}


QDragObject *DraggableListBox::dragObject() {
  return 0L;
}


void DraggableListBox::mousePressEvent(QMouseEvent *e) {
  if (_dragEnabled) {
    _pressPos = QPoint(-1, -1);

    if (e->button() & Qt::LeftButton && !isRubberSelecting()) {
      QListBoxItem *item;
      if ((item = itemAt(e->pos()))) {
        setCurrentItem(item);
        if (!item->isSelected()) {
          if (!(e->state() & Qt::ControlButton)) {
            clearSelection();
          }
          setSelected(item, true);
        }
        _pressPos = e->pos();
        e->accept();
        return;
      }
    }
  }

  QListBox::mousePressEvent(e);
}


void DraggableListBox::mouseReleaseEvent(QMouseEvent *e) {
  _pressPos = QPoint(-1, -1);
  QListBox::mouseReleaseEvent(e);
}


void DraggableListBox::mouseMoveEvent(QMouseEvent *e) {
  if (_dragEnabled && e->state() & Qt::LeftButton && _pressPos != QPoint(-1, -1)) {
    QPoint delta = e->pos() - _pressPos;
    if (delta.manhattanLength() > QApplication::startDragDistance()) {
      _pressPos = QPoint(-1, -1);
      startDrag();
    }
    e->accept();
  } else {
    QListBox::mouseMoveEvent(e);
  }
}


void DraggableListBox::startDrag() {
  QDragObject *o = dragObject();
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
