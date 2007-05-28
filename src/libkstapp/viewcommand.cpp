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

#include "viewcommand.h"
#include "kstapplication.h"
#include "kstplotview.h"

#include <QDebug>
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

}


// vim: ts=2 sw=2 et
