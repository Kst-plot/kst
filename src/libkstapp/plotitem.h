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

namespace Kst {

class PlotItem;

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
    enum MajorTickMode {
      Coarse = 2,
      Normal = 5,
      Fine = 10,
      VeryFine = 15
    };

    enum ZoomMode { Auto, AutoBorder, FixedExpression, SpikeInsensitive, MeanCentered };

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

    bool isBottomAxisVisible() const;
    void setBottomAxisVisible(bool visible);

    bool isLeftAxisVisible() const;
    void setLeftAxisVisible(bool visible);

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

    MajorTickMode xAxisMajorTickMode() const;
    void setXAxisMajorTickMode(MajorTickMode mode);

    MajorTickMode yAxisMajorTickMode() const;
    void setYAxisMajorTickMode(MajorTickMode mode);

    int xAxisMinorTickCount() const;
    void setXAxisMinorTickCount(const int count);

    int yAxisMinorTickCount() const;
    void setYAxisMinorTickCount(const int count);

    bool drawXAxisMajorTicks() const;
    void setDrawXAxisMajorTicks(const bool draw);

    bool drawXAxisMinorTicks() const;
    void setDrawXAxisMinorTicks(const bool draw);

    bool drawYAxisMajorTicks() const;
    void setDrawYAxisMajorTicks(const bool draw);

    bool drawYAxisMinorTicks() const;
    void setDrawYAxisMinorTicks(const bool draw);

    bool drawXAxisMajorGridLines() const;
    void setDrawXAxisMajorGridLines(const bool draw);

    bool drawXAxisMinorGridLines() const;
    void setDrawXAxisMinorGridLines(const bool draw);

    bool drawYAxisMajorGridLines() const;
    void setDrawYAxisMajorGridLines(const bool draw);

    bool drawYAxisMinorGridLines() const;
    void setDrawYAxisMinorGridLines(const bool draw);

    QColor xAxisMajorGridLineColor() const;
    void setXAxisMajorGridLineColor(const QColor &color);

    QColor xAxisMinorGridLineColor() const;
    void setXAxisMinorGridLineColor(const QColor &color);

    QColor yAxisMajorGridLineColor() const;
    void setYAxisMajorGridLineColor(const QColor &color);

    QColor yAxisMinorGridLineColor() const;
    void setYAxisMinorGridLineColor(const QColor &color);

    Qt::PenStyle xAxisMajorGridLineStyle() const;
    void setXAxisMajorGridLineStyle(const Qt::PenStyle style);

    Qt::PenStyle xAxisMinorGridLineStyle() const;
    void setXAxisMinorGridLineStyle(const Qt::PenStyle style);

    Qt::PenStyle yAxisMajorGridLineStyle() const;
    void setYAxisMajorGridLineStyle(const Qt::PenStyle style);

    Qt::PenStyle yAxisMinorGridLineStyle() const;
    void setYAxisMinorGridLineStyle(const Qt::PenStyle style);

    int xAxisSignificantDigits() const;
    void setXAxisSignificantDigits(const int digits);

    int yAxisSignificantDigits() const;
    void setYAxisSignificantDigits(const int digits);

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

    ZoomMode xAxisZoomMode() const;
    void setXAxisZoomMode(ZoomMode mode);

    ZoomMode yAxisZoomMode() const;
    void setYAxisZoomMode(ZoomMode mode);

    bool xAxisLog() const;
    void setXAxisLog(bool log);

    bool yAxisLog() const;
    void setYAxisLog(bool log);

    bool xAxisReversed() const;
    void setXAxisReversed(const bool enabled);

    bool yAxisReversed() const;
    void setYAxisReversed(const bool enabled);

    bool xAxisBaseOffset() const;
    void setXAxisBaseOffset(const bool enabled);

    bool yAxisBaseOffset() const;
    void setYAxisBaseOffset(const bool enabled);

    bool xAxisInterpret() const;
    void setXAxisInterpret(const bool enabled);

    bool yAxisInterpret() const;
    void setYAxisInterpret(const bool enabled);

    KstAxisDisplay xAxisDisplay() const;
    void setXAxisDisplay(const KstAxisDisplay display);

    KstAxisDisplay yAxisDisplay() const;
    void setYAxisDisplay(const KstAxisDisplay display);

    KstAxisInterpretation xAxisInterpretation() const;
    void setXAxisInterpretation(const KstAxisInterpretation interpret);

    KstAxisInterpretation yAxisInterpretation() const;
    void setYAxisInterpretation(const KstAxisInterpretation interpret);

    PlotMarkers xAxisPlotMarkers() { return _xAxisPlotMarkers; }
    void setXAxisPlotMarkers(const PlotMarkers &plotMarkers) { _xAxisPlotMarkers = plotMarkers; }

    PlotMarkers yAxisPlotMarkers() { return _yAxisPlotMarkers; }
    void setYAxisPlotMarkers(const PlotMarkers &plotMarkers) { _yAxisPlotMarkers = plotMarkers; }

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

  public Q_SLOTS:
    void zoomFixedExpression(const QRectF &projection);
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

    virtual void paintPlot(QPainter *painter,
                                     const QList<qreal> &xMajorTicks,
                                     const QList<qreal> &xMinorTicks,
                                     const QList<qreal> &yMajorTicks,
                                     const QList<qreal> &yMinorTicks);

    virtual void paintMajorGridLines(QPainter *painter,
                                     const QList<qreal> &xMajorTicks,
                                     const QList<qreal> &yMajorTicks);

     virtual void paintMinorGridLines(QPainter *painter,
                                     const QList<qreal> &xMinorTicks,
                                     const QList<qreal> &yMinorTicks);

    virtual void paintMajorTicks(QPainter *painter,
                                 const QList<qreal> &xMajorTicks,
                                 const QList<qreal> &yMajorTicks);

    virtual void paintMinorTicks(QPainter *painter,
                                 const QList<qreal> &xMinorTicks,
                                 const QList<qreal> &yMinorTicks);

    virtual void paintTickLabels(QPainter *painter,
                                      const QList<qreal> &xMajorTicks,
                                      const QList<qreal> &yMajorTicks,
                                      const QMap<qreal, QString> &xLabels,
                                      const QMap<qreal, QString> &yLabels);

    virtual void paintBottomTickLabels(QPainter *painter,
                                      const QList<qreal> &xMajorTicks,
                                      const QMap<qreal, QString> &xLabels);

    virtual void paintLeftTickLabels(QPainter *painter,
                                      const QList<qreal> &yMajorTicks,
                                      const QMap<qreal, QString> &yLabels);

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

    QString interpretLabel(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double base, double lastValue);
    double convertTimeValueToJD(KstAxisInterpretation axisInterpretation, double valueIn);
    QString convertJDToDateString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double dJD);
    double convertTimeDiffValueToDays(KstAxisInterpretation axisInterpretation, double offsetIn);
    double interpretOffset(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double base, double value);

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

    void computeTicks(QList<qreal> *xMajorTicks, QList<qreal> *xMinorTicks, QList<qreal> *yMajorTicks, QList<qreal> *yMinorTicks, QMap<qreal, QString> *xLabels, QMap<qreal, QString> *yLabels);
    qreal computedMajorTickSpacing(Qt::Orientation orientation) const;
    void computeLogTicks(QList<qreal> *MajorTicks, QList<qreal> *MinorTicks, QMap<qreal, QString> *Labels, qreal min, qreal max, MajorTickMode tickMode);

    QSizeF calculateXTickLabelBound(QPainter *painter, const QList<qreal> &xMajorTicks);
    QSizeF calculateYTickLabelBound(QPainter *painter, const QList<qreal> &yMajorTicks);

    ZoomState currentZoomState();
    void setCurrentZoomState(ZoomState zoomState);

  private:
    QHash<PlotRenderItem::RenderType, PlotRenderItem*> _renderers;
    bool _isTiedZoom;
    ZoomMode _xAxisZoomMode;
    ZoomMode _yAxisZoomMode;
    bool _isLeftLabelVisible;
    bool _isBottomLabelVisible;
    bool _isRightLabelVisible;
    bool _isTopLabelVisible;
    bool _isBottomAxisVisible;
    bool _isLeftAxisVisible;
    qreal _calculatedLeftLabelMargin;
    qreal _calculatedRightLabelMargin;
    qreal _calculatedTopLabelMargin;
    qreal _calculatedBottomLabelMargin;
    qreal _calculatedLabelMarginWidth;
    qreal _calculatedLabelMarginHeight;
    qreal _calculatedAxisMarginWidth;
    qreal _calculatedAxisMarginHeight;

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

    bool _xAxisLog;
    bool _yAxisLog;
    bool _xAxisReversed;
    bool _yAxisReversed;
    bool _xAxisBaseOffset;
    bool _yAxisBaseOffset;
    bool _xAxisInterpret;
    bool _yAxisInterpret;
    KstAxisDisplay _xAxisDisplay;
    KstAxisDisplay _yAxisDisplay;
    KstAxisInterpretation _xAxisInterpretation;
    KstAxisInterpretation _yAxisInterpretation;

    MajorTickMode _xAxisMajorTickMode;
    MajorTickMode _yAxisMajorTickMode;
    int _xAxisMinorTickCount;
    int _yAxisMinorTickCount;

    int _xAxisSignificantDigits;
    int _yAxisSignificantDigits;

    bool _drawXAxisMajorTicks;
    bool _drawXAxisMinorTicks;
    bool _drawYAxisMajorTicks;
    bool _drawYAxisMinorTicks;
    bool _drawXAxisMajorGridLines;
    bool _drawXAxisMinorGridLines;
    bool _drawYAxisMajorGridLines;
    bool _drawYAxisMinorGridLines;

    QColor _xAxisMajorGridLineColor;
    QColor _xAxisMinorGridLineColor;
    QColor _yAxisMajorGridLineColor;
    QColor _yAxisMinorGridLineColor;

    Qt::PenStyle _xAxisMajorGridLineStyle;
    Qt::PenStyle _xAxisMinorGridLineStyle;
    Qt::PenStyle _yAxisMajorGridLineStyle;
    Qt::PenStyle _yAxisMinorGridLineStyle;

    PlotMarkers _xAxisPlotMarkers;
    PlotMarkers _yAxisPlotMarkers;

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

}

#endif

// vim: ts=2 sw=2 et
