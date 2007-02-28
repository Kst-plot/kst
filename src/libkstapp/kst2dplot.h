/***************************************************************************
                              kst2dplot.h
                             -------------
    begin                : Mar 28, 2004
    copyright            : (C) 2004 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KST2DPLOT_H
#define KST2DPLOT_H

#include <time.h>

#include "kstbackbuffer.h"
#include "kstbasecurve.h"
#include "kstcolorsequence.h"
#include "kstplotbase.h"
#include "kstplotdefines.h"
#include "kstviewwidget.h"
#include "kstviewlegend.h"
#include "kst_export.h"

#include <q3valuestack.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QMouseEvent>
#include <Q3ValueList>
#include <QKeyEvent>
#include <Q3PtrList>

namespace Equation {
  class Node;
}

class Kst2DPlot;
typedef KstObjectList<KstSharedPtr<Kst2DPlot> > Kst2DPlotList;
class KstViewLabel;
typedef KstSharedPtr<KstViewLabel> KstViewLabelPtr;
class KstPlotLabel;

enum ZoomType { ZOOM_MOVE_HORIZONTAL = 0, ZOOM_MOVE_VERTICAL = 1, ZOOM_CENTER = 2, ZOOM_VERTICAL = 3, ZOOM_HORIZONTAL = 4, ZOOM_X_MODE = 5, ZOOM_Y_MODE = 6, ZOOM_Y_LOCAL_MAX = 7, ZOOM_XY_MODES = 8 };
enum KstScaleModeType { AUTO = 0, AC = 1, FIXED = 2, AUTOUP = 3, NOSPIKE = 4, AUTOBORDER = 5, EXPRESSION = 6 };

struct KstPlotScale {
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  KstScaleModeType xscalemode;
  KstScaleModeType yscalemode;
  bool xlog, ylog;
  QString xMinExp, xMaxExp, yMinExp, yMaxExp;
};

struct KstMarker {
  double value;
  bool isRising;
  bool isFalling;
  bool isVectorValue;
};

typedef Q3ValueList<KstMarker> KstMarkerList;

enum KstMouseModeType { INACTIVE, XY_ZOOMBOX, Y_ZOOMBOX, X_ZOOMBOX, LAYOUT_TOOL };

struct KstMouse {
  KstMouse();
  KstMouseModeType mode;
  int label, minMove;
  QPoint lastLocation, pressLocation;  // for zooming primarily
  QPoint tracker; // for tracking the mouse location
  QPoint lastGuideline; // for tracking the last guideline location
  KstMouseModeType lastGuidelineType;
  QRect plotGeometry;
  bool zooming() const;
  void zoomStart(KstMouseModeType t, const QPoint& location);
  void zoomUpdate(KstMouseModeType t, const QPoint& location);
  QRect mouseRect() const;
  bool rectBigEnough() const;
};


class KST_EXPORT Kst2DPlot : public KstPlotBase {
  Q_OBJECT
public:
  Kst2DPlot(const QString& in_tag = "SomePlot",
          KstScaleModeType yscale = AUTOBORDER,
          KstScaleModeType xscale = AUTO,
          double xmin = 0, double ymin = 0,
          double xmax = 1, double ymax = 1);
  Kst2DPlot(const QDomElement& e);
  Kst2DPlot(const Kst2DPlot& plot, const QString& name);
  virtual ~Kst2DPlot();

  QWidget *configWidget();

  // handle custom widget, if any: is called by KstEditViewObjectDialogI
  bool fillConfigWidget(QWidget *w, bool isNew) const;
  bool readConfigWidget(QWidget *w);
  void connectConfigWidget(QWidget *parent, QWidget *w) const;
    
  static Kst2DPlotList globalPlotList();
  static Kst2DPlotPtr findPlotByName(const QString& name);
  static bool checkRange(double& min_in, double& max_in);
  static bool checkLRange(double& min_in, double& max_in, bool isLog, double logBase);
  static void genAxisTickLabel(QString& label, double z, bool isLog, double logBase);

  virtual UpdateType update(int update_counter);
  virtual void save(Q3TextStream& ts, const QString& indent = QString::null);
  virtual void saveAttributes(Q3TextStream& ts, const QString& indent = QString::null);
  
  virtual bool popupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent);
  virtual bool layoutPopupMenu(KPopupMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent);

  void drawGraphicSelectionAt(QPainter& p, const QPoint& pos);
  void drawDotAt(QPainter& p, double x, double y);
  void drawPlusAt(QPainter& p, double x, double y);
  void internalAlignment(KstPainter& p, QRect& plotRegion);
  bool addCurve(KstBaseCurvePtr curve);
  void removeCurve(KstBaseCurvePtr curve);
  void clearCurves();
  void setXScaleMode(KstScaleModeType scalemode);
  void setYScaleMode(KstScaleModeType scalemode);
  void nextImageColorScale();

  void pushAdjustLineWidth(int adjustment);
  void popLineWidth();
  void pushCurveColor(const QColor& c);
  void popCurveColor();
  void pushCurveHasPoints(bool yes);
  void popCurveHasPoints();
  void pushCurveHasLines(bool yes);
  void popCurveHasLines();
  void pushCurvePointDensity(int pointDensity);
  void popCurvePointDensity();
  void pushPlotColors();
  void popPlotColors();

  /** Set the scale */
  void setScale(double xmin, double ymin, double xmax, double ymax);
  /** Set the X scale */
  bool setXScale(double xmin, double xmax);
  /** Set the y scale */
  bool setYScale(double ymin, double ymax);

  // set the X Expressions
  bool setXExpressions(const QString& minExp, const QString& maxExp);
  // set the Y Expressions
  bool setYExpressions(const QString& minExp, const QString& maxExp);

  /** Set the log_scale */
  bool setLScale(double xmin, double ymin, double xmax, double ymax);
  /** Set the X log_scale */
  bool setLXScale(double xmin, double xmax);
  /** Set the y log_scale */
  bool setLYScale(double ymin, double ymax);
  /** Push the scale setting (X,Y, mode) onto scale list */
  void pushScale();
  /** Pop the scale settings off of scale list: true if stack not empty */
  bool popScale();

  void setLog(bool x_log, bool y_log);
  double xLogBase() const;
  double yLogBase() const;
  bool isXLog() const;
  bool isYLog() const;
  void setXAxisInterpretation( bool isXAxisInterpreted, KstAxisInterpretation xAxisInterpretation, KstAxisDisplay xAxisDisplay);
  void getXAxisInterpretation( bool& isXAxisInterpreted, KstAxisInterpretation& xAxisInterpretation, KstAxisDisplay& xAxisDisplay) const;
  void setYAxisInterpretation( bool isYAxisInterpreted, KstAxisInterpretation yAxisInterpretation, KstAxisDisplay yAxisDisplay);
  void getYAxisInterpretation( bool& isYAxisInterpreted, KstAxisInterpretation& yAxisInterpretation, KstAxisDisplay& yAxisDisplay) const;
  void getScale(double& xmin, double& ymin, double& xmax, double& ymax) const;

  // get expressions for the scale
  void getXScaleExps(QString& minExp, QString& maxExp) const;
  void getYScaleExps(QString& minExp, QString& maxExp) const;

  void getLScale(double& xmin, double& ymin, double& xmax, double& ymax) const;

  KstScaleModeType xScaleMode() const;
  KstScaleModeType yScaleMode() const;

  KstPlotLabel *xLabel() const;
  KstPlotLabel *yLabel() const;
  KstPlotLabel *topLabel() const;
  KstPlotLabel *xTickLabel() const;
  KstPlotLabel *yTickLabel() const;
  KstPlotLabel *fullTickLabel() const;
  void setPlotLabelFontSizes(int size);
  

  QRect GetPlotRegion() const;
  QRect GetWinRegion() const;
  QRect GetTieBoxRegion() const;
  QRect GetPlotAndAxisRegion() const;

  bool isTied() const;
  void toggleTied();
  void setTied(bool is_tied);

  virtual QString menuTitle() const;

  double _width;
  double _height;
  double _pos_x;
  double _pos_y;

  KstBaseCurveList Curves;

  void generateDefaultLabels(bool xl = true, bool yl = true, bool zl = true);

  /* kstview tells kstplot where to offset the plot to */
  void setPixRect(const QRect& RelPlotRegion, const QRect& RelWinRegion, const QRect& RelPlotAndAxisRegion);

  virtual void move(const QPoint&);
  virtual void resize(const QSize&);
  virtual void parentResized();
  virtual void parentMoved(const QPoint&);

  virtual bool mouseHandler() const;
  virtual void mouseMoveEvent(QWidget *view, QMouseEvent *e);
  virtual void mousePressEvent(QWidget *view, QMouseEvent *e);
  virtual void mouseDoubleClickEvent(QWidget *view, QMouseEvent *e);
  virtual void mouseReleaseEvent(QWidget *view, QMouseEvent *e);
  virtual void keyPressEvent(QWidget *view, QKeyEvent *e);
  virtual void keyReleaseEvent(QWidget *view, QKeyEvent *e);
  virtual void wheelEvent(QWidget *view, QWheelEvent *e);
  virtual void setHasFocus(bool hasFocus);

  void cancelZoom(QWidget *view);
  void zoomSelfYLocalMax(bool unused);
  bool moveSelfHorizontal(bool left);
  bool moveSelfVertical(bool up);
  bool zoomSelfHorizontal(bool in);
  bool zoomSelfVertical(bool in);
  double xLeft() const;
  void moveUp(KstViewWidget *);
  void moveDown(KstViewWidget *);
  void moveLeft(KstViewWidget *);
  void moveRight(KstViewWidget *);
  void xZoomIn(KstViewWidget *);
  void yZoomIn(KstViewWidget *);
  void xZoomOut(KstViewWidget *);
  void yZoomOut(KstViewWidget *);
  void xZoomMax(KstViewWidget *);
  void yZoomMax(KstViewWidget *);
  void yZoomLocalMax(KstViewWidget *);
  void zoomMax(KstViewWidget *);
  void xLogSlot(KstViewWidget *);
  void yLogSlot(KstViewWidget *);
  void zoomPrev(KstViewWidget *);
  void yZoomAc(KstViewWidget *);
  void xZoomNormal(KstViewWidget *);
  void yZoomNormal(KstViewWidget *);
  void zoomSpikeInsensitiveMax(KstViewWidget *);

  // plot Markers
  bool setPlotMarker(const double xValue, bool isRisng = false, bool isFalling = false, bool isVectorValue = false);
  bool removePlotMarker(const double xValue);
  void setPlotMarkerList(const KstMarkerList& newMarkers);
  const KstMarkerList plotMarkers(const double minX, const double maxX) const;
  const KstMarkerList& plotMarkers() const;
  bool nextMarker(const double currentPosition, double& marker);
  bool prevMarker(const double currentPosition, double& marker);
  void moveToNextMarker(KstViewWidget*);
  void moveToPrevMarker(KstViewWidget*);
  void moveSelfToCenter(double center);

  // curve related plot marker functions
  void setCurveToMarkers(KstVCurvePtr curve, bool risingDetect, bool fallingDetect);
  bool hasCurveToMarkers() const;
  void removeCurveToMarkers();
  KstVCurvePtr curveToMarkers() const;
  bool curveToMarkersRisingDetect() const;
  bool curveToMarkersFallingDetect() const;
  void setVectorToMarkers(KstVectorPtr vector);
  bool hasVectorToMarkers() const;
  void removeVectorToMarkers();
  KstVectorPtr vectorToMarkers() const;
  
  void setXOffsetMode(bool xOffsetMode) { _xOffsetMode = xOffsetMode; }
  void setYOffsetMode(bool yOffsetMode) { _yOffsetMode = yOffsetMode; }
  bool xOffsetMode() const { return _xOffsetMode; }
  bool yOffsetMode() const { return _yOffsetMode; }

  // plot grid lines
  void setGridLinesColor(const QColor& majorColor, const QColor& minorColor,
                         bool majorGridColorDefault, bool minorGridColorDefault);
  void setXGridLines(bool xMajor, bool xMinor);
  void setYGridLines(bool yMajor, bool yMinor);
  bool hasXMajorGrid() const { return _xMajorGrid; }
  bool hasYMajorGrid() const { return _yMajorGrid; }
  bool hasXMinorGrid() const { return _xMinorGrid; }
  bool hasYMinorGrid() const { return _yMinorGrid; }
  const QColor& majorGridColor() const { return _majorGridColor; }
  const QColor& minorGridColor() const { return _minorGridColor; }
  bool defaultMajorGridColor() const { return _majorGridColorDefault; }
  bool defaultMinorGridColor() const { return _minorGridColorDefault; }

  void setXMinorTicks(int minorTicks); //set to -1 for auto
  void setYMinorTicks(int minorTicks); //set to -1 for auto
  int xMinorTicks() const { return _xMinorTicks - 1; }
  int yMinorTicks() const { return _yMinorTicks - 1; }
  bool xMinorTicksAuto() const { return _reqXMinorTicks < 0; }
  bool yMinorTicksAuto() const { return _reqYMinorTicks < 0; }
  void setXMajorTicks(int majorTicks);
  void setYMajorTicks(int majorTicks);
  int xMajorTicks() const { return _xMajorTicks; }
  int yMajorTicks() const { return _yMajorTicks; }

  void setMajorPenWidth(int mpw) { _majorPenWidth = mpw; }
  int majorPenWidth() const { return _majorPenWidth; }

  void setMinorPenWidth(int mpw) { _minorPenWidth = mpw; }
  int minorPenWidth() const { return _minorPenWidth; }

  void setAxisPenWidth(int apw) { _axisPenWidth = apw; }
  int axisPenWidth() const { return _axisPenWidth; }

  void setColorMarkers(QColor color) { _colorMarkers = color; }
  QColor colorMarkers() const { return _colorMarkers; }
  void setLineWidthMarkers(int lineWidth) { _lineWidthMarkers = lineWidth; }
  int lineWidthMarkers() const { return _lineWidthMarkers; }
  void setLineStyleMarkers(int lineStyle) { _lineStyleMarkers = lineStyle; }
  int lineStyleMarkers() const { return _lineStyleMarkers; }
  void setDefaultColorMarker(bool defaultColor) { _defaultMarkerColor = defaultColor; }
  bool defaultColorMarker() const { return _defaultMarkerColor; }
  
  // set and get tick mark display options
  void setXTicksInPlot(bool yes);
  void setXTicksOutPlot(bool yes);
  void setYTicksInPlot(bool yes);
  void setYTicksOutPlot(bool yes);
  bool xTicksInPlot() const;
  bool xTicksOutPlot() const;
  bool yTicksInPlot() const;
  bool yTicksOutPlot() const;

  bool suppressTop() const;
  bool suppressBottom() const;
  bool suppressLeft() const;
  bool suppressRight() const;
  void setSuppressTop(bool yes);
  void setSuppressBottom(bool yes);
  void setSuppressLeft(bool yes);
  void setSuppressRight(bool yes);

  // change EXPRESSION mode to FIXED mode if possible
  void optimizeXExps();
  void optimizeYExps();

  // transformed opposite axes
  void setXTransformedExp(const QString& exp);
  void setYTransformedExp(const QString& exp);
  const QString& xTransformedExp() const;
  const QString& yTransformedExp() const;

  // axes reversal
  void setXReversed(bool yes);
  void setYReversed(bool yes);
  bool xReversed() const;
  bool yReversed() const;

  // change the color mode for the plot
  // If any of pointStylePriority, lineStylePriority, lineWidthPriority are == -1, then the
  // corresponding curve property is not cycled or altered. The remaining priorities must
  // be in the range [0, X-1] with no duplicates, where X = number of priorities != -1
  void changeToMonochrome(int pointStylePriority, int lineStylePriority, int lineWidthPriority,
                          int maxLineWidth, int pointDensity);
  // undo changes made by changeToMonochrome
  // PRE: pointStylePriority, lineStylePriority, lineWidthPriority must have same values as when
  //      passed to changeToMonochrome (otherwise behaviour is not as expected)
  bool undoChangeToMonochrome(int pointStylePriority, int lineStylePriority, int lineWidthPriority);

  //convenience routines for working with viewLegends
  KstViewLegendPtr legend() const;

  KstViewLegendPtr getOrCreateLegend();
  
  virtual QRect contentsRect() const;

  double verticalSizeFactor();
  double horizontalSizeFactor();
  
signals:
  void modified();

public slots:
  void copy();

  virtual void edit();
  virtual void copyObject();
  virtual KstViewObject* copyObjectQuietly(KstViewObject& parent, const QString& name = QString::null) const;
  void draw(); // draw into back buffer
  void draw(KstPainter& p); // This actually does the drawing
  void paintSelf(KstPainter& p, const QRegion& bounds);
  void updateSelf();
  void editCurve(int id);
  void editObject(int id);
  void editVector(int id);
  void editMatrix(int id);
  void matchAxes(int id);
  void matchXAxis(int id);
  bool tiedZoomPrev(QWidget *view);
  bool tiedZoomMode(ZoomType zoom, bool flag, double center, KstScaleModeType mode, KstScaleModeType modeExtra);
  void tiedZoom(bool x, double xmin, double xmax, bool y, double ymin, double ymax);
  void fitCurve(int id);
  void filterCurve(int id);
  void removeCurve(int id);

protected slots:
  void menuMoveUp();
  void menuMoveDown();
  void menuMoveLeft();
  void menuMoveRight();
  void menuXZoomIn();
  void menuXZoomOut();
  void menuYZoomIn();
  void menuYZoomOut();
  void menuXZoomMax();
  void menuYZoomMax();
  void menuYZoomLocalMax();
  void menuZoomMax();
  void menuXLogSlot();
  void menuYLogSlot();
  void menuZoomPrev();
  void menuYZoomAc();
  void menuXNormalize();
  void menuYNormalize();
  void menuZoomSpikeInsensitiveMax();
  void menuNextMarker();
  void menuPrevMarker();
  void menuNextImageColorScale();

  void timezoneChanged(const QString& tz, int utcOffset);

private:
  // for backwards compatibility with old labels - convert an old saved label to a KstViewLabel
  KstViewLabelPtr convertLabelToViewLabel(const QDomElement &e);

  void updateDirtyFromLabels();
  void setCursorPos(QWidget *view);
  void unsetCursorPos(QWidget *view);
  void drawCursorPos(QWidget *view);
  void drawCursorPos(QPainter& p);
  void updateMousePos(const QPoint& pos);
  void getCursorPos(const QPoint& pos, double &xpos, double &ypos, double &xmin, double &xmax, double &ymin, double& ymax);
  bool getNearestDataPoint(const QPoint& pos, QString& name, double &newxpos, double &newypos, double xpos, double ypos, double xmin, double xmax);
  void highlightNearestDataPoint(bool repaint, KstPainter *p, const QPoint& pos);
  void updateTieBox(QPainter&);
  bool legendUnder(QMouseEvent *e);
  KstMouseModeType globalZoomType() const;
  void setCursorForMode(QWidget *view, KstMouseModeType mode, const QPoint& pos);
  void zoomRectUpdate(QWidget *view, KstMouseModeType t, int x, int y);
  inline void commonConstructor(const QString& in_tag,
                                KstScaleModeType xscale,
                                KstScaleModeType yscale,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax,
                                bool x_log = false,
                                bool y_log = false,
                                double x_logbase = 10.0,
                                double y_logbase = 10.0);
  QPair<double, double> computeAutoBorder(bool log, double logBase, double min, double max);
  void setBorders(double& xleft_bdr_px, double& xright_bdr_px,
                         double& ytop_bdr_px, double& ybot_bdr_px,
                         TickParameters &tpx,  TickParameters &tpy,
                         QPainter& p, bool& bOffsetX, bool& bOffsetY,
                         double xtick_len_px, double ytick_len_px);
  void setTicks(double& tick, double& org,
                double max, double min, bool is_log, double logBase, bool isX, int base);
  double convertTimeValueToJD(KstAxisInterpretation axisInterpretation, double valueIn);
  double convertTimeDiffValueToDays(KstAxisInterpretation axisInterpretation, double diffIn);
  void convertJDToDateString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, uint& length, double dJD);
  void convertTimeValueToString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, uint& length, double z, bool isLog, double logBase);
  void convertDiffTimevalueToString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double& zdiff, double zbase, double zvalue, bool isLog, double scale);
  void genAxisTickLabelFullPrecision(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, uint& length, double z, bool isLog, double logBase, bool isInterpreted);
  void genAxisTickLabelDifference(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, double zbase, double zvalue, bool isLog, double logBase, bool isInterpreted, double scale);
  void getPrefixUnitsScale(bool isInterpreted, KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, bool bLog, double logBase, double Min, double Max, double& range, double& scale, int& base, QString& strPrefix, QString& strUnits);
  void genOffsetLabel(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, double zbase, double zvalue, double Min, double Max, bool isLog, double logBase, bool isInterpreted);
  void genAxisTickLabels(TickParameters &tp, double Min, double Max, bool bLog, double logBase, KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, bool isX, bool isInterpreted, bool isOffsetMode);

  // helper function needed before setBorders
  void set2dPlotTickPix(double& xtickpix, double& ytickpix, int x_pix, int y_pix);

  void updateXYGuideline(QWidget *view, const QPoint& oldPos, const QPoint& newPos, const QRect& pr, KstMouseModeType gzType);

  // range and domain of plot: not plot dimentions
  double XMin, XMax, YMin, YMax;

  double _copy_x, _copy_y;
  double _cursor_x, _cursor_y;

  // grid lines
  QColor _majorGridColor;
  QColor _minorGridColor;
  bool _xMajorGrid : 1;
  bool _xMinorGrid : 1;
  bool _yMajorGrid : 1;
  bool _yMinorGrid : 1;
  bool _majorGridColorDefault : 1;
  bool _minorGridColorDefault : 1;
  
  int _majorPenWidth;
  int _minorPenWidth;
  int _axisPenWidth;

  double _xLogBase;
  double _yLogBase;
  
  bool _cursorOffset : 1;

  bool _xLog : 1;
  bool _yLog : 1;
  bool _isTied : 1;
  bool _zoomPaused : 1;
  bool _curveToMarkersRisingDetect : 1;
  bool _curveToMarkersFallingDetect : 1;
  bool _isLogLast : 1;

  bool _xOffsetMode : 1;
  bool _yOffsetMode : 1;

  // tick mark display options
  bool _xTicksInPlot : 1;
  bool _xTicksOutPlot : 1;
  bool _yTicksInPlot : 1;
  bool _yTicksOutPlot : 1;

  // suppress labels and axes
  bool _suppressTop : 1;
  bool _suppressBottom : 1;
  bool _suppressLeft : 1;
  bool _suppressRight : 1;

  bool _xMinParsedValid : 1, _xMaxParsedValid : 1, _yMinParsedValid : 1, _yMaxParsedValid : 1;
  bool _xTransformed : 1, _yTransformed : 1;

  bool _isXAxisInterpreted : 1, _isYAxisInterpreted : 1;

  // reverse axes
  bool _xReversed : 1, _yReversed : 1;

  bool _drawingGraphics : 1;

  KstAxisInterpretation _xAxisInterpretation, _yAxisInterpretation;
  KstAxisDisplay _xAxisDisplay, _yAxisDisplay;

  KstScaleModeType _xScaleMode, _yScaleMode;

  Q3PtrList<KstPlotScale> _plotScaleList;

  // hold last minimum and maximum y values that were plotted
  double _yPlottedMinCached, _yPlottedMaxCached;


  /** Stores border limits.  Set by KstPlot::paint().
      Stored here to be Used to determine mouse mode */
  QRect PlotRegion;
  QRect WinRegion;
  QRect PlotAndAxisRegion;

  void updateScale();
  //void adjustFontSize();

  KstMouse _mouse;
  QMap<int, QString> _curveEditMap, _curveFitMap, _curveRemoveMap, _objectEditMap;
  QMap<int, QPointer<Kst2DPlot> > _plotMap;

  KstBackBuffer _buffer;
  QPointer<KstViewWidget> _menuView;
  QPointer<KstViewWidget> _layoutMenuView;
  QSize _oldSize;
  QRect _oldAlignment;
  double _m_X, _b_X, _m_Y, _b_Y;
  double _tickYLast, _stLast;
  int _autoTickYLast;

  QColor _colorMarkers;
  int _lineWidthMarkers;
  int _lineStyleMarkers;
  bool _defaultMarkerColor : 1;

  // plot markers. This needs to remain sorted
  KstMarkerList _plotMarkers;
  // the vcurve used for auto marker generation
  KstVCurvePtr _curveToMarkers;
  KstVectorPtr _vectorToMarkers;
  void updateMarkersFromCurve();  //auto-generates the markers from _curveToMarkers
  void updateMarkersFromVector(); //auto-generates the markers from _vectorToMarkers

  // minor tick marks.
  int _reqXMinorTicks; // -1 means use auto
  int _reqYMinorTicks; // -1 means use auto
  int _xMinorTicks;
  int _yMinorTicks;

  int _xMajorTicks;
  int _yMajorTicks;
  
  int _i_per; // index for next image color range

  KstPlotLabel *_xLabel, *_yLabel, *_topLabel, *_xTickLabel, *_yTickLabel, *_fullTickLabel;

  Q3ValueStack<QColor> _colorStack;

  // for range expressions
  bool reparse(const QString& stringExp, Equation::Node** eqNode);
  bool reparseToText(QString& stringExp);
  Equation::Node* _xMinParsed;
  Equation::Node* _xMaxParsed;
  Equation::Node* _yMinParsed;
  Equation::Node* _yMaxParsed;

  // expressions for range and domain of plot
  QString _xMinExp, _xMaxExp, _yMinExp, _yMaxExp;

  // for the transformed axes
  QString _xTransformedExp, _yTransformedExp;

  // helper functions for draw(...)
  void plotLabels(QPainter& p, int x_px, int y_px, double xleft_bdr_px, double xright_bdr_px, double ytop_bdr_px);
  void plotAxes(QPainter& p, QRect& plotRegion,
      TickParameters tpx,
      double xleft_bdr_px, double xright_bdr_px,
      double x_orig_px, double xtick_px,
      double xtick_len_px, int x_px,
      TickParameters tpy,
      double ytop_bdr_px, double ybot_bdr_px,
      double y_orig_px, double ytick_px,
      double ytick_len_px, int y_px,
      bool offsetY);
  void plotGridLines(KstPainter& p,
      double XTick, double xleft_bdr_px, double xright_bdr_px,
      double x_orig_px, double xtick_px, double xtick_len_px, int x_px,
      double YTick, double ytop_bdr_px, double ybot_bdr_px,
      double y_orig_px, double ytick_px, double ytick_len_px, int y_px);
  void plotPlotMarkers(KstPainter& p, double b_X, double b_Y, double x_max, double x_min,
      double y_px, double ytop_bdr_px, double ybot_bdr_px);

  KstScaleModeType _xScaleModeDefault;
  KstScaleModeType _yScaleModeDefault;
  
  int _tabToShow;
};

typedef KstSharedPtr<Kst2DPlot> Kst2DPlotPtr;
typedef KstObjectMap<Kst2DPlotPtr> Kst2DPlotMap;

#endif
// vim: ts=2 sw=2 et
