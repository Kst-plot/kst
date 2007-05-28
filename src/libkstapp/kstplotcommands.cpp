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

#include "kstplotcommands.h"
#include "kstapplication.h"
#include "kstplotview.h"
#include "viewitem.h"
#include "labelitem.h"
#include "lineitem.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

namespace Kst {
ViewCommand::ViewCommand(const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(kstApp->mainWindow()->currentPlotView()) {
  if (addToStack)
    _view->undoStack()->push(this);
}


ViewCommand::ViewCommand(KstPlotView *view, const QString &text,
                                       bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(view) {
  if (addToStack)
    _view->undoStack()->push(this);
}


ViewCommand::~ViewCommand() {
}


ViewItemCommand::ViewItemCommand(const QString &text, bool addToStack, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(kstApp->mainWindow()->currentPlotView()->currentPlotItem()) {
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


CreateCommand::CreateCommand(KstPlotView *view, const QString &text, QUndoCommand *parent)
    : ViewCommand(view, text, false, parent) {
}


CreateCommand::~CreateCommand() {
}


void CreateCommand::undo() {
  if (_item)
    _item->graphicsItem()->hide();
}


void CreateCommand::redo() {
  if (!_item)
    createItem();

  _item->graphicsItem()->show();
}


void CreateCommand::creationComplete() {
  _view->undoStack()->push(this);
}


void CreateLabelCommand::createItem() {
  _item = new LabelItem(_view);
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));

  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));

}


void CreateLineCommand::createItem() {
  _item = new LineItem(_view);
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));

  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}


void MoveCommand::undo() {
  _item->graphicsItem()->setPos(_originalPos);
}


void MoveCommand::redo() {
  _item->graphicsItem()->setPos(_newPos);
}

}

#include "kstplotcommands.moc"

// vim: ts=2 sw=2 et
