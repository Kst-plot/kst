/***************************************************************************
                            draggablelistview.cpp
                             -------------------
    begin                : Jul 19, 2006
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

#include <qapplication.h>
//Added by qt3to4:
#include <QMouseEvent>
#include "draggablelistview.h"

DraggableListView::DraggableListView(QWidget *parent, const char *name)
: Q3ListView(parent, name), _pressPos(-1, -1), _dragEnabled(false) {
}


DraggableListView::~DraggableListView() {
}


bool DraggableListView::dragEnabled() const {
  return _dragEnabled;
}


void DraggableListView::setDragEnabled(bool en) {
  _dragEnabled = en;
}


Q3DragObject *DraggableListView::dragObject() {
  return 0L;
}


void DraggableListView::mousePressEvent(QMouseEvent *e) {
  if (_dragEnabled) {
    _pressPos = QPoint(-1, -1);

    if (e->button() & Qt::LeftButton) {
      Q3ListViewItem *item;
      if ((item = itemAt(e->pos()))) {
        setCurrentItem(item);
        if (!item->isSelected()) {
          if (!(e->state() & Qt::ControlModifier)) {
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

  Q3ListView::mousePressEvent(e);
}


void DraggableListView::mouseReleaseEvent(QMouseEvent *e) {
  _pressPos = QPoint(-1, -1);
  Q3ListView::mouseReleaseEvent(e);
}


void DraggableListView::mouseMoveEvent(QMouseEvent *e) {
  if (_dragEnabled && e->state() & Qt::LeftButton && _pressPos != QPoint(-1, -1)) {
    QPoint delta = e->pos() - _pressPos;
    if (delta.manhattanLength() > QApplication::startDragDistance()) {
      _pressPos = QPoint(-1, -1);
      startDrag();
    }
    e->accept();
  } else {
    Q3ListView::mouseMoveEvent(e);
  }
}


void DraggableListView::startDrag() {
  Q3DragObject *o = dragObject();
  if (o) {
    o->drag();
  }
}

#include "draggablelistview.moc"
// vim: ts=2 sw=2 et
