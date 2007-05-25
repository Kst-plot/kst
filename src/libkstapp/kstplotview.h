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
  enum MouseMode { Default, Move, Create };
  enum CreationEvent {
    MousePress = 0x0,
    MouseRelease =0x1,
    MouseMove = 0x2
  };
  Q_DECLARE_FLAGS(CreationEvents, CreationEvent)

  KstPlotView();
  virtual ~KstPlotView();

  QUndoStack *undoStack() const;
  KstPlotItem* currentPlotItem() const;

  MouseMode mouseMode() const;
  void setMouseMode(MouseMode mode);

  QPolygonF creationPolygon(CreationEvents events) const;

Q_SIGNALS:
  void mouseModeChanged();
  void creationPolygonChanged(KstPlotView::CreationEvent event);
  void resized();

protected:
  bool eventFilter(QObject *obj, QEvent *event);
  void resizeEvent(QResizeEvent *event);

private:
  QUndoStack *_undoStack;
  KstPlotItem *_currentPlotItem;
  MouseMode _mouseMode;
  QPolygonF _creationPolygonPress;
  QPolygonF _creationPolygonMove;
  QPolygonF _creationPolygonRelease;
};

#endif

// vim: ts=2 sw=2 et
