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

#ifndef KSTPLOTVIEW_H
#define KSTPLOTVIEW_H

#include <QGraphicsView>

#include "kst_export.h"

class QUndoStack;
class KstPlotItem;

class KST_EXPORT KstPlotView : public QGraphicsView
{
  Q_OBJECT
public:
  KstPlotView();
  virtual ~KstPlotView();

  QUndoStack *undoStack() const;

  KstPlotItem* currentPlotItem() const;

private:
  QUndoStack *_undoStack;
  KstPlotItem *_currentPlotItem;
};

#endif

// vim: ts=2 sw=2 et
