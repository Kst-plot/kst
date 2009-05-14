
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
#include "sharedaxisboxitem.h"
#include "labelrenderer.h"

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

    virtual void setTiedZoom(bool tiedZoom, bool checkAllTied = true);

    bool isInSharedAxisBox() const;
    void setInSharedAxisBox(bool inSharedBox);

    SharedAxisBoxItem* sharedAxisBox();
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

    PlotLabel* leftLabelDetails() const { return _leftLabelDetails; };
    PlotLabel* rightLabelDetails() const { return _rightLabelDetails; };
    PlotLabel* topLabelDetails() const { return _topLabelDetails; };
    PlotLabel* bottomLabelDetails() const { return _bottomLabelDetails; };
    PlotLabel* numberLabelDetails() const { return _numberLabelDetails; };

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

    virtual void updateObject();

    LegendItem* legend();

    virtual QString descriptionTip() const; // description for tooltips
    ZoomState currentZoomState();

    void setAllowUpdates(bool allowed);

  protected:
    virtual QString _automaticDescriptiveName() const;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect);

  Q_SIGNALS:
    void updatePlotRect();
    void updateAxes();
    void triggerRedraw();

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

    void setPlotPixmapDirty() { _plotPixmapDirty = true; } 
    void setAxisLabelsDirty() { _axisLabelsDirty = true; }

  private:
    void createActions();
    void createZoomMenu();
    void createFilterMenu();
    void createFitMenu();

    void resetSelectionRect();

    void updatePlotPixmap();
    virtual void paintPixmap(QPainter *painter);

    void updateXAxisLines();
    void updateYAxisLines();

    void updateXAxisLabels(QPainter* painter);
    void updateYAxisLabels(QPainter* painter);

    virtual void paintPlot(QPainter *painter);

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

    bool _showLegend;

    bool _plotMaximized;
    QPointF _plotMaximizedSourcePosition;
    QRectF _plotMaximizedSourceRect;
    qreal _plotMaximizedSourceZValue;
    ViewItem* _plotMaximizedSourceParent;

    bool _allowUpdates;
    bool _updateDelayed;

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
    QAction *_plotMaximize;

    QMenu *_filterMenu;
    QAction *_filterAction;
    QMenu *_fitMenu;
    QAction *_fitAction;

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
