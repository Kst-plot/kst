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

#include "kstplotcommand.h"
#include "kstplotview.h"

KstPlotCommand::KstPlotCommand(KstPlotView *view, const QString &text, QUndoCommand *parent)
    : QUndoCommand(text, parent), _view(view) {
}


KstPlotCommand::~KstPlotCommand() {
}

// vim: ts=2 sw=2 et
