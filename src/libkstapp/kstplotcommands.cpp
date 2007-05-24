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

KstPlotViewCommand::KstPlotViewCommand(const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(kstApp->mainWindow()->currentPlotView()) {
}


KstPlotViewCommand::KstPlotViewCommand(KstPlotView *view, const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(view) {
}


KstPlotViewCommand::~KstPlotViewCommand() {
}


KstPlotItemCommand::KstPlotItemCommand(const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(kstApp->mainWindow()->currentPlotView()->currentPlotItem()) {
}


KstPlotItemCommand::KstPlotItemCommand(KstPlotItem *item, const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _item(item) {
}


KstPlotItemCommand::~KstPlotItemCommand() {
}

// vim: ts=2 sw=2 et
