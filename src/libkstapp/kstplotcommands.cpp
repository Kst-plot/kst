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

#include <QObject>
#include <QInputDialog>

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


CreateLabelCommand::CreateLabelCommand()
    : KstPlotViewCommand(QObject::tr("Create Label")) {

  bool ok;
  QString text = QInputDialog::getText(_view, QObject::tr("label"),
                                       QObject::tr("label:"), QLineEdit::Normal,
                                       QString::null, &ok);
  if (ok && !text.isEmpty()) {
    //Create the item!!
  }
}


CreateLabelCommand::CreateLabelCommand(KstPlotView *view)
    : KstPlotViewCommand(view, QObject::tr("Create Label")) {
}


CreateLabelCommand::~CreateLabelCommand() {
}


// vim: ts=2 sw=2 et
