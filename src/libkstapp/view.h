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

#include "curveplacement.h"

class QUndoStack;
class QXmlStreamWriter;

namespace Kst {

class ViewItem;
class LayoutBoxItem;

class KST_EXPORT View : public QGraphicsView
{
  Q_OBJECT
  public:
    enum ViewMode { Data, Layout };
    enum MouseMode { Default, Move, Create, Resize, Scale, Rotate };
    enum CreationEvent {
      MousePress = 0x0,
      MouseRelease =0x1,
      MouseMove = 0x2
    };
    Q_DECLARE_FLAGS(CreationEvents, CreationEvent)

    View();
    virtual ~View();

    virtual void save(QXmlStreamWriter &xml);

    bool useOpenGL() const;
    void setUseOpenGL(bool useOpenGL);

    QUndoStack *undoStack() const;

    ViewItem *selectedViewItem() const;

    ViewMode viewMode() const;
    void setViewMode(ViewMode mode);

    MouseMode mouseMode() const;
    void setMouseMode(MouseMode mode);

    QPolygonF creationPolygon(CreationEvents events) const;

    LayoutBoxItem* layoutBoxItem() const { return _layoutBoxItem; }
    void setLayoutBoxItem(LayoutBoxItem *layoutBoxItem) { _layoutBoxItem = layoutBoxItem; }

    bool showGrid() const { return _showGrid; }
    void setShowGrid(bool showGrid);

    QFont defaultFont(double scale = 1.0) const;

    QSizeF gridSpacing() const { return _gridSpacing; }
    void setGridSpacing(const QSizeF &gridSpacing);

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
    void viewModeChanged(View::ViewMode oldMode);
    void mouseModeChanged(View::MouseMode oldMode);
    void creationPolygonChanged(View::CreationEvent event);

  public Q_SLOTS:
    void createLayout(int columns = 0);
    void appendToLayout(CurvePlacement::Layout layout, ViewItem* item, int columns = 0);

  protected:
    bool event(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);

  private Q_SLOTS:
    void updateSettings();
    void updateBrush();
    void updateFont();

  private:
    void updateChildGeometry(const QRectF &oldSceneRect);

  private:
    QUndoStack *_undoStack;
    ViewMode _viewMode;
    MouseMode _mouseMode;
    LayoutBoxItem *_layoutBoxItem;
    QPolygonF _creationPolygonPress;
    QPolygonF _creationPolygonMove;
    QPolygonF _creationPolygonRelease;
    QFont _defaultFont;
    bool _useOpenGL;
    QSizeF _gridSpacing;
    bool _showGrid;
    bool _snapToGridHorizontal;
    bool _snapToGridVertical;
};

}

#endif

// vim: ts=2 sw=2 et
