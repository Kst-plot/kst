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

#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

#include "kst_export.h"

class QUndoStack;
namespace Kst {
class ViewItem;

class KST_EXPORT View : public QGraphicsView
{
  Q_OBJECT
public:
  enum MouseMode { Default, Move, Create, Resize, Rotate };
  enum CreationEvent {
    MousePress = 0x0,
    MouseRelease =0x1,
    MouseMove = 0x2
  };
  Q_DECLARE_FLAGS(CreationEvents, CreationEvent)

  View();
  virtual ~View();

  QUndoStack *undoStack() const;
  ViewItem* currentPlotItem() const;

  MouseMode mouseMode() const;
  void setMouseMode(MouseMode mode);

  QPolygonF creationPolygon(CreationEvents events) const;

Q_SIGNALS:
  void mouseModeChanged();
  void creationPolygonChanged(View::CreationEvent event);

protected:
  bool eventFilter(QObject *obj, QEvent *event);
  void setVisible(bool visible);
  void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
  void initializeSceneRect();

private:
  QUndoStack *_undoStack;
  ViewItem *_currentPlotItem;
  MouseMode _mouseMode;
  QPolygonF _creationPolygonPress;
  QPolygonF _creationPolygonMove;
  QPolygonF _creationPolygonRelease;
};

}

#endif

// vim: ts=2 sw=2 et
