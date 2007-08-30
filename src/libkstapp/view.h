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
  enum MouseMode { Default, Move, Create, Resize, Scale, Rotate };
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

  QSizeF gridSpacing() const { return _gridSpacing; }
  void setGridSpacing(const QSizeF &gridSpacing) { _gridSpacing = gridSpacing; }

  bool snapToGridHorizontal() const
  { return _snapToGridHorizontal; }
  void setSnapToGridHorizontal(bool snapToGridHorizontal)
  { _snapToGridHorizontal = snapToGridHorizontal; }

  bool snapToGridVertical() const
  { return _snapToGridVertical; }
  void setSnapToGridVertical(bool snapToGridVertical)
  { _snapToGridVertical = snapToGridVertical; }

  bool snapToGrid() const
  { return _snapToGridHorizontal && _snapToGridVertical; }
  void setSnapToGrid(bool snapToGrid)
  { _snapToGridHorizontal = snapToGrid; _snapToGridVertical = snapToGrid; }

  QPointF snapPoint(const QPointF &point);

Q_SIGNALS:
  void mouseModeChanged(View::MouseMode oldMode);
  void creationPolygonChanged(View::CreationEvent event);

protected:
  bool eventFilter(QObject *obj, QEvent *event);
  void setVisible(bool visible);
  void resizeEvent(QResizeEvent *event);
  void drawBackground(QPainter *painter, const QRectF &rect);

private Q_SLOTS:
  void initializeSceneRect();

private:
  QUndoStack *_undoStack;
  ViewItem *_currentPlotItem;
  MouseMode _mouseMode;
  QPolygonF _creationPolygonPress;
  QPolygonF _creationPolygonMove;
  QPolygonF _creationPolygonRelease;
  QSizeF _gridSpacing;
  bool _snapToGridHorizontal;
  bool _snapToGridVertical;
};

}

#endif

// vim: ts=2 sw=2 et
