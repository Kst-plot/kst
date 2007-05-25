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
#include "kstplotitems.h"

#include <QDebug>
#include <QObject>
#include <QInputDialog>
#include <QGraphicsScene>

KstPlotViewCommand::KstPlotViewCommand(const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(kstApp->mainWindow()->currentPlotView()) {
  _view->undoStack()->push(this);
}


KstPlotViewCommand::KstPlotViewCommand(KstPlotView *view, const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(view) {
  _view->undoStack()->push(this);
}


KstPlotViewCommand::~KstPlotViewCommand() {
}


KstPlotItemCommand::KstPlotItemCommand(const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(kstApp->mainWindow()->currentPlotView()->currentPlotItem()) {
  _item->parentView()->undoStack()->push(this);
}


KstPlotItemCommand::KstPlotItemCommand(KstPlotItem *item, const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(item) {
  _item->parentView()->undoStack()->push(this);
}


KstPlotItemCommand::~KstPlotItemCommand() {
}


CreateCommand::CreateCommand(const QString &text, QUndoCommand *parent)
    : KstPlotViewCommand(text, parent) {
}


CreateCommand::CreateCommand(KstPlotView *view, const QString &text, QUndoCommand *parent)
    : KstPlotViewCommand(view, text, parent) {
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


void CreateLabelCommand::createItem() {
  bool ok;
  QString text = QInputDialog::getText(_view, QObject::tr("label"),
                                       QObject::tr("label:"), QLineEdit::Normal,
                                       QString::null, &ok);
  if (ok && !text.isEmpty()) {
    _item = new LabelItem(text, _view);
    _view->scene()->addItem(_item->graphicsItem());
    _item->graphicsItem()->setZValue(1);
  }
}


void CreateLineCommand::createItem() {
    _item = new LineItem(_view);
}

// vim: ts=2 sw=2 et
