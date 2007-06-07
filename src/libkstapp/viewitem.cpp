/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "viewitem.h"
#include "kstapplication.h"
#include "tabwidget.h"

#include <QDebug>
#include <QGraphicsScene>

namespace Kst {

ViewItem::ViewItem(View *parent)
  : QObject(parent) {
#ifdef DEBUG_GEOMETRY
  QColor semiRed(QColor(255, 0, 0, 50));
  setPen(semiRed);
  setBrush(semiRed);
#endif

  connect(parent, SIGNAL(mouseModeChanged()), this, SLOT(mouseModeChanged()));
}


ViewItem::~ViewItem() {
}


View *ViewItem::parentView() const {
  return qobject_cast<View*>(parent());
}


void ViewItem::mouseModeChanged() {
  if (parentView()->mouseMode() == View::Move)
    _originalPosition = pos();
  else if (_originalPosition != pos())
    new MoveCommand(this, _originalPosition, pos());
}


ViewItemCommand::ViewItemCommand(const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(kstApp->mainWindow()->tabWidget()->currentView()->currentPlotItem()) {
  if (addToStack)
    _item->parentView()->undoStack()->push(this);
}


ViewItemCommand::ViewItemCommand(ViewItem *item, const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(item) {
  if (addToStack)
    _item->parentView()->undoStack()->push(this);
}


ViewItemCommand::~ViewItemCommand() {
}


CreateCommand::CreateCommand(const QString &text, QUndoCommand *parent)
    : ViewCommand(text, false, parent) {
}


CreateCommand::CreateCommand(View *view, const QString &text, QUndoCommand *parent)
    : ViewCommand(view, text, false, parent) {
}


CreateCommand::~CreateCommand() {
}


void CreateCommand::undo() {
  if (_item)
    _item->hide();
}


void CreateCommand::redo() {
  if (!_item)
    createItem();

  _item->show();
}


void CreateCommand::creationComplete() {
  _view->undoStack()->push(this);
}


void MoveCommand::undo() {
  _item->setPos(_originalPos);
}


void MoveCommand::redo() {
  _item->setPos(_newPos);
}


}

#include "viewitem.moc"

// vim: ts=2 sw=2 et
