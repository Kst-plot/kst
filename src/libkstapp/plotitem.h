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
#include "namedobject.h"

#include <QHash>

#include "relation.h"
#include "plotrenderitem.h"
#include "plotiteminterface.h"
#include "plotdefines.h"
#include "plotmarkers.h"

#include "plotaxis.h"
#include "legenditem.h"
#include "curveplacement.h"

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


class PlotItem : public ViewItem, public PlotItemInterface, public NamedObject
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

    bool isInSharedAxisBox() const;
    void setInSharedAxisBox(bool inSharedBox);

    void setSharedAxisBox(ViewItem* parent);

    qreal leftMarginSize() const;
    void setLeftPadding(const qreal);
    qreal bottomMarginSize() const;
    void setBottomPadding(const qreal);
    qreal rightMarginSize() const;
    void setRightPadding(const qreal);
    qreal topMarginSize() const;
    void setTopPadding(const qreal);

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

    bool leftFontUseGlobal() const {return _leftFontUseGlobal;}
    void setLeftFontUseGlobal(const bool use_global) {_leftFontUseGlobal = use_global;}

    bool rightFontUseGlobal() const {return _rightFontUseGlobal;}
    void setRightFontUseGlobal(const bool use_global) {_rightFontUseGlobal = use_global;}

    bool topFontUseGlobal() const {return _topFontUseGlobal;}
    void setTopFontUseGlobal(const bool use_global) {_topFontUseGlobal = use_global;}

    bool bottomFontUseGlobal() const {return _bottomFontUseGlobal;}
    void setBottomFontUseGlobal(const bool use_global) {_bottomFontUseGlobal = use_global;}

    bool numberFontUseGlobal() const {return _numberFontUseGlobal;}
    void setNumberFontUseGlobal(const bool use_global) {_numberFontUseGlobal = use_global;}

    QFont globalFont() const;
    void setGlobalFont(const QFont &font);

    QFont topLabelFont() const;
    void setTopLabelFont(const QFont &font);

    QFont leftLabelFont() const;
    void setLeftLabelFont(const QFont &font);

    QFont rightLabelFont() const;
    void setRightLabelFont(const QFont &font);

    QFont bottomLabelFont() const;
    void setBottomLabelFont(const QFont &font);

    QFont numberLabelFont() const;
    void setNumberLabelFont(const QFont &font);

    qreal globalFontScale() const;
    void setGlobalFontScale(const qreal scale);

    qreal bottomLabelFontScale() const;
    void setBottomLabelFontScale(const qreal scale);

    qreal leftLabelFontScale() const;
    void setLeftLabelFontScale(const qreal scale);

    qreal topLabelFontScale() const;
    void setTopLabelFontScale(const qreal scale);

    qreal rightLabelFontScale() const;
    void setRightLabelFontScale(const qreal scale);

    qreal numberLabelFontScale() const;
    void setNumberLabelFontScale(const qreal scale);

    QColor globalFontColor() const;
    void setGlobalFontColor(const QColor &color);

    QColor bottomLabelFontColor() const;
    void setBottomLabelFontColor(const QColor &color);

    QColor topLabelFontColor() const;
    void setTopLabelFontColor(const QColor &color);

    QColor leftLabelFontColor() const;
    void setLeftLabelFontColor(const QColor &color);

    QColor rightLabelFontColor() const;
    void setRightLabelFontColor(const QColor &color);

    QColor numberLabelFontColor() const;
    void setNumberLabelFontColor(const QColor &color);

    bool showLegend() const;
    void setShowLegend(const bool show);

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
    virtual bool tryShortcut(const QString &keySequence);

    virtual void updateObject();

    LegendItem* legend();

    virtual QString descriptionTip() const; // description for tooltips
    ZoomState currentZoomState();

  protected:
    virtual QString _automaticDescriptiveName() const;

  Q_SIGNALS:
    void marginsChanged();
    void updatePlotRect();
    void updateAxes();

  public Q_SLOTS:
    void zoomFixedExpression(const QRectF &projection);
    void zoomXRange(const QRectF &projection);
    void zoomYRange(const QRectF &projection);
    void zoomMaximum();
    void zoomGeneral(ZoomState &zoomstate);
    void zoomMaxSpikeInsensitive();
    void zoomPrevious();
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
    void marginsUpdated();

    void showFilterDialog(QAction*);
    void showFitDialog(QAction*);

  private:
    void createActions();
    void createZoomMenu();
    void createFilterMenu();
    void createFitMenu();

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
    QFont calculatedNumberLabelFont();

    qreal calculatedAxisMarginWidth() const;
    void setCalculatedAxisMarginWidth(qreal marginWidth);

    qreal calculatedAxisMarginHeight() const;
    void setCalculatedAxisMarginHeight(qreal marginHeight);

    QSizeF calculateBottomTickLabelBound(QPainter *painter);
    QSizeF calculateLeftTickLabelBound(QPainter *painter);

    void setCurrentZoomState(ZoomState zoomState);

  private:
    QHash<PlotRenderItem::RenderType, PlotRenderItem*> _renderers;
    bool _isTiedZoom;
    bool _isInSharedAxisBox;
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

    qreal _leftPadding;
    qreal _bottomPadding;
    qreal _rightPadding;
    qreal _topPadding;

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

    QFont _globalFont;
    QFont _leftLabelFont;
    QFont _bottomLabelFont;
    QFont _topLabelFont;
    QFont _rightLabelFont;
    QFont _numberLabelFont;

    qreal _globalFontScale;
    qreal _leftLabelFontScale;
    qreal _bottomLabelFontScale;
    qreal _topLabelFontScale;
    qreal _rightLabelFontScale;
    qreal _numberLabelFontScale;

    QColor _globalFontColor;
    QColor _leftLabelFontColor;
    QColor _bottomLabelFontColor;
    QColor _topLabelFontColor;
    QColor _rightLabelFontColor;
    QColor _numberLabelFontColor;

    bool _leftFontUseGlobal;
    bool _rightFontUseGlobal;
    bool _topFontUseGlobal;
    bool _bottomFontUseGlobal;
    bool _numberFontUseGlobal;

    bool _showLegend;

    LegendItem* _legend;

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

    QMenu *_filterMenu;
    QAction *_filterAction;
    QMenu *_fitMenu;
    QAction *_fitAction;

    QUndoStack *_undoStack;

    friend class ZoomCommand;
    friend class ZoomMaximumCommand;
    friend class ZoomGeneralCommand;
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
    CreatePlotForCurve()
      : CreateCommand(QObject::tr("Create Plot For Curve")) {}
    CreatePlotForCurve(View *view)
      : CreateCommand(view, QObject::tr("Create Plot For Curve")) {}
    virtual ~CreatePlotForCurve() {}
    virtual void createItem();
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
    ZoomCommand(PlotItem *item, const QString &text);
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
    ZoomFixedExpressionCommand(PlotItem *item, const QRectF &fixed)
        : ZoomCommand(item, QObject::tr("Zoom Fixed Expression")), _fixed(fixed) {}
    virtual ~ZoomFixedExpressionCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    QRectF _fixed;
};


class KST_EXPORT ZoomGeneralCommand : public ZoomCommand
{
  public:
    ZoomGeneralCommand(PlotItem *item, const ZoomState &zoomstate)
        : ZoomCommand(item, QObject::tr("Zoom Fixed Expression")), _zoomstate(zoomstate) {}
    virtual ~ZoomGeneralCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    ZoomState _zoomstate;
};


class KST_EXPORT ZoomXRangeCommand : public ZoomCommand
{
  public:
    ZoomXRangeCommand(PlotItem *item, const QRectF &fixed)
        : ZoomCommand(item, QObject::tr("Zoom X Range Expression")), _fixed(fixed) {}
    virtual ~ZoomXRangeCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    QRectF _fixed;
};


class KST_EXPORT ZoomYRangeCommand : public ZoomCommand
{
  public:
    ZoomYRangeCommand(PlotItem *item, const QRectF &fixed)
        : ZoomCommand(item, QObject::tr("Zoom X Range Expression")), _fixed(fixed) {}
    virtual ~ZoomYRangeCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    QRectF _fixed;
};


class KST_EXPORT ZoomMaximumCommand : public ZoomCommand
{
  public:
    ZoomMaximumCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Maximum")) {}
    virtual ~ZoomMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomMaxSpikeInsensitiveCommand : public ZoomCommand
{
  public:
    ZoomMaxSpikeInsensitiveCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Max Spike Insensitive")) {}
    virtual ~ZoomMaxSpikeInsensitiveCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYMeanCenteredCommand : public ZoomCommand
{
  public:
    ZoomYMeanCenteredCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Mean Centered")) {}
    virtual ~ZoomYMeanCenteredCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXMaximumCommand : public ZoomCommand
{
  public:
    ZoomXMaximumCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom X Maximum")) {}
    virtual ~ZoomXMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXRightCommand : public ZoomCommand
{
  public:
    ZoomXRightCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Scroll X Right")) {}
    virtual ~ZoomXRightCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXLeftCommand : public ZoomCommand
{
  public:
    ZoomXLeftCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Scroll X Left")) {}
    virtual ~ZoomXLeftCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXOutCommand : public ZoomCommand
{
  public:
    ZoomXOutCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom X Out")) {}
    virtual ~ZoomXOutCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXInCommand : public ZoomCommand
{
  public:
    ZoomXInCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom X In")) {}
    virtual ~ZoomXInCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomNormalizeXToYCommand : public ZoomCommand
{
  public:
    ZoomNormalizeXToYCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Normalize X to Y")) {}
    virtual ~ZoomNormalizeXToYCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYLocalMaximumCommand : public ZoomCommand
{
  public:
    ZoomYLocalMaximumCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Local Maximum")) {}
    virtual ~ZoomYLocalMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYMaximumCommand : public ZoomCommand
{
  public:
    ZoomYMaximumCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Maximum")) {}
    virtual ~ZoomYMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYUpCommand : public ZoomCommand
{
  public:
    ZoomYUpCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Up")) {}
    virtual ~ZoomYUpCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYDownCommand : public ZoomCommand
{
  public:
    ZoomYDownCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Down")) {}
    virtual ~ZoomYDownCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYOutCommand : public ZoomCommand
{
  public:
    ZoomYOutCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Out")) {}
    virtual ~ZoomYOutCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomYInCommand : public ZoomCommand
{
  public:
    ZoomYInCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y In")) {}
    virtual ~ZoomYInCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomNormalizeYToXCommand : public ZoomCommand
{
  public:
    ZoomNormalizeYToXCommand(PlotItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Normalize Y to X")) {}
    virtual ~ZoomNormalizeYToXCommand() {}

    virtual void applyZoomTo(PlotItem *item);
};

class KST_EXPORT ZoomXLogCommand : public ZoomCommand
{
  public:
    ZoomXLogCommand(PlotItem *item, bool enableLog)
        : ZoomCommand(item, QObject::tr("Zoom X Log")), _enableLog(enableLog) {}
    virtual ~ZoomXLogCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    bool _enableLog;
};

class KST_EXPORT ZoomYLogCommand : public ZoomCommand
{
  public:
    ZoomYLogCommand(PlotItem *item, bool enableLog)
        : ZoomCommand(item, QObject::tr("Zoom Y Log")), _enableLog(enableLog) {}
    virtual ~ZoomYLogCommand() {}

    virtual void applyZoomTo(PlotItem *item);

  private:
    bool _enableLog;
};

}

#endif

// vim: ts=2 sw=2 et
