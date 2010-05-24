
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
#include "namedobject.h"

#include "plotaxis.h"
#include "legenditem.h"
#include "curveplacement.h"
#include "labelrenderer.h"

namespace Kst {

class PlotItem;
class PlotAxis;
class SharedAxisBoxItem;

struct ZoomState {
  QPointer<PlotItem> item;
  QRectF projectionRect;
  int xAxisZoomMode;
  int yAxisZoomMode;
  bool isXAxisLog;
  bool isYAxisLog;
};


struct CachedLabel {
  CachedLabel() { valid = false; dirty = true; parsed = 0; rc = 0; };
  ~CachedLabel() { delete parsed; delete rc; };

  bool valid;
  bool dirty;
  Label::Parsed *parsed;
  Label::RenderContext *rc;
  QTransform transform;
};


struct CachedPlotLabel {
  CachedPlotLabel() { baseLabel = false; };

  bool baseLabel;
  QRectF bound;
  QString value;
};


class PlotLabel : public QObject {
  Q_OBJECT
  public:
    PlotLabel(PlotItem *plotItem);

    bool isVisible() const;
    void setVisible(bool visible);

    bool isAuto() {return _isAuto;}
    void setIsAuto(bool is_auto) { _isAuto = is_auto;}

    QString text() const;
    void setText(const QString &label);

    bool fontUseGlobal() const;
    void setFontUseGlobal(const bool use_global);

    QFont font() const;
    void setFont(const QFont &font);

    qreal fontScale() const;
    void setFontScale(const qreal scale);

    QColor fontColor() const;
    void setFontColor(const QColor &color);

    QFont calculatedFont();

    void setDetails(const QString &label, bool is_auto, const bool use_global, const QFont &font, const qreal scale, const QColor &color);
    void saveInPlot(QXmlStreamWriter &xml, QString labelId);
    bool configureFromXml(QXmlStreamReader &xml, ObjectStore *store);

    void saveAsDialogDefaults(const QString &group) const;

  Q_SIGNALS:
    void labelChanged();

  public:
    PlotItem* _plotItem;
    bool _visible;
    QString _text;
    QFont _font;
    qreal _fontScale;
    QColor _fontColor;
    bool _fontUseGlobal;
    bool _isAuto;
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
    QRectF plotAxisRect();

    /* This is the rectangle of the PlotRenderItem's and includes the actual curves. */
    QRectF plotRect();

    QRectF projectionRect() const;
    void setProjectionRect(const QRectF &rect, bool forceAxisUpdate = false);
    QRectF computedProjectionRect();
    void computedRelationalMax(qreal &minimum, qreal &maximum);
    void computeBorder(Qt::Orientation orientation, qreal &minimum, qreal &maximum) const;

    PlotAxis* xAxis() { return _xAxis; }
    PlotAxis* yAxis() { return _yAxis; }

    virtual void setTiedZoom(bool tiedXZoom, bool tiedYZoom, bool checkAllTied = true);

    bool isInSharedAxisBox() const;
    void setInSharedAxisBox(bool inSharedBox);

    SharedAxisBoxItem* sharedAxisBox() const;
    void setSharedAxisBox(SharedAxisBoxItem* parent);

    void setPadding(const qreal, const qreal, const qreal, const qreal);
    void setLeftPadding(const qreal);
    void setBottomPadding(const qreal);
    void setRightPadding(const qreal);
    void setTopPadding(const qreal);

    qreal leftMarginSize() const;
    qreal bottomMarginSize() const;
    qreal rightMarginSize() const;
    qreal topMarginSize() const;

    QString leftLabel() const;
    QString bottomLabel() const;
    QString rightLabel() const;
    QString topLabel() const;

    QString autoLeftLabel() const;
    QString autoBottomLabel() const;
    QString autoRightLabel() const;
    QString autoTopLabel() const;

    PlotLabel* leftLabelDetails() const { return _leftLabelDetails; }
    PlotLabel* rightLabelDetails() const { return _rightLabelDetails; }
    PlotLabel* topLabelDetails() const { return _topLabelDetails; }
    PlotLabel* bottomLabelDetails() const { return _bottomLabelDetails; }
    PlotLabel* numberLabelDetails() const { return _numberLabelDetails; }

    void setTopSuppressed(bool suppressed);
    void setBottomSuppressed(bool suppressed);
    void setLeftSuppressed(bool suppressed);
    void setRightSuppressed(bool suppressed);

    void setLabelsVisible(bool visible);

    qreal axisMarginWidth() const;
    qreal axisMarginHeight() const;

    QString titleOverride() const;
    void setTitleOverride(const QString &label);

    QFont globalFont() const;
    void setGlobalFont(const QFont &font);

    qreal globalFontScale() const;
    void setGlobalFontScale(const qreal scale);

    QColor globalFontColor() const;
    void setGlobalFontColor(const QColor &color);

    bool showLegend() const;
    void setShowLegend(const bool show);

    qreal mapXToPlot(const qreal &x);
    qreal mapYToPlot(const qreal &y);
    QPointF mapToPlot(const QPointF &point);

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

    virtual bool handleChangedInputs(qint64 serial);

    LegendItem* legend();

    virtual QString descriptionTip() const; // description for tooltips
    ZoomState currentZoomState();

    void setAllowUpdates(bool allowed);

    virtual void setItemPen(const QPen & pen) { ViewItem::setPen(pen); setPlotPixmapDirty();}

    void scaleAxisLabels(qreal scaleFactor);
    void resetScaleAxisLabels();

    bool isUseAxisScale() const;
    void setUseAxisScale(bool useScale);

    virtual QPainterPath checkBox() const;
    virtual QPainterPath tiedZoomCheck() const;
    virtual bool supportsTiedZoom() const;

    void saveAsDialogDefaults() const;
    void applyDefaults();

    bool maskedByMaximization() {return (parentView()->childMaximized() && !_plotMaximized);}
  protected:
    virtual QString _automaticDescriptiveName() const;
    virtual void _initializeShortName();

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect);

  Q_SIGNALS:
    void updatePlotRect();
    void updateAxes();
    void triggerRedraw();

  public Q_SLOTS:
    void zoomFixedExpression(const QRectF &projection, bool force = false);
    void zoomXRange(const QRectF &projection, bool force = false);
    void zoomYRange(const QRectF &projection, bool force = false);
    void zoomMaximum(bool force = false);
    void zoomMaxSpikeInsensitive(bool force = false);
    void zoomPrevious();
    void adjustImageColorScale();

    void zoomTied();
    void zoomXTied();
    void zoomYTied();

    //void zoomXMeanCentered(bool force = false);
    void zoomXMaximum(bool force = false);
    void zoomXNoSpike(bool force = false);
    void zoomXAutoBorder(bool force = false);
    void zoomXRight(bool force = false);
    void zoomXLeft(bool force = false);
    void zoomXOut(bool force = false);
    void zoomXIn(bool force = false);
    void zoomNormalizeXtoY(bool force = false);
    void zoomLogX(bool force = false, bool autoLog = true, bool enableLog = false);

    void zoomMeanCentered(bool force = false);
    void zoomYMeanCentered(qreal dY, bool force = false);
    void zoomXMeanCentered(qreal dX, bool force = false);
    void zoomYLocalMaximum(bool force = false);
    void zoomYMaximum(bool force = false);
    void zoomYNoSpike(bool force = false);
    void zoomYAutoBorder(bool force = false);
    void zoomYUp(bool force = false);
    void zoomYDown(bool force = false);
    void zoomYOut(bool force = false);
    void zoomYIn(bool force = false);
    void zoomNormalizeYtoX(bool force = false);
    void zoomLogY(bool force = false, bool autoLog = true, bool enableLog = false);

    void setPlotBordersDirty(bool dirty = true);

    virtual void edit();
    void plotMaximize();
    void redrawPlot();
    void setPlotRectsDirty();

    void showFilterDialog(QAction*);
    void showFitDialog(QAction*);

    void setLeftLabelDirty() { _leftLabel.dirty = true; setPlotPixmapDirty(); }
    void setRightLabelDirty() { _rightLabel.dirty = true; setPlotPixmapDirty(); }
    void setTopLabelDirty() { _topLabel.dirty = true; setPlotPixmapDirty(); }
    void setBottomLabelDirty() { _bottomLabel.dirty = true; setPlotPixmapDirty(); }
    void setLabelsDirty() { _leftLabel.dirty = true; _rightLabel.dirty = true; _topLabel.dirty = true; _bottomLabel.dirty = true; setPlotPixmapDirty(); }

    void setPlotPixmapDirty(bool dirty = true) {_plotPixmapDirty = dirty; }
    void setAxisLabelsDirty(bool dirty = true) { _axisLabelsDirty = dirty; }

  private:
    void createActions();
    void createZoomMenu();
    void createFilterMenu();
    void createFitMenu();
    void createSharedAxisBoxMenu();

    void resetSelectionRect();

    void updatePlotPixmap();
    virtual void paintPixmap(QPainter *painter);

    void updateXAxisLines();
    void updateYAxisLines();

    void updateXAxisLabels(QPainter* painter);
    void updateYAxisLabels(QPainter* painter);

    virtual void paintPlot(QPainter *painter, bool xUpdated, bool yUpdated);

    virtual void paintMajorGridLines(QPainter *painter);
    virtual void paintMinorGridLines(QPainter *painter);
    virtual void paintMajorTicks(QPainter *painter);
    virtual void paintMinorTicks(QPainter *painter);

    virtual void paintTickLabels(QPainter *painter);
    virtual void paintBottomTickLabels(QPainter *painter);
    virtual void paintLeftTickLabels(QPainter *painter);

    virtual void paintPlotMarkers(QPainter *painter);

    void calculateBorders(QPainter *painter);

    qreal labelMarginWidth() const;
    qreal labelMarginHeight() const;

    qreal leftLabelMargin() const;
    qreal rightLabelMargin() const;
    qreal topLabelMargin() const;
    qreal bottomLabelMargin() const;

    QRectF topLabelRect() const;
    QRectF bottomLabelRect() const;
    QRectF leftLabelRect() const;
    QRectF rightLabelRect() const;

    void calculatePlotRects();

    void calculateLeftLabelMargin(QPainter *painter);
    void calculateBottomLabelMargin(QPainter *painter);
    void calculateRightLabelMargin(QPainter *painter);
    void calculateTopLabelMargin(QPainter *painter);
    void calculateMargins();

    void generateLeftLabel();
    void generateBottomLabel();
    void generateRightLabel();
    void generateTopLabel();

    void paintLeftLabel(QPainter *painter);
    void paintBottomLabel(QPainter *painter);
    void paintRightLabel(QPainter *painter);
    void paintTopLabel(QPainter *painter);

    void calculateBottomTickLabelBound(QPainter *painter);
    void calculateLeftTickLabelBound(QPainter *painter);

    void setCurrentZoomState(ZoomState zoomState);

  private:
    QHash<PlotRenderItem::RenderType, PlotRenderItem*> _renderers;
    bool _isInSharedAxisBox;

    PlotLabel* _leftLabelDetails;
    PlotLabel* _rightLabelDetails;
    PlotLabel* _topLabelDetails;
    PlotLabel* _bottomLabelDetails;
    PlotLabel* _numberLabelDetails;

    bool _plotRectsDirty;
    QRectF _calculatedPlotRect;
    QRectF _calculatedPlotAxisRect;

    qreal _calculatedLeftLabelMargin;
    qreal _calculatedRightLabelMargin;
    qreal _calculatedTopLabelMargin;
    qreal _calculatedTopLabelHeight;
    qreal _calculatedBottomLabelMargin;
    qreal _calculatedLabelMarginWidth;
    qreal _calculatedLabelMarginHeight;

    qreal _calculatedAxisMarginWidth;
    qreal _calculatedAxisMarginHeight;
    qreal _calculatedAxisMarginVLead; 
    qreal _calculatedAxisMarginHLead; 
    qreal _calculatedAxisMarginROverflow;
    qreal _calculatedAxisMarginTOverflow;

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

    QFont _globalFont;
    qreal _globalFontScale;
    QColor _globalFontColor;

    qreal _numberAxisLabelScaleFactor;
    bool _useNumberAxisLabelScale;

    bool _showLegend;

    bool _plotMaximized;
    QPointF _plotMaximizedSourcePosition;
    QRectF _plotMaximizedSourceRect;
    QRectF _plotMaximizedSourceParentRect;
    qreal _plotMaximizedSourceZValue;
    ViewItem* _plotMaximizedSourceParent;

    bool _allowUpdates;

    LegendItem* _legend;

    QMenu *_zoomMenu;
    QAction *_zoomMaximum;
    QAction *_zoomMaxSpikeInsensitive;
    QAction *_zoomPrevious;
    QAction *_zoomTied;
    QAction *_zoomXTied;
    QAction *_zoomYTied;
    QAction *_zoomMeanCentered;
    QAction *_zoomXMaximum;
    QAction *_zoomXAutoBorder;
    QAction *_zoomXNoSpike;
    QAction *_zoomXRight;
    QAction *_zoomXLeft;
    QAction *_zoomXOut;
    QAction *_zoomXIn;
    QAction *_zoomNormalizeXtoY;
    QAction *_zoomLogX;
    QAction *_zoomYLocalMaximum;
    QAction *_zoomYMaximum;
    QAction *_zoomYAutoBorder;
    QAction *_zoomYNoSpike;
    QAction *_zoomYUp;
    QAction *_zoomYDown;
    QAction *_zoomYOut;
    QAction *_zoomYIn;
    QAction *_zoomNormalizeYtoX;
    QAction *_zoomLogY;
    QAction *_plotMaximize;
    QAction *_adjustImageColorscale;

    QMenu *_filterMenu;
    QAction *_filterAction;
    QMenu *_fitMenu;
    QAction *_fitAction;
    QMenu *_sharedAxisBoxMenu;
    QAction *_shareBoxShareX;
    QAction *_shareBoxShareY;

    SharedAxisBoxItem * _sharedBox;

    bool _axisLabelsDirty;
    CachedLabel _leftLabel;
    CachedLabel _rightLabel;
    CachedLabel _topLabel;
    CachedLabel _bottomLabel;

    QVector<QLineF> _xMajorGridLines;
    QVector<QLineF> _yMajorGridLines;
    QVector<QLineF> _xMinorGridLines;
    QVector<QLineF> _yMinorGridLines;
    QVector<QLineF> _xMajorTickLines;
    QVector<QLineF> _yMajorTickLines;
    QVector<QLineF> _xMinorTickLines;
    QVector<QLineF> _yMinorTickLines;

    QVector<QLineF> _xPlotMarkerLines;
    QVector<QLineF> _yPlotMarkerLines;

    QVector<CachedPlotLabel> _xPlotLabels;
    QVector<CachedPlotLabel> _yPlotLabels;

    bool _plotPixmapDirty;
    QPixmap _plotPixmap;

    QUndoStack *_undoStack;

    int _i_per; // index for image smart ranges

    friend class ZoomCommand;
    friend class ZoomMaximumCommand;
    friend class ZoomGeneralCommand;
    friend class ZoomMaxSpikeInsensitiveCommand;
    friend class ZoomMeanCenteredCommand;
    friend class ZoomXMeanCenteredCommand;
    friend class ZoomYMeanCenteredCommand;
    friend class ZoomXMaximumCommand;
    friend class ZoomYLocalMaximumCommand;
    friend class ZoomYMaximumCommand;
    friend class ViewGridLayout;
};

class CreatePlotCommand : public CreateCommand
{
  public:
    CreatePlotCommand() : CreateCommand(QObject::tr("Create Plot")) {}
    CreatePlotCommand(View *view) : CreateCommand(view, QObject::tr("Create Plot")) {}
    virtual ~CreatePlotCommand() {}
    virtual void createItem();
};

class CreatePlotForCurve : public CreateCommand
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


class ZoomCommand : public ViewItemCommand
{
  public:
    ZoomCommand(PlotItem *item, const QString &text, bool forced = false);
    virtual ~ZoomCommand();

    virtual void undo();
    virtual void redo();

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true) = 0;
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true) = 0;

  private:
    QList<ZoomState> _originalStates;
    QList<ViewItem*> _viewItems;
    QList<PlotItem*> _sharedPlotItems;
    PlotItem* _plotItem;
};


class ZoomFixedExpressionCommand : public ZoomCommand
{
  public:
    ZoomFixedExpressionCommand(PlotItem *item, const QRectF &fixed, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Fixed Expression"), forced), _fixed(fixed) {}
    virtual ~ZoomFixedExpressionCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);

  private:
    QRectF _fixed;
};


class ZoomXRangeCommand : public ZoomCommand
{
  public:
    ZoomXRangeCommand(PlotItem *item, const QRectF &fixed, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X Range Expression"), forced), _fixed(fixed) {}
    virtual ~ZoomXRangeCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
    
  private:
    QRectF _fixed;
};


class ZoomYRangeCommand : public ZoomCommand
{
  public:
    ZoomYRangeCommand(PlotItem *item, const QRectF &fixed, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X Range Expression"), forced), _fixed(fixed) {}
    virtual ~ZoomYRangeCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
    
  private:
    QRectF _fixed;
};


class ZoomMaximumCommand : public ZoomCommand
{
  public:
    ZoomMaximumCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Maximum"), forced) {}
    virtual ~ZoomMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomMaxSpikeInsensitiveCommand : public ZoomCommand
{
  public:
    ZoomMaxSpikeInsensitiveCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Max Spike Insensitive"), forced) {}
    virtual ~ZoomMaxSpikeInsensitiveCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomMeanCenteredCommand : public ZoomCommand
{
  public:
    ZoomMeanCenteredCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Mean Centered"), forced) {}
    virtual ~ZoomMeanCenteredCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};


class ZoomXMeanCenteredCommand : public ZoomCommand
{
  public:
    ZoomXMeanCenteredCommand(PlotItem *item, qreal dX, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X Mean Centered"), forced), _dX(dX)  {}
    virtual ~ZoomXMeanCenteredCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
  private:
    qreal _dX;
};


class ZoomYMeanCenteredCommand : public ZoomCommand
{
  public:
    ZoomYMeanCenteredCommand(PlotItem *item, qreal dY, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Mean Centered"), forced), _dY(dY) {}
    virtual ~ZoomYMeanCenteredCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
  private:
    qreal _dY;
};


class ZoomXMaximumCommand : public ZoomCommand
{
  public:
    ZoomXMaximumCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X Maximum"), forced) {}
    virtual ~ZoomXMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomXAutoBorderCommand : public ZoomCommand
{
  public:
    ZoomXAutoBorderCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X Auto Border"), forced) {}
    virtual ~ZoomXAutoBorderCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomXNoSpikeCommand : public ZoomCommand
{
  public:
    ZoomXNoSpikeCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X No Spike"), forced) {}
    virtual ~ZoomXNoSpikeCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomXRightCommand : public ZoomCommand
{
  public:
    ZoomXRightCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Scroll X Right"), forced) {}
    virtual ~ZoomXRightCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomXLeftCommand : public ZoomCommand
{
  public:
    ZoomXLeftCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Scroll X Left"), forced) {}
    virtual ~ZoomXLeftCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomXOutCommand : public ZoomCommand
{
  public:
    ZoomXOutCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X Out"), forced) {}
    virtual ~ZoomXOutCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomXInCommand : public ZoomCommand
{
  public:
    ZoomXInCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X In"), forced) {}
    virtual ~ZoomXInCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomNormalizeXToYCommand : public ZoomCommand
{
  public:
    ZoomNormalizeXToYCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Normalize X to Y"), forced) {}
    virtual ~ZoomNormalizeXToYCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYLocalMaximumCommand : public ZoomCommand
{
  public:
    ZoomYLocalMaximumCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Local Maximum"), forced) {}
    virtual ~ZoomYLocalMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYMaximumCommand : public ZoomCommand
{
  public:
    ZoomYMaximumCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Maximum"), forced) {}
    virtual ~ZoomYMaximumCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYAutoBorderCommand : public ZoomCommand
{
  public:
    ZoomYAutoBorderCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Auto Border"), forced) {}
    virtual ~ZoomYAutoBorderCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYNoSpikeCommand : public ZoomCommand
{
  public:
    ZoomYNoSpikeCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y No Spike"), forced) {}
    virtual ~ZoomYNoSpikeCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYUpCommand : public ZoomCommand
{
  public:
    ZoomYUpCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Up"), forced) {}
    virtual ~ZoomYUpCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYDownCommand : public ZoomCommand
{
  public:
    ZoomYDownCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Down"), forced) {}
    virtual ~ZoomYDownCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYOutCommand : public ZoomCommand
{
  public:
    ZoomYOutCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Out"), forced) {}
    virtual ~ZoomYOutCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomYInCommand : public ZoomCommand
{
  public:
    ZoomYInCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y In"), forced) {}
    virtual ~ZoomYInCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomNormalizeYToXCommand : public ZoomCommand
{
  public:
    ZoomNormalizeYToXCommand(PlotItem *item, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Normalize Y to X"), forced) {}
    virtual ~ZoomNormalizeYToXCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);
};

class ZoomXLogCommand : public ZoomCommand
{
  public:
    ZoomXLogCommand(PlotItem *item, bool enableLog, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom X Log"), forced), _enableLog(enableLog) {}
    virtual ~ZoomXLogCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);

  private:
    bool _enableLog;
};

class ZoomYLogCommand : public ZoomCommand
{
  public:
    ZoomYLogCommand(PlotItem *item, bool enableLog, bool forced = false)
        : ZoomCommand(item, QObject::tr("Zoom Y Log"), forced), _enableLog(enableLog) {}
    virtual ~ZoomYLogCommand() {}

    virtual void applyZoomTo(PlotItem *item, bool applyX = true, bool applyY = true);
    virtual void applyZoomTo(ViewItem *item, bool applyX = true, bool applyY = true);

  private:
    bool _enableLog;
};

}

#endif

// vim: ts=2 sw=2 et
