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

#ifndef PLOTITEM_H
#define PLOTITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

#include <QHash>

#include "relation.h"
#include "plotrenderitem.h"
#include "plotiteminterface.h"
#include "plotdefines.h"
#include "plotmarkers.h"

#include "plotaxis.h"

namespace Kst {

class PlotItem;
class PlotAxis;

struct ZoomState {
  QPointer<PlotItem> item;
  QRectF projectionRect;
  int xAxisZoomMode;
  int yAxisZoomMode;
  bool isXAxisLog;
  bool isYAxisLog;
  qreal xLogBase;
  qreal yLogBase;
};


class PlotItem : public ViewItem, public PlotItemInterface
{
  Q_OBJECT
  public:

    PlotItem(View *parent);
    virtual ~PlotItem();

    virtual QString plotName() const; //from PlotItemInterface

    QList<PlotRenderItem*> renderItems() const;
    PlotRenderItem *renderItem(PlotRenderItem::RenderType type);

    virtual void save(QXmlStreamWriter &xml);

    virtual void paint(QPainter *painter);

    /* This is the rectangle of the PlotAxis and includes the axis labels. */
    QRectF plotAxisRect() const;

    /* This is the rectangle of the PlotRenderItem's and includes the actual curves. */
    QRectF plotRect() const;

    QRectF projectionRect() const;
    void setProjectionRect(const QRectF &rect);
    QRectF computedProjectionRect();
    void computedRelationalMax(qreal &minimum, qreal &maximum);
    void computeBorder(Qt::Orientation orientation, qreal &minimum, qreal &maximum) const;

    PlotAxis* xAxis() { return _xAxis; }
    PlotAxis* yAxis() { return _yAxis; }

    bool isTiedZoom() const;
    void setTiedZoom(bool tiedZoom);

    qreal leftLabelMargin() const;
    qreal rightLabelMargin() const;
    qreal topLabelMargin() const;
    qreal bottomLabelMargin() const;

    QString leftLabel() const;
    QString bottomLabel() const;
    QString rightLabel() const;
    QString topLabel() const;

    void setTopSuppressed(bool suppressed);
    void setBottomSuppressed(bool suppressed);
    void setLeftSuppressed(bool suppressed);
    void setRightSuppressed(bool suppressed);

    bool isLeftLabelVisible() const;
    void setLeftLabelVisible(bool visible);

    bool isBottomLabelVisible() const;
    void setBottomLabelVisible(bool visible);

    bool isRightLabelVisible() const;
    void setRightLabelVisible(bool visible);

    bool isTopLabelVisible() const;
    void setTopLabelVisible(bool visible);

    void setLabelsVisible(bool visible);

    qreal axisMarginWidth() const;
    qreal axisMarginHeight() const;

    QString bottomLabelOverride() const;
    void setBottomLabelOverride(const QString &label);

    QString leftLabelOverride() const;
    void setLeftLabelOverride(const QString &label);

    QString topLabelOverride() const;
    void setTopLabelOverride(const QString &label);

    QString rightLabelOverride() const;
    void setRightLabelOverride(const QString &label);

    QString titleOverride() const;
    void setTitleOverride(const QString &label);

    QFont topLabelFont() const;
    void setTopLabelFont(const QFont &font);

    QFont leftLabelFont() const;
    void setLeftLabelFont(const QFont &font);

    QFont rightLabelFont() const;
    void setRightLabelFont(const QFont &font);

    QFont bottomLabelFont() const;
    void setBottomLabelFont(const QFont &font);

    qreal bottomLabelFontScale() const;
    void setBottomLabelFontScale(const qreal scale);

    qreal leftLabelFontScale() const;
    void setLeftLabelFontScale(const qreal scale);

    qreal topLabelFontScale() const;
    void setTopLabelFontScale(const qreal scale);

    qreal rightLabelFontScale() const;
    void setRightLabelFontScale(const qreal scale);

    qreal mapXToPlot(const qreal &x) const;
    qreal mapYToPlot(const qreal &y) const;
    QPointF mapToPlot(const QPointF &point) const;

    QPointF mapToProjection(const QPointF &point);
    QRectF mapToProjection(const QRectF &rect);

    void updateScale();

    qreal xMin() { return _xMin; }
    qreal xMax() { return _xMax; }
    qreal yMin() { return _yMin; }
    qreal yMax() { return _yMax; }

    void triggerUpdate() { emit geometryChanged(); update(rect()); }
    virtual void addToMenuForContextEvent(QMenu &menu);

  Q_SIGNALS:
    void marginsChanged();
    void updatePlotRect();
    void updateAxes();

  public Q_SLOTS:
    void zoomFixedExpression(const QRectF &projection);
    void zoomXRange(const QRectF &projection);
    void zoomYRange(const QRectF &projection);
    void zoomMaximum();
    void zoomMaxSpikeInsensitive();
    void zoomYMeanCentered();
    void zoomXMaximum();
    void zoomXRight();
    void zoomXLeft();
    void zoomXOut();
    void zoomXIn();
    void zoomNormalizeXtoY();
    void zoomLogX();
    void zoomYLocalMaximum();
    void zoomYMaximum();
    void zoomYUp();
    void zoomYDown();
    void zoomYOut();
    void zoomYIn();
    void zoomNormalizeYtoX();
    void zoomLogY();
    virtual void edit();

  private:
    void createActions();
    void createZoomMenu();

    void resetSelectionRect();

    virtual void paintPlot(QPainter *painter);

    virtual void paintMajorGridLines(QPainter *painter);
    virtual void paintMinorGridLines(QPainter *painter);
    virtual void paintMajorTicks(QPainter *painter);
    virtual void paintMinorTicks(QPainter *painter);

    virtual void paintTickLabels(QPainter *painter);
    virtual void paintBottomTickLabels(QPainter *painter);
    virtual void paintLeftTickLabels(QPainter *painter);

    virtual void paintPlotMarkers(QPainter *painter);

    qreal calculatedLabelMarginWidth() const;
    qreal calculatedLabelMarginHeight() const;

    qreal calculatedLeftLabelMargin() const;
    void setCalculatedLeftLabelMargin(qreal margin);

    qreal calculatedRightLabelMargin() const;
    void setCalculatedRightLabelMargin(qreal margin);

    qreal calculatedTopLabelMargin() const;
    void setCalculatedTopLabelMargin(qreal margin);

    qreal calculatedBottomLabelMargin() const;
    void setCalculatedBottomLabelMargin(qreal margin);

    QRectF topLabelRect(bool calc) const;
    QRectF bottomLabelRect(bool calc) const;
    QRectF leftLabelRect(bool calc) const;
    QRectF rightLabelRect(bool calc) const;

    void paintLeftLabel(QPainter *painter);
    QSizeF calculateLeftLabelBound(QPainter *painter);
    void paintBottomLabel(QPainter *painter);
    QSizeF calculateBottomLabelBound(QPainter *painter);
    void paintRightLabel(QPainter *painter);
    QSizeF calculateRightLabelBound(QPainter *painter);
    void paintTopLabel(QPainter *painter);
    QSizeF calculateTopLabelBound(QPainter *painter);

    QFont calculatedTopLabelFont();
    QFont calculatedBottomLabelFont();
    QFont calculatedLeftLabelFont();
    QFont calculatedRightLabelFont();

    qreal calculatedAxisMarginWidth() const;
    void setCalculatedAxisMarginWidth(qreal marginWidth);

    qreal calculatedAxisMarginHeight() const;
    void setCalculatedAxisMarginHeight(qreal marginHeight);

    QSizeF calculateBottomTickLabelBound(QPainter *painter);
    QSizeF calculateLeftTickLabelBound(QPainter *painter);

    ZoomState currentZoomState();
    void setCurrentZoomState(ZoomState zoomState);

  private:
    QHash<PlotRenderItem::RenderType, PlotRenderItem*> _renderers;
    bool _isTiedZoom;
    bool _isLeftLabelVisible;
    bool _isBottomLabelVisible;
    bool _isRightLabelVisible;
    bool _isTopLabelVisible;

    qreal _calculatedLeftLabelMargin;
    qreal _calculatedRightLabelMargin;
    qreal _calculatedTopLabelMargin;
    qreal _calculatedBottomLabelMargin;
    qreal _calculatedLabelMarginWidth;
    qreal _calculatedLabelMarginHeight;
    qreal _calculatedAxisMarginWidth;
    qreal _calculatedAxisMarginHeight;

    PlotAxis *_xAxis;
    PlotAxis *_yAxis;

    QRectF _projectionRect;

    qreal _xMax;
    qreal _xMin;
    qreal _yMax;
    qreal _yMin;

    QRectF _yLabelRect;
    QRectF _xLabelRect;

    QString _leftLabelOverride;
    QString _bottomLabelOverride;
    QString _topLabelOverride;
    QString _rightLabelOverride;

    QFont _leftLabelFont;
    QFont _bottomLabelFont;
    QFont _topLabelFont;
    QFont _rightLabelFont;

    qreal _leftLabelFontScale;
    qreal _bottomLabelFontScale;
    qreal _topLabelFontScale;
    qreal _rightLabelFontScale;

    QMenu *_zoomMenu;
    QAction *_zoomMaximum;
    QAction *_zoomMaxSpikeInsensitive;
    QAction *_zoomPrevious;
    QAction *_zoomYMeanCentered;
    QAction *_zoomXMaximum;
    QAction *_zoomXRight;
    QAction *_zoomXLeft;
    QAction *_zoomXOut;
    QAction *_zoomXIn;
    QAction *_zoomNormalizeXtoY;
    QAction *_zoomLogX;
    QAction *_zoomYLocalMaximum;
    QAction *_zoomYMaximum;
    QAction *_zoomYUp;
    QAction *_zoomYDown;
    QAction *_zoomYOut;
    QAction *_zoomYIn;
    QAction *_zoomNormalizeYtoX;
    QAction *_zoomLogY;

    QUndoStack *_undoStack;

    friend class ZoomCommand;
    friend class ZoomMaximumCommand;
    friend class ZoomMaxSpikeInsensitiveCommand;
    friend class ZoomYMeanCenteredCommand;
    friend class ZoomXMaximumCommand;
    friend class ZoomYLocalMaximumCommand;
    friend class ZoomYMaximumCommand;
    friend class ViewGridLayout;
};

class KST_EXPORT CreatePlotCommand : public CreateCommand
{
  public:
    CreatePlotCommand() : CreateCommand(QObject::tr("Create Plot")) {}
    CreatePlotCommand(View *view) : CreateCommand(view, QObject::tr("Create Plot")) {}
    virtual ~CreatePlotCommand() {}
    virtual void createItem();
};

class KST_EXPORT CreatePlotForCurve : public CreateCommand
{
  public:
    CreatePlotForCurve(bool createLayout, bool appendToLayout)
      : CreateCommand(QObject::tr("Create Plot For Curve")),
        _createLayout(createLayout),
        _appendToLayout(appendToLayout) {}
    CreatePlotForCurve(bool createLayout, bool appendToLayout, View *view)
      : CreateCommand(view, QObject::tr("Create Plot For Curve")),
        _createLayout(createLayout),
        _appendToLayout(appendToLayout) {}
    virtual ~CreatePlotForCurve() {}
    virtual void createItem();

  private:
    bool _createLayout;
    bool _appendToLayout;
};

class PlotItemFactory : public GraphicsFactory {
  public:
    PlotItemFactory();
    ~PlotItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};


class KST_EXPORT ZoomCommand : public ViewItemCommand
{
  public:
    ZoomCommand(PlotItem *item, const QString &text, bool addToStack = true);
    virtual ~ZoomCommand();

    virtual void undo();
    virtual void redo();

    virtual void applyZoomTo(PlotItem *item) = 0;

  private:
    QList<ZoomState> _originalStates;
};


class KST_EXPORT ZoomFixedExpressionCommand : public ZoomCommand
{
  public:
    ZoomFixedExpressionCommand(PlotItem *item, const QRectF &fixed, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Fixed Expression"), addToStack), _fixed(fixed) {}
    virtual ~ZoomFixedExpressionCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    QRectF _fixed;
};


class KST_EXPORT ZoomXRangeCommand : public ZoomCommand
{
  public:
    ZoomXRangeCommand(PlotItem *item, const QRectF &fixed, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom X Range Expression"), addToStack), _fixed(fixed) {}
    virtual ~ZoomXRangeCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    QRectF _fixed;
};


class KST_EXPORT ZoomYRangeCommand : public ZoomCommand
{
  public:
    ZoomYRangeCommand(PlotItem *item, const QRectF &fixed, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom X Range Expression"), addToStack), _fixed(fixed) {}
    virtual ~ZoomYRangeCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    QRectF _fixed;
};


class KST_EXPORT ZoomMaximumCommand : public ZoomCommand
{
  public:
    ZoomMaximumCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Maximum"), addToStack) {}
    virtual ~ZoomMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomMaxSpikeInsensitiveCommand : public ZoomCommand
{
  public:
    ZoomMaxSpikeInsensitiveCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Max Spike Insensitive"), addToStack) {}
    virtual ~ZoomMaxSpikeInsensitiveCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYMeanCenteredCommand : public ZoomCommand
{
  public:
    ZoomYMeanCenteredCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y Mean Centered"), addToStack) {}
    virtual ~ZoomYMeanCenteredCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXMaximumCommand : public ZoomCommand
{
  public:
    ZoomXMaximumCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom X Maximum"), addToStack) {}
    virtual ~ZoomXMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXRightCommand : public ZoomCommand
{
  public:
    ZoomXRightCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Scroll X Right"), addToStack) {}
    virtual ~ZoomXRightCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXLeftCommand : public ZoomCommand
{
  public:
    ZoomXLeftCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Scroll X Left"), addToStack) {}
    virtual ~ZoomXLeftCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXOutCommand : public ZoomCommand
{
  public:
    ZoomXOutCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom X Out"), addToStack) {}
    virtual ~ZoomXOutCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXInCommand : public ZoomCommand
{
  public:
    ZoomXInCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom X In"), addToStack) {}
    virtual ~ZoomXInCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomNormalizeXToYCommand : public ZoomCommand
{
  public:
    ZoomNormalizeXToYCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Normalize X to Y"), addToStack) {}
    virtual ~ZoomNormalizeXToYCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYLocalMaximumCommand : public ZoomCommand
{
  public:
    ZoomYLocalMaximumCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y Local Maximum"), addToStack) {}
    virtual ~ZoomYLocalMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYMaximumCommand : public ZoomCommand
{
  public:
    ZoomYMaximumCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y Maximum"), addToStack) {}
    virtual ~ZoomYMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYUpCommand : public ZoomCommand
{
  public:
    ZoomYUpCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y Up"), addToStack) {}
    virtual ~ZoomYUpCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYDownCommand : public ZoomCommand
{
  public:
    ZoomYDownCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y Down"), addToStack) {}
    virtual ~ZoomYDownCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYOutCommand : public ZoomCommand
{
  public:
    ZoomYOutCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y Out"), addToStack) {}
    virtual ~ZoomYOutCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYInCommand : public ZoomCommand
{
  public:
    ZoomYInCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y In"), addToStack) {}
    virtual ~ZoomYInCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomNormalizeYToXCommand : public ZoomCommand
{
  public:
    ZoomNormalizeYToXCommand(PlotItem *item, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Normalize Y to X"), addToStack) {}
    virtual ~ZoomNormalizeYToXCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXLogCommand : public ZoomCommand
{
  public:
    ZoomXLogCommand(PlotItem *item, bool enableLog, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom X Log"), addToStack), _enableLog(enableLog) {}
    virtual ~ZoomXLogCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    bool _enableLog;
};

class KST_EXPORT ZoomYLogCommand : public ZoomCommand
{
  public:
    ZoomYLogCommand(PlotItem *item, bool enableLog, bool addToStack = true)
        : ZoomCommand(item, QObject::tr("Zoom Y Log"), addToStack), _enableLog(enableLog) {}
    virtual ~ZoomYLogCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    bool _enableLog;
};

}

#endif

// vim: ts=2 sw=2 et
