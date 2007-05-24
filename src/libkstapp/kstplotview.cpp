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

#include "kstplotview.h"
#include "kstmainwindow.h"
#include "kstapplication.h"

#include <QUndoStack>
#include <QGraphicsScene>

KstPlotView::KstPlotView()
    : QGraphicsView(kstApp->mainWindow()) {

  _undoStack = new QUndoStack(this);

  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->addText("Hello, Kst Plot!");
  setScene(scene);

}


KstPlotView::~KstPlotView() {
}


QUndoStack *KstPlotView::undoStack() const {
  return _undoStack;
}

#include "kstplotview.moc"

// vim: ts=2 sw=2 et
