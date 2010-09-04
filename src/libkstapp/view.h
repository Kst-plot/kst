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
class PlotItem;

class View : public QGraphicsView
{
  Q_OBJECT
  public:
    enum ViewMode { Data, Layout };
    enum MouseMode { Default, Move, Create, Resize, Scale, Rotate };
    enum CreationEvent {
      MousePress = 0x0,
      MouseRelease =0x1,
      MouseMove = 0x2,
      EscapeEvent = 0x3
    };
    Q_DECLARE_FLAGS(CreationEvents, CreationEvent)

    View();
    View(QWidget* parent);
    virtual ~View();

    void init();

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

    QFont defaultFont(double pointSize) const;

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

    void setPrinting(bool printing) { _printing = printing; }
    bool isPrinting() { return _printing; }

    void setDataMode(bool dataMode) { _dataMode = dataMode; }
    bool isDataMode() { return _dataMode; }

    virtual void contextMenuEvent();

    bool plotBordersDirty() const {return _plotBordersDirty;}
    void setPlotBordersDirty(bool dirty) {_plotBordersDirty = dirty;}

    void configurePlotFontDefaults(PlotItem *plot);

    void resetPlotFontSizes(double pointSize = 0);

    void setFontRescale(double rescale) {_fontRescale = rescale;}
    double fontRescale() const {return _fontRescale;}

    void setChildMaximized(bool isMax) { _childMaximized = isMax;}
    bool childMaximized() const {return _childMaximized;}

  Q_SIGNALS:
    void viewModeChanged(View::ViewMode oldMode);
    void mouseModeChanged(View::MouseMode oldMode);
    void creationPolygonChanged(View::CreationEvent event);

  public Q_SLOTS:
    void createLayout(int columns = 0);
    void appendToLayout(CurvePlacement::Layout layout, ViewItem* item, int columns = 0);
    void createCustomLayout();
    void viewChanged();
    void forceChildResize(QRectF oldRect, QRectF newRect);
    void processResize(QSize size);

  protected:
    bool event(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent *event=NULL);
    void drawBackground(QPainter *painter, const QRectF &rect);
    void addTitle(QMenu *menu) const;

    QAction *_editAction;
    QAction *_autoLayoutAction;
    QAction *_customLayoutAction;

  private Q_SLOTS:
    void updateSettings();
    void loadSettings();
    void updateFont();
    void updateBrush();
    virtual void edit();

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
    bool _plotBordersDirty;
    bool _printing;
    bool _dataMode;
    double _fontRescale;
    bool _childMaximized;
};

}

#endif

// vim: ts=2 sw=2 et
