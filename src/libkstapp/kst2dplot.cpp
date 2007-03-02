/***************************************************************************
                              kst2dplot.cpp
                             ---------------
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

#include <assert.h>
#include <time.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __sun
#include <ieeefp.h>
#endif
#include <limits.h>
#include <math.h>

// include files for Qt
#include <qbitmap.h>
#include <qclipboard.h>
#include <q3deepcopy.h>
#include <q3pointarray.h>
#include <q3stylesheet.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QKeyEvent>
#include <QMouseEvent>

// include files for KDE
#include <qdebug.h>
#include <kglobal.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <QFontComboBox>

// application specific includes
#include "dialoglauncher.h"
#include "enodes.h"
#include "eparse-eh.h"
#include "kst2dplot.h"
#include "kstdataobjectcollection.h"
#include "kstdebug.h"
#include "kstdoc.h"
#include "kstfitdialog.h"
#include "kstfilterdialog.h"
#include "kstgfxmousehandler.h"
#include "kstimage.h"
#include "kstlinestyle.h"
#include "kstmath.h"
#include "kstnumbersequence.h"
#include "kstplotlabel.h"
#include "kstrmatrix.h"
#include "kstrvector.h"
#include "kstsettings.h"
#include "ksttimers.h"
#include "kstviewwindow.h"
#include "kstviewlabel.h"
#include "kstvcurve.h"
#include "kstviewlabel.h"
#include "kstviewlegend.h"
#include "kstviewobjectfactory.h"
#include "plotmimesource.h"
#include "view2dplotwidget.h"
#include "plotlistbox.h"
#include "vectorselector.h"
#include "kstgfx2dplotmousehandler.h"

#define JD1900                  2415020.5
#define JD1970                  2440587.5
#define JD_RJD                  2400000.0
#define JD_MJD                  2400000.5
#define TICK_HYSTERESIS_FACTOR  1.25
#define DIFFERENCE_PRECISION    7
#define LABEL_PRECISION         9
#ifndef DBL_EPSILON
  #define DBL_EPSILON           2.2204460492503131e-016
#endif
#ifndef DBL_DIG
  #define FULL_PRECISION        15
#else
  #define FULL_PRECISION        DBL_DIG
#endif

#define CONTENT_TAB     0
#define APPEARANCE_TAB  1
#define X_AXIS_TAB      2
#define Y_AXIS_TAB      3
#define RANGE_TAB       4
#define MARKERS_TAB     5


extern "C" int yyparse();
extern "C" void *ParsedEquation;
extern "C" struct yy_buffer_state *yy_scan_string(const char*);

Kst2DPlot::Kst2DPlot(const QString& in_tag,
                 KstScaleModeType yscale_in,
                 KstScaleModeType xscale_in,
                 double xmin_in, double ymin_in,
                 double xmax_in, double ymax_in)
: KstPlotBase("Kst2DPlot") {
  // must stay here for plot loading correctness
  _pos_x = 0.0;
  _pos_y = 0.0;
  _width = 0.0;
  _height = 0.0;
  _xOffsetMode = false;
  _yOffsetMode = false;

  // defaults
  _majorGridColor = KstSettings::globalSettings()->majorColor;
  _minorGridColor = KstSettings::globalSettings()->minorColor;
  _majorGridColorDefault = KstSettings::globalSettings()->majorGridColorDefault;
  _minorGridColorDefault = KstSettings::globalSettings()->minorGridColorDefault;

  // FIXME: should be in kstsettings
  _majorPenWidth = 1;
  _minorPenWidth = 1;
  _axisPenWidth = 1;

  _xMajorGrid = KstSettings::globalSettings()->xMajor;
  _xMinorGrid = KstSettings::globalSettings()->xMinor;
  _yMajorGrid = KstSettings::globalSettings()->yMajor;
  _yMinorGrid = KstSettings::globalSettings()->yMinor;
  _xMajorTicks = 5;
  _yMajorTicks = 5;
  _reqXMinorTicks = -1;
  _reqYMinorTicks = -1;
  _isXAxisInterpreted = KstSettings::globalSettings()->xAxisInterpret;
  _xAxisInterpretation = KstSettings::globalSettings()->xAxisInterpretation;
  _xAxisDisplay = KstSettings::globalSettings()->xAxisDisplay;
  _isYAxisInterpreted = KstSettings::globalSettings()->yAxisInterpret;
  _yAxisInterpretation = KstSettings::globalSettings()->yAxisInterpretation;
  _yAxisDisplay = KstSettings::globalSettings()->yAxisDisplay;
  _drawingGraphics = false;
  _xTicksInPlot = true;
  _xTicksOutPlot = false;
  _yTicksInPlot = true;
  _yTicksOutPlot = false;
  _suppressTop = false;
  _suppressBottom = false;
  _suppressLeft = false;
  _suppressRight = false;
  _xScaleModeDefault = AUTO;
  _yScaleModeDefault = AUTOBORDER;
  _xTransformed = false;
  _yTransformed = false;
  _xReversed = false;
  _yReversed = false;
  _lineStyleMarkers = 0;
  _lineWidthMarkers = 0;
  _colorMarkers = QColor("black");
  _defaultMarkerColor = true;

  commonConstructor(in_tag, xscale_in, yscale_in, xmin_in, ymin_in,
                    xmax_in, ymax_in);
}


Kst2DPlot::Kst2DPlot(const QDomElement& e)
: KstPlotBase(e) {
  QString in_tag = "unknown";
  KstScaleModeType yscale_in = AUTOBORDER, xscale_in = AUTO;
  double xmin_in = 0, ymin_in = 0, xmax_in = 1, ymax_in = 1;
  QString xminexp_in, xmaxexp_in, yminexp_in, ymaxexp_in;
  QStringList ctaglist;
  bool x_log = false, y_log = false;
  double x_logbase = 10.0, y_logbase = 10.0;
  QString in_curveToMarkersName;
  QString in_vectorToMarkersName;
  bool in_curveToMarkersRisingDetect = false;
  bool in_curveToMarkersFallingDetect = false;
  KstMarker marker;
  _drawingGraphics = false;

  marker.isRising = false;
  marker.isFalling = false;

  // for older .kst files that don't have these settings
  _xMajorGrid = false;
  _xMinorGrid = false;
  _yMajorGrid = false;
  _yMinorGrid = false;
  _majorPenWidth = 1;
  _minorPenWidth = 1;
  _axisPenWidth = 1;
  _xMajorTicks = 5;
  _yMajorTicks = 5;
  _reqXMinorTicks = -1;
  _reqYMinorTicks = -1;
  _isXAxisInterpreted = KstSettings::globalSettings()->xAxisInterpret;
  _xAxisInterpretation = KstSettings::globalSettings()->xAxisInterpretation;
  _xAxisDisplay = KstSettings::globalSettings()->xAxisDisplay;
  _isYAxisInterpreted = KstSettings::globalSettings()->yAxisInterpret;
  _yAxisInterpretation = KstSettings::globalSettings()->yAxisInterpretation;
  _yAxisDisplay = KstSettings::globalSettings()->yAxisDisplay;
  _xTicksInPlot = true;
  _xTicksOutPlot = false;
  _yTicksInPlot = true;
  _yTicksOutPlot = false;
  _suppressTop = false;
  _suppressBottom = false;
  _suppressLeft = false;
  _suppressRight = false;
  _xTransformed = false;
  _yTransformed = false;
  _xReversed = false;
  _yReversed = false;
  _majorGridColorDefault = KstSettings::globalSettings()->majorGridColorDefault;
  _minorGridColorDefault = KstSettings::globalSettings()->minorGridColorDefault;
  _lineStyleMarkers = 0;
  _lineWidthMarkers = 0;
  _colorMarkers = QColor("black");
  _defaultMarkerColor = true;

  // must stay here for plot loading correctness
  _pos_x = 0.0;
  _pos_y = 0.0;
  _width = 0.0;
  _height = 0.0;
  _xOffsetMode = false;
  _yOffsetMode = false;
  _xScaleModeDefault = AUTO;
  _yScaleModeDefault = AUTOBORDER;

  QDomElement xLabelNode, yLabelNode, topLabelNode, xTickLabelNode, yTickLabelNode, fullTickLabelNode;

  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement(); // try to convert the node to an element.
    if (!el.isNull()) { // the node was really an element.
      if (el.tagName() == "width") {
        _width = el.text().toDouble();
      } else if (el.tagName() == "height") {
        _height = el.text().toDouble();
      } else if (el.tagName() == "pos_x") {
        _pos_x = el.text().toDouble();
      } else if (el.tagName() == "pos_y") {
        _pos_y = el.text().toDouble();
      } else if (el.tagName() == "xscalemode") {
        xscale_in = KstScaleModeType(el.text().toInt());
      } else if (el.tagName() == "yscalemode") {
        yscale_in = KstScaleModeType(el.text().toInt());
      } else if (el.tagName() == "xscalemodedefault") {
        _xScaleModeDefault = KstScaleModeType(el.text().toInt());
      } else if (el.tagName() == "yscalemodedefault") {
        _yScaleModeDefault = KstScaleModeType(el.text().toInt());
      } else if (el.tagName() == "xmin") {
        xmin_in = el.text().toDouble();
      } else if (el.tagName() == "xmax") {
        xmax_in = el.text().toDouble();
      } else if (el.tagName() == "ymin") {
        ymin_in = el.text().toDouble();
      } else if (el.tagName() == "ymax") {
        ymax_in = el.text().toDouble();
      } else if (el.tagName() == "toplabel") {
        topLabelNode = el;
      } else if (el.tagName() == "xlabel") {
        xLabelNode = el;
      } else if (el.tagName() == "ylabel") {
        yLabelNode = el;
      } else if (el.tagName() == "xticklabel") {
        xTickLabelNode = el;
      } else if (el.tagName() == "yticklabel") {
        yTickLabelNode = el;
      } else if (el.tagName() == "xfullticklabel") {
        fullTickLabelNode = el;
      } else if (el.tagName() == "yfullticklabel") {
        // depricated... redundant with xfullticklabel
      } else if (el.tagName() == "label") {
        KstViewLabelPtr label = convertLabelToViewLabel(el);
        appendChild(KstViewObjectPtr(label), true);
      } else if (el.tagName() == "curvetag") {
        ctaglist.append(el.text());
      } else if (el.tagName() == "image") { // for compatibility with older kst files
        ctaglist.append(el.text());
      } else if (el.tagName() == "xlog") {
        x_log = true;
      } else if (el.tagName() == "ylog") {
        y_log = true;
      } else if (el.tagName() == "xlogbase") {
        x_logbase = el.text().toDouble();
      } else if (el.tagName() == "ylogbase") {
        y_logbase = el.text().toDouble();
      } else if (el.tagName() == "plotforecolor") {
        _foregroundColor.setNamedColor(el.text());
      } else if (el.tagName() == "plotbackcolor") {
        _backgroundColor.setNamedColor(el.text());
      } else if (el.tagName() == "axispenwidth") {
        setAxisPenWidth(el.text().toInt());
      } else if (el.tagName() == "majorpenwidth") {
        setMajorPenWidth(el.text().toInt());
      } else if (el.tagName() == "minorpenwidth") {
        setMinorPenWidth(el.text().toInt());
      } else if (el.tagName() == "plotmarker") {
        if (el.attribute("value").isEmpty()) {
          marker.isRising = false;
          marker.isFalling = false;
          marker.value = el.text().toDouble();
          _plotMarkers.append(marker);
        } else {
          marker.isRising = !el.attribute("rising").isEmpty();
          marker.isFalling = !el.attribute("falling").isEmpty();
          marker.isVectorValue = !el.attribute("vector").isEmpty();
          marker.value = el.attribute("value").toDouble();
          _plotMarkers.append(marker);
        }
      } else if (el.tagName() == "curvetomarkersname") {
        in_curveToMarkersName = el.text();
      } else if (el.tagName() == "curvetomarkersrisingdetect") {
        in_curveToMarkersRisingDetect = el.text() != "0";
      } else if (el.tagName() == "curvetomarkersfallingdetect") {
        in_curveToMarkersFallingDetect = el.text() != "0";
      } else if (el.tagName() == "vectortomarkersname") {
        in_vectorToMarkersName = el.text();
      } else if (el.tagName() == "xmajorgrid") {
        _xMajorGrid = el.text() != "0";
      } else if (el.tagName() == "ymajorgrid") {
        _yMajorGrid = el.text() != "0";
      } else if (el.tagName() == "xminorgrid") {
        _xMinorGrid = el.text() != "0";
      } else if (el.tagName() == "yminorgrid") {
        _yMinorGrid = el.text() != "0";
      } else if (el.tagName() == "majorgridcolor") {
        _majorGridColor.setNamedColor(el.text());
      } else if (el.tagName() == "minorgridcolor") {
        _minorGridColor.setNamedColor(el.text());
      } else if (el.tagName() == "yminorticks") {
        _reqYMinorTicks = el.text().toInt();
      } else if (el.tagName() == "xminorticks") {
        _reqXMinorTicks = el.text().toInt();
      } else if (el.tagName() == "majorgridcolordefault") {
        _majorGridColorDefault = el.text() != "0";
      } else if (el.tagName() == "minorgridcolordefault") {
        _minorGridColorDefault = el.text() != "0";
      } else if (el.tagName() == "ymajorticks") {
        _yMajorTicks = el.text().toInt();
      } else if (el.tagName() == "xmajorticks") {
        _xMajorTicks = el.text().toInt();
      } else if (el.tagName() == "xinterpret") {
        _isXAxisInterpreted = el.text() != "0";
      } else if (el.tagName() == "xinterpretas") {
        _xAxisInterpretation = KstAxisInterpretation(el.text().toInt());
      } else if (el.tagName() == "xdisplayas") {
        _xAxisDisplay = KstAxisDisplay(el.text().toInt());
      } else if (el.tagName() == "yinterpret") {
        _isYAxisInterpreted = el.text() != "0";
      } else if (el.tagName() == "yinterpretas") {
        _yAxisInterpretation = KstAxisInterpretation(el.text().toInt());
      } else if (el.tagName() == "ydisplayas") {
        _yAxisDisplay = KstAxisDisplay(el.text().toInt());
      } else if (el.tagName() == "xoffsetmode") {
        _xOffsetMode = el.text() != "0";
      } else if (el.tagName() == "yoffsetmode") {
        _yOffsetMode = el.text() != "0";
      } else if (el.tagName() == "xminexp") {
        xminexp_in = el.text();
      } else if (el.tagName() == "xmaxexp") {
        xmaxexp_in = el.text();
      } else if (el.tagName() == "yminexp") {
        yminexp_in = el.text();
      } else if (el.tagName() == "ymaxexp") {
        ymaxexp_in = el.text();
      } else if (el.tagName() == "xticksinplot") {
        _xTicksInPlot = el.text() != "0";
      } else if (el.tagName() == "xticksoutplot") {
        _xTicksOutPlot = el.text() != "0";
      } else if (el.tagName() == "yticksinplot") {
        _yTicksInPlot = el.text() != "0";
      } else if (el.tagName() == "yticksoutplot") {
        _yTicksOutPlot = el.text() != "0";
      } else if (el.tagName() == "suppresstop") {
        _suppressTop = el.text() != "0";
      } else if (el.tagName() == "suppressbottom") {
        _suppressBottom = el.text() != "0";
      } else if (el.tagName() == "suppressleft") {
        _suppressLeft = el.text() != "0";
      } else if (el.tagName() == "suppressright") {
        _suppressRight = el.text() != "0";
      } else if (el.tagName() == "xtransformed") {
        _xTransformed = el.text() != "0";
      } else if (el.tagName() == "ytransformed") {
        _yTransformed = el.text() != "0";
      } else if (el.tagName() == "xtransformedexp") {
        _xTransformedExp = el.text();
      } else if (el.tagName() == "ytransformedexp") {
        _yTransformedExp = el.text();
      } else if (el.tagName() == "xreversed") {
        _xReversed = el.text() != "0";
      } else if (el.tagName() == "yreversed") {
        _yReversed = el.text() != "0";
      } else if (el.tagName() == "stylemarker") {
        _lineStyleMarkers = el.text().toInt();
      } else if (el.tagName() == "widthmarker") {
        _lineWidthMarkers = el.text().toInt();
      } else if (el.tagName() == "colormarker") {
        _colorMarkers.setNamedColor(el.text());
      } else if (el.tagName() == "defaultcolormarker") {
         _defaultMarkerColor = el.text() != "0";
      }
    }
    n = n.nextSibling();
  }

  if (_xScaleModeDefault != AUTO || _xScaleModeDefault != AUTOBORDER) {
    _xScaleModeDefault = AUTO;
  }
  if (_yScaleModeDefault != AUTO || _yScaleModeDefault != AUTOBORDER) {
    _yScaleModeDefault = AUTOBORDER;
  }

  commonConstructor(tagName(), xscale_in, yscale_in, xmin_in, ymin_in,
                    xmax_in, ymax_in, x_log, y_log, x_logbase, y_logbase);

  if (!topLabelNode.isNull()) {
    _topLabel->load(topLabelNode);
  }

  if (!yLabelNode.isNull()) {
    _yLabel->load(yLabelNode);
  }

  if (!xLabelNode.isNull()) {
    _xLabel->load(xLabelNode);
  }

  if (!xTickLabelNode.isNull()) {
    _xTickLabel->load(xTickLabelNode);
  }

  if (!yTickLabelNode.isNull()) {
    _yTickLabel->load(yTickLabelNode);
  }

  if (!fullTickLabelNode.isNull()) {
    _fullTickLabel->load(fullTickLabelNode);
  }

  KstBaseCurveList l = kstObjectSubList<KstDataObject,KstBaseCurve>(KST::dataObjectList);
  for (unsigned i = 0; i < ctaglist.count(); i++) {
    KstBaseCurveList::Iterator it = l.findTag(ctaglist[i]);
    if (it != l.end()) {
      addCurve(*it);
    }
  }

  // initialized range expressions
  _xMinExp = xminexp_in;
  _xMaxExp = xmaxexp_in;
  _yMinExp = yminexp_in;
  _yMaxExp = ymaxexp_in;
  _yMaxParsedValid = false;
  _yMinParsedValid = false;
  _xMaxParsedValid = false;
  _xMinParsedValid = false;

  if (!in_curveToMarkersName.isEmpty()) {
    KstVCurveList vcurves = kstObjectSubList<KstDataObject, KstVCurve>(KST::dataObjectList);
    KstVCurveList::iterator curves_iter = vcurves.findTag(in_curveToMarkersName);
    setCurveToMarkers(*curves_iter, in_curveToMarkersRisingDetect, in_curveToMarkersFallingDetect);
  }
  if (!in_vectorToMarkersName.isEmpty()) {
    KstVectorList::iterator vectors_iter = KST::vectorList.findTag(in_vectorToMarkersName);
    setVectorToMarkers(*vectors_iter);
  }
}


// FIXME: broken copy constructor
// what is broken?
Kst2DPlot::Kst2DPlot(const Kst2DPlot& plot, const QString& name)
: KstPlotBase(plot) {
  QString plotName;

  _type = "Plot";

  if (name.isEmpty()) {
    plotName = i18n("%1-copy").arg(plot.tagName());
  } else {
    plotName = name;
  }

  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *iter;
  bool duplicate = true;
  int last = 0;

  // check for unique plot name
  while (duplicate) {
    duplicate = false;
    iter = app->createIterator();
    while (iter->currentItem() && !duplicate) {
      KMdiChildView *childview = iter->currentItem();
      KstViewWindow *viewwindow = dynamic_cast<KstViewWindow*>(childview);
      if (viewwindow) {
        if (viewwindow->view()->findChild(plotName)) {
          if (last == 0) {
            plotName = i18n("%1-copy").arg(plot.tagName());
          } else {
            plotName = i18n("%1-copy%2").arg(plot.tagName()).arg(last);
          }
          ++last;

          duplicate = true;
          break;
        }
      }
      iter->next();
    }
    app->deleteIterator(iter);
  }

  commonConstructor(plotName,
                    plot._xScaleMode,
                    plot._yScaleMode,
                    plot.XMin,
                    plot.YMin,
                    plot.XMax,
                    plot.YMax,
                    plot._xLog,
                    plot._yLog,
                    plot._xLogBase,
                    plot._yLogBase);

  *_xLabel = *(plot._xLabel);
  *_yLabel = *(plot._yLabel);
  *_topLabel = *(plot._topLabel);
  *_xTickLabel = *(plot._xTickLabel);
  *_yTickLabel = *(plot._xTickLabel);
  *_fullTickLabel = *(plot._fullTickLabel);

  _xOffsetMode = plot._xOffsetMode;
  _yOffsetMode = plot._yOffsetMode;
  _xMajorGrid = plot._xMajorGrid;
  _xMinorGrid = plot._xMinorGrid;
  _yMajorGrid = plot._yMajorGrid;
  _yMinorGrid = plot._yMinorGrid;
  _majorPenWidth = plot._majorPenWidth;
  _minorPenWidth = plot._majorPenWidth;
  _axisPenWidth = plot._axisPenWidth;
  _xMajorTicks = plot._xMajorTicks;
  _yMajorTicks = plot._yMajorTicks;
  _reqXMinorTicks = plot._reqXMinorTicks;
  _reqYMinorTicks = plot._reqYMinorTicks;
  _isXAxisInterpreted = plot._isXAxisInterpreted;
  _xAxisInterpretation = plot._xAxisInterpretation;
  _xAxisDisplay = plot._xAxisDisplay;
  _isYAxisInterpreted = plot._isYAxisInterpreted;
  _yAxisInterpretation = plot._yAxisInterpretation;
  _yAxisDisplay = plot._yAxisDisplay;
  setDirty(plot.dirty());
  _zoomPaused = plot._zoomPaused;
  _curveToMarkers = plot._curveToMarkers;
  _curveToMarkersRisingDetect = plot._curveToMarkersRisingDetect;
  _curveToMarkersFallingDetect = plot._curveToMarkersFallingDetect;
  _vectorToMarkers = plot._vectorToMarkers;
  _isLogLast = plot._isLogLast;
  _xScaleModeDefault = plot._xScaleModeDefault;
  _yScaleModeDefault = plot._yScaleModeDefault;

  _aspect = plot._aspect;
  _aspectOldZoomedObject = plot._aspectOldZoomedObject;

  _suppressTop = plot._suppressTop;
  _suppressBottom = plot._suppressBottom;
  _suppressLeft = plot._suppressLeft;
  _suppressRight = plot._suppressRight;
  _xTicksInPlot = plot._xTicksInPlot;
  _xTicksOutPlot = plot._xTicksOutPlot;
  _yTicksInPlot = plot._yTicksInPlot;
  _yTicksOutPlot = plot._yTicksOutPlot;
  _xTransformed = plot._xTransformed;
  _yTransformed = plot._yTransformed;
  _xTransformedExp = plot._xTransformedExp;
  _yTransformedExp = plot._yTransformedExp;

  _xMinExp = plot._xMinExp;
  _xMaxExp = plot._xMaxExp;
  _yMinExp = plot._yMinExp;
  _yMaxExp = plot._yMaxExp;

  _xMinParsedValid = false;
  _xMaxParsedValid = false;
  _yMinParsedValid = false;
  _yMaxParsedValid = false;

  _xReversed = plot._xReversed;
  _yReversed = plot._yReversed;

  _lineWidthMarkers = plot._lineWidthMarkers;
  _lineStyleMarkers = plot._lineStyleMarkers;

  Curves = plot.Curves;
}


void Kst2DPlot::commonConstructor(const QString &in_tag,
                                KstScaleModeType xscale_in,
                                KstScaleModeType yscale_in,
                                double xmin_in,
                                double ymin_in,
                                double xmax_in,
                                double ymax_in,
                                bool x_log,
                                bool y_log,
                                double x_logbase,
                                double y_logbase) {
  connect(KstApp::inst(), SIGNAL(timezoneChanged(const QString&, int)), this, SLOT(timezoneChanged(const QString&, int)));
  
  _tabToShow = CONTENT_TAB;
  _xLabel = new KstPlotLabel;
  _yLabel = new KstPlotLabel(270);
  _topLabel = new KstPlotLabel;
  _xTickLabel = new KstPlotLabel;
  _yTickLabel = new KstPlotLabel;
  _fullTickLabel = new KstPlotLabel;
  //_fullTickLabel->setInterpreted(false); // much faster
  //_xTickLabel->setInterpreted(false); // much faster
  //_yTickLabel->setInterpreted(false); // much faster
  _fullTickLabel->setDoScalarReplacement(false); // much faster
  _xTickLabel->setDoScalarReplacement(false); // much faster
  _yTickLabel->setDoScalarReplacement(false); // much faster
  _zoomPaused = false;
  setDirty(true);
  _oldSize.setWidth(0);
  _oldSize.setHeight(0);
  _hasFocus = false;
  _copy_x = _copy_y = KST::NOPOINT;
  _cursor_x = _cursor_y = KST::NOPOINT;
  _cursorOffset = false;
  _standardActions |= Delete | Edit | Zoom | Pause;
  _type = "Plot";
  _xLog = x_log;
  _yLog = y_log;
  _xLogBase = x_logbase;
  _yLogBase = y_logbase;
  _tickYLast = 0.0;
  _stLast = 0.0;
  _autoTickYLast = 0;
  _isLogLast = false;

  _i_per = 0;
  
  setTagName(KstObjectTag(in_tag, KstObjectTag::globalTagContext));  // FIXME: tag context
  _isTied = false;

  XMin = xmin_in;
  XMax = xmax_in;
  YMin = ymin_in;
  YMax = ymax_in;

  _xScaleMode = xscale_in;
  _yScaleMode = yscale_in;

  if (_xScaleMode == AUTO || _xScaleMode == AUTOBORDER) {
    _xScaleModeDefault = _xScaleMode;
  }
  if (_yScaleMode == AUTO || _yScaleMode == AUTOBORDER) {
    _yScaleModeDefault = _yScaleMode;
  }

  // verify that scale limits make sense.  If not, go to auto.
  if (XMax <= XMin) { // not OK: ignore request
    XMin = 0;
    XMax = 1;
    if (_xScaleMode != AUTOUP) {
      _xScaleMode = AUTO;
    }
  }
  if (YMax <= YMin) {
    YMin = 0;
    YMax = 1;
    if (_yScaleMode != AUTOUP) {
      _yScaleMode = AUTO;
    }
  }

  _plotScaleList.setAutoDelete(true);
  pushScale();

  // let this Kst2DPlot register doc changes.
  connect(this, SIGNAL(modified()), KstApp::inst(), SLOT(registerDocChange()));
}


Kst2DPlot::~Kst2DPlot() {
  delete _xLabel;
  _xLabel = 0L;
  delete _yLabel;
  _yLabel = 0L;
  delete _topLabel;
  _topLabel = 0L;
  delete _xTickLabel;
  _xTickLabel = 0L;
  delete _yTickLabel;
  _yTickLabel = 0L;
  delete _fullTickLabel;
  _fullTickLabel = 0L;

  _curveToMarkers = 0L;
  _vectorToMarkers = 0L;
}


bool Kst2DPlot::checkRange(double &min_in, double &max_in) {
  double diff  = fabs(1000.0 * min_in) * DBL_EPSILON;

  if (isnan(min_in) || isnan(max_in) || isinf(min_in) || isinf(max_in)) {
    return false;
  }

  if (max_in <= min_in + diff) {
    max_in = min_in + diff;
  }

  return true;
}


bool Kst2DPlot::checkLRange(double &min_in, double &max_in, bool bIsLog, double logBase) {
  bool rc = true;

  if (bIsLog) {
    if (isnan(pow(logBase, min_in)) || isnan(pow(logBase, max_in)) ||
        isinf(pow(logBase, min_in)) || isinf(pow(logBase, max_in))) {
      rc = false;
    }
  } else if (isnan(min_in) || isnan(max_in) ||
             isinf(min_in) || isinf(max_in)) {
    rc = false;
  }

  if (rc) {
    double diff = fabs(1000.0 * min_in) * DBL_EPSILON;

    if (max_in <= min_in + diff) {
      max_in = min_in + diff;
    }
  }

  return rc;
}


bool Kst2DPlot::setXScale(double xmin_in, double xmax_in) {
  if (checkRange(xmin_in, xmax_in)) {
    XMax = xmax_in;
    XMin = xmin_in;
    return true;
  }

  return false;
}


bool Kst2DPlot::setYScale(double ymin_in, double ymax_in) {
  if (checkRange(ymin_in, ymax_in)) {
    YMax = ymax_in;
    YMin = ymin_in;
    return true;
  }

  return false;
}


bool Kst2DPlot::setLXScale(double xmin_in, double xmax_in) {
  if (checkLRange(xmin_in, xmax_in, _xLog, _xLogBase)) {
    if (_xLog) {
      // don't do auto scale change... this is the wrong place to do it 
      // and it acts spooky!
/*      if (_xLogBase == 10.0 && xmax_in - xmin_in < 1.1) {
        _xLogBase = 2.0;
        xmin_in *= 1.0/log10(2.0);
        xmax_in *= 1.0/log10(2.0);
      } else if (_xLogBase == 2.0 && xmax_in - xmin_in > 1.5) {
        _xLogBase = 10.0;
        xmin_in *= log10(2.0);
        xmax_in *= log10(2.0);
      }*/
      XMax = pow(_xLogBase, xmax_in);
      XMin = pow(_xLogBase, xmin_in);
    } else {
      XMax = xmax_in;
      XMin = xmin_in;
    }
    return true;
  }

  return false;
}


bool Kst2DPlot::setLYScale(double ymin_in, double ymax_in) {
  if (checkLRange(ymin_in, ymax_in, _yLog, _yLogBase)) {
    if (_yLog) {
      // don't do auto scale change... this is the wrong place to do it 
      // and it acts spooky!
/*      if (_yLogBase == 10.0 && ymax_in - ymin_in < 2.0) {
        _yLogBase = 2.0;
      } else if (_yLogBase == 2.0 && ymax_in - ymin_in > 3.0) {
        _yLogBase = 10.0;
      }*/
      YMax = pow(_yLogBase, ymax_in);
      YMin = pow(_yLogBase, ymin_in);
    } else {
      YMax = ymax_in;
      YMin = ymin_in;
    }
    return true;
  }

  return false;
}


void Kst2DPlot::setScale(double xmin_in, double ymin_in, double xmax_in, double ymax_in) {
  setXScale(xmin_in, xmax_in);
  setYScale(ymin_in, ymax_in);
}


// Odd, should we be returning true if only one of these works?  I think this
// is a bit strange.
bool Kst2DPlot::setLScale(double xmin_in, double ymin_in, double xmax_in, double ymax_in) {
  bool rc = setLXScale(xmin_in, xmax_in);
  return setLYScale(ymin_in, ymax_in) || rc;
}


void Kst2DPlot::getScale(double& xmin, double& ymin, double& xmax, double& ymax) const {
  xmin = XMin;
  xmax = XMax;
  ymin = YMin;
  ymax = YMax;
}


void Kst2DPlot::getLScale(double& x_min, double& y_min, double& x_max, double& y_max) const {
  if (_xLog) {
    x_min = logXLo(XMin, _xLogBase);
    x_max = logXHi(XMax, _xLogBase);
  } else {
    x_max = XMax;
    x_min = XMin;
  }

  if (_yLog) {
    y_min = logYLo(YMin, _yLogBase);
    y_max = logYHi(YMax, _yLogBase);
  } else {
    y_max = YMax;
    y_min = YMin;
  }
}


KstScaleModeType Kst2DPlot::xScaleMode() const {
  return _xScaleMode;
}


KstScaleModeType Kst2DPlot::yScaleMode() const {
  return _yScaleMode;
}


void Kst2DPlot::setXScaleMode(KstScaleModeType scalemode_in) {
  if (scalemode_in == AUTO || scalemode_in == AUTOBORDER) {
    _xScaleModeDefault = scalemode_in;
  }
  _xScaleMode = scalemode_in;
}


void Kst2DPlot::setYScaleMode(KstScaleModeType scalemode_in) {
  if (scalemode_in == AUTO || scalemode_in == AUTOBORDER) {
    _yScaleModeDefault = scalemode_in;
  }
  _yScaleMode = scalemode_in;
}

bool Kst2DPlot::addCurve(KstBaseCurvePtr incurve) {
  if (!Curves.contains(incurve)) {
    Curves.append(incurve);
    setDirty();
    KstApp::inst()->document()->setModified();
    if (KstViewLegendPtr vl = legend()) {
      if (vl->trackContents()) {
        vl->addCurve(incurve);
      }
    }
    return true;
  }

  return false;
}


void Kst2DPlot::clearCurves() {
  if (!Curves.isEmpty()) {
    if (KstViewLegendPtr vl = legend()) {
      if (vl->trackContents()) {
        for (KstBaseCurveList::ConstIterator i = Curves.begin(); i != Curves.end(); ++i) {
          vl->removeCurve(*i);
        }
      }
    }
    Curves.clear();
    setDirty();
    KstApp::inst()->document()->setModified();
  }
}


void Kst2DPlot::fitCurve(int id) {
  KMdiChildView* c = KstApp::inst()->activeWindow();
  if (c) {
    KstBaseCurvePtr curve = *(Curves.findTag(_curveRemoveMap[id]));
    if (curve) {
      KstFitDialogI::globalInstance()->show_setCurve(_curveRemoveMap[id], tagName(), c->caption());
      if (_menuView) {
        _menuView->paint();
      }
    }
  }
}


void Kst2DPlot::filterCurve(int id) {
  KMdiChildView* c = KstApp::inst()->activeWindow();
  if (c) {
    KstBaseCurvePtr curve = *(Curves.findTag(_curveRemoveMap[id]));
    if (curve) {
      KstFilterDialogI::globalInstance()->show_setCurve(_curveRemoveMap[id], tagName(), c->caption());
      if (_menuView) {
        _menuView->paint();
      }
    }
  }
}


void Kst2DPlot::removeCurve(KstBaseCurvePtr incurve) {
  Curves.removeAll(incurve);
  KstViewLegendPtr vl = legend();
  if (vl) {
    if (vl->trackContents()) {
      vl->removeCurve(incurve);
    }
  }
  setDirty();
  KstApp::inst()->document()->setModified();
}


QPair<double, double> Kst2DPlot::computeAutoBorder(bool log, double logBase, double currentMin, double currentMax) {
  double min = currentMin, max = currentMax;
  if (log) {
    min = log10(min)/log10(logBase);
    max = max > 0.0 ? log10(max) : 0.0;
    double dx = (max - min) / 40.0;
    max = pow(logBase, max + dx);
    min = pow(logBase, min - dx);
  } else {
    double dx = (max - min) / 40.0;
    max += dx;
    min -= dx;
  }
  return qMakePair(min, max);
}


void Kst2DPlot::updateScale() {
  double mid, delta;
  bool first;
  int count;

  KstScaleModeType t = _xScaleMode;
  if (t == EXPRESSION && _xMinParsedValid && _xMaxParsedValid && _xMinParsed->isConst() && _xMaxParsed->isConst()) {
    t = FIXED;
  }

  switch (t) {
    case AUTOBORDER:  // set scale so all of all curves fits
    case AUTO:  // set scale so all of all curves fits
      XMin = 0.0;
      XMax = 1.0;
      first = true;

      {
        KstBaseCurveList cl = Q3DeepCopy<KstBaseCurveList>(Curves);
        qSort(cl);
        for (unsigned i = 0; i < cl.count(); i++) {
          KstBaseCurvePtr c = cl[i];
          c->readLock();
          if (!c->ignoreAutoScale()) {
            if (_xLog) {
              if (first || XMin > c->minPosX()) {
                XMin = c->minPosX();
              }
            } else {
              if (first || XMin > c->minX()) {
                XMin = c->minX();
              }
            }
            if (first || XMax < c->maxX()) {
              XMax = c->maxX();
            }
            first = false;
          }
          c->unlock();
        }
      }

      if (XMax <= XMin) {  // if curves had no variation in them
        XMin -= 0.1;
        XMax  = XMin + 0.2;
      }

      if (_xLog && XMin <= 0.0) {
        XMin = pow(_xLogBase, -350.0);
      }
      if (_xScaleMode == AUTOBORDER) {
        QPair<double, double> borders = computeAutoBorder(_xLog, _xLogBase, XMin, XMax);
        XMin = borders.first;
        XMax = borders.second;
      }
      break;

    case NOSPIKE:  // set scale so all of all curves fits
      XMin = 0.0;
      XMax = 1.0;
      first = true;

      for (unsigned i = 0; i < Curves.count(); i++) {
        KstBaseCurvePtr c = Curves[i];
        c->readLock();
        if (!c->ignoreAutoScale()) {
          if (_xLog) {
            if (first || XMin > c->minPosX()) {
              XMin = c->minPosX();
            }
          } else {
            if (first || XMin > c->ns_minX()) {
              XMin = c->ns_minX();
            }
          }
          if (first || XMax < c->ns_maxX()) {
            XMax = c->ns_maxX();
          }

          first = false;
        }
        c->unlock();
      }
      if (XMax <= XMin) {  // if curves had no variation in them
        XMin -= 0.1;
        XMax = XMin + 0.2;
      }

      if (_xLog && XMin < 0.0) {
        XMin = pow(_xLogBase, -350.0);
      }
      break;

    case AC: // keep range const, but set mid to mid of all curves
      first = true;
      count = 0;
      mid = 0.0;

      for (unsigned i = 0; i < Curves.count(); i++) {
        KstBaseCurvePtr c = Curves[i];
        c->readLock();
        if (!c->ignoreAutoScale()) {
          mid += c->midX();
          ++count;
        }
        c->unlock();
      }
      if (count > 0) {
        if (XMax <= XMin) { // make sure that range is legal
          XMin = 0.0;
          XMax = 1.0;
        }
        mid /= double(count);
        delta = XMax - XMin;
        XMin = mid - delta / 2.0;
        XMax = mid + delta / 2.0;
      } else {
        XMin = -0.5;
        XMax =  0.5;
      }
      break;

    case FIXED:  // don't change the range
      if (XMin > XMax) {  // has to be legal, even for fixed scale...
        double tmp = XMax;
        XMax = XMin;
        XMin = tmp;
      } else if (XMin == XMax) {
        if (XMin == 0.0) {
          XMin = -0.5;
          XMax =  0.5;
        } else {
          XMax += fabs(XMax) * 0.01;
          XMin -= fabs(XMin) * 0.01;
        }
      }
      break;

    case AUTOUP:  // only change up
      for (unsigned i = 0; i < Curves.count(); i++) {
        KstBaseCurvePtr c = Curves[i];
        c->readLock();
        if (!c->ignoreAutoScale()) {
          if (_xLog) {
            if (XMin > c->minPosX()) {
              XMin = c->minPosX();
            }
          } else {
            if (XMin > c->minX()) {
              XMin = c->minX();
            }
          }
          if (XMax < c->maxX()) {
            XMax = c->maxX();
          }
        }
        c->unlock();
      }

      if (_xLog && XMin < 0.0) {
        XMin = pow(_xLogBase, -350.0);
      }
      if (XMin > XMax) {
        double tmp = XMax;
        XMax = XMin;
        XMin = tmp;
      } else if (XMin == XMax) {
        XMax += fabs(XMax) * 0.01;
        XMin -= fabs(XMin) * 0.01;
      }
      break;

    case EXPRESSION:
      // reparse if necessary
      if (!_xMinParsedValid) {
        _xMinParsedValid = reparse(_xMinExp, &_xMinParsed);
      }
      if (!_xMaxParsedValid) {
        _xMaxParsedValid = reparse(_xMaxExp, &_xMaxParsed);
      }
      // get values from expressions
      {
        Equation::Context ctx;

        if (_xMinParsedValid) {
          _xMinParsed->update(-1, &ctx);
          XMin = _xMinParsed->value(&ctx);
        }
        if (_xMaxParsedValid) {
          _xMaxParsed->update(-1, &ctx);
          XMax = _xMaxParsed->value(&ctx);
        }
      }
      if (XMin > XMax) {
        double tmp = XMax;
        XMax = XMin;
        XMin = tmp;
      } else if (XMin == XMax) {
        if (XMin == 0.0) {
          XMin = -0.5;
          XMax =  0.5;
        } else {
          XMax += fabs(XMax) * 0.01;
          XMin -= fabs(XMin) * 0.01;
        }
      }
      break;

    default:
      kstdWarning() << "Bug in Kst2DPlot::updateScale: bad scale mode" << endl;
      break;
  }

  t = _yScaleMode;
  if (t == EXPRESSION && _yMinParsedValid && _yMaxParsedValid && _yMinParsed->isConst() && _yMaxParsed->isConst()) {
    t = FIXED;
  }

  switch (t) {
    case AUTOBORDER:  // set scale so all of all curves fits
    case AUTO:  // set scale so all of all curves fits
      YMin = 0.0;
      YMax = 1.0;
      first = true;

      for (unsigned i = 0; i < Curves.count(); i++) {
        KstBaseCurvePtr c = Curves[i];
        c->readLock();
        if (!c->ignoreAutoScale()) {
          if (_yLog) {
            if (first || YMin > c->minPosY()) {
              YMin = c->minPosY();
            }
          } else {
            if (first || YMin > c->minY()) {
              YMin = c->minY();
            }
          }
          if (first || YMax < c->maxY()) {
            YMax = c->maxY();
          }
          first = false;
        }
        c->unlock();
      }

      if (YMax <= YMin) {  // if curves had no variation in them
        YMin -= 0.1;
        YMax  = YMin + 0.2;
      }

      if (_yLog && YMin <= 0.0) {
        YMin = pow(_yLogBase, -350.0);
      }
      if (_yScaleMode == AUTOBORDER) {
        QPair<double, double> borders = computeAutoBorder(_yLog, _yLogBase, YMin, YMax);
        YMin = borders.first;
        YMax = borders.second;
      }

      break;

    case NOSPIKE:  // set scale so all of all curves fits
      YMin = 0.0;
      YMax = 1.0;
      first = true;

      for (unsigned i = 0; i < Curves.count(); i++) {
        KstBaseCurvePtr c = Curves[i];
        c->readLock();
        if (!c->ignoreAutoScale()) {
          if (_yLog) {
            if (first || YMin > c->minPosY()) {
              YMin = c->minPosY();
            }
          } else {
            if (first || YMin > c->ns_minY()) {
              YMin = c->ns_minY();
            }
          }
          if (first || YMax < c->ns_maxY()) {
            YMax = c->ns_maxY();
          }

          first = false;
        }
        c->unlock();
      }
      if (YMax <= YMin) {  // if curves had no variation in them
        YMin -= 0.1;
        YMax = YMin + 0.2;
      }

      if (_yLog && YMin <= 0.0) {
        YMin = pow(_yLogBase, -350.0);
      }
      break;

    case AC: // keep range const, but set mid to mean of all curves
      first = true;
      count = 0;
      mid = 0.0;

      for (unsigned i = 0; i < Curves.count(); i++) {
        KstBaseCurvePtr c = Curves[i];
        c->readLock();
        if (!c->ignoreAutoScale()) {
          mid += c->midY();
          ++count;
        }
        c->unlock();
      }
      if (count > 0) {
        if (YMax <= YMin) { // make sure that range is legal
          YMin = 0.0;
          YMax = 1.0;
        }
        mid /= (double)count;
        delta = YMax - YMin;
        YMin = mid - delta / 2.0;
        YMax = mid + delta / 2.0;
      } else {
        YMin = -0.5;
        YMax =  0.5;
      }
      break;

    case FIXED:  // don't change the range

      if (YMin > YMax) {  // has to be legal, even for fixed scale...
        double tmp = YMax;
        YMax = YMin;
        YMin = tmp;
      } else if (YMin == YMax) {
        if (YMin == 0.0) {
          YMin = -0.5;
          YMax =  0.5;
        } else {
          YMax += fabs(YMax) * 0.01;
          YMin -= fabs(YMin) * 0.01;
        }
      }
      break;

    case AUTOUP:  // only change up
      for (unsigned i = 0; i < Curves.count(); i++) {
        KstBaseCurvePtr c = Curves[i];
        c->readLock();
        if (!c->ignoreAutoScale()) {
          if (_yLog) {
            if (YMin > c->minPosY()) {
              YMin = c->minPosY();
            }
          } else {
            if (YMin > c->minY()) {
              YMin = c->minY();
            }
          }
          if (YMax < c->maxY()) {
            YMax = c->maxY();
          }
        }
        c->unlock();
      }

      if (_yLog && YMin <= 0.0) {
        YMin = pow(_yLogBase, -350.0);
      }
      if (YMin >= YMax) {  // has to be legal, even for autoup...
        if (YMax == 0.0) {
          YMin = -0.5;
          YMax =  0.5;
        } else {
          YMax += YMax * 0.01;
          YMin -= YMin * 0.01;
        }
      }
      break;

    case EXPRESSION:
      // reparse if necessary
      if (!_yMinParsedValid) {
        _yMinParsedValid = reparse(_yMinExp, &_yMinParsed);
      }
      if (!_yMaxParsedValid) {
        _yMaxParsedValid = reparse(_yMaxExp, &_yMaxParsed);
      }
      // get values from expressions
      {
        Equation::Context ctx;

        if (_yMinParsedValid) {
          _yMinParsed->update(-1, &ctx);
          YMin = _yMinParsed->value(&ctx);
        }
        if (_yMaxParsedValid) {
          _yMaxParsed->update(-1, &ctx);
          YMax = _yMaxParsed->value(&ctx);
        }
      }
      if (YMin > YMax) {
        double tmp = YMax;
        YMax = YMin;
        YMin = tmp;
      } else if (YMin == YMax) {
        if (YMin == 0.0) {
          YMin = -0.5;
          YMax =  0.5;
        } else {
          YMax += fabs(YMax) * 0.01;
          YMin -= fabs(YMin) * 0.01;
        }
      }
      break;

    default:
      kstdWarning() << "Bug in Kst2DPlot::updateScale: bad scale mode" << endl;
      break;
  }
}


double Kst2DPlot::convertTimeValueToJD(KstAxisInterpretation axisInterpretation, double valueIn) {
  double value = valueIn;

  switch (axisInterpretation) {
    case AXIS_INTERP_YEAR:
      value -= 1900.0;
      value *= 365.25;
      value += JD1900 + 0.5;
      break;
    case AXIS_INTERP_CTIME:
      value /= 24.0 * 60.0 * 60.0;
      value += JD1970;
      break;
    case AXIS_INTERP_JD:
      break;
    case AXIS_INTERP_MJD:
      value += JD_MJD;
      break;
    case AXIS_INTERP_RJD:
      value += JD_RJD;
      break;
    case AXIS_INTERP_AIT:
      value -= 86400.0 * (365.0 * 12.0 + 3.0);
      // current difference (seconds) between UTC and AIT
      // refer to the following for more information:
      // http://hpiers.obspm.fr/eop-pc/earthor/utc/TAI-UTC_tab.html
      value -= 32.0;
      value /= 24.0 * 60.0 * 60.0;
      value += JD1970;
    default:
      break;
  }

  return value;
}


double Kst2DPlot::convertTimeDiffValueToDays(KstAxisInterpretation axisInterpretation, double diffIn) {
  double diff = diffIn;

  switch (axisInterpretation) {
    case AXIS_INTERP_YEAR:
      diff *= 365.25;
      break;
    case AXIS_INTERP_CTIME:
      diff /= 24.0 * 60.0 * 60.0;
      break;
    case AXIS_INTERP_JD:
    case AXIS_INTERP_MJD:
    case AXIS_INTERP_RJD:
      break;
    case AXIS_INTERP_AIT:
      diff /= 24.0 * 60.0 * 60.0;
      break;
    default:
      break;
  }

  return diff;
}


void Kst2DPlot::convertJDToDateString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, uint& length, double dJD) {
  QDate date;
  QRect pr = GetPlotRegion();
  double xmin, ymin, xmax, ymax;
  double xdelta;
  int accuracy = 0;

  // check how many decimal places we need based on the scale
  getLScale(xmin, ymin, xmax, ymax);
  if (isXLog()) {
    xdelta = (pow(_xLogBase, xmax) - pow(_xLogBase, xmin))/double(pr.width());
  } else {
    xdelta = (xmax-xmin)/double(pr.width());
  }
  xdelta = convertTimeDiffValueToDays(axisInterpretation, xdelta);
  xdelta *= 24.0 * 60.0 * 60.0;

  if (xdelta == 0.0) {
    accuracy = FULL_PRECISION;
  } else {
    accuracy = 1 - int(log10(xdelta));
    if (accuracy < 0) {
      accuracy = 0;
    }
  }

  // utcOffset() is returned in seconds... as it must be since
  //  some time zones are not an integer number of hours offset
  //  from UTC...
  dJD += double(KstSettings::globalSettings()->utcOffset()) / 86400.0;

  length = 0;

  // get the date from the Julian day number
  double dJDDay = floor(dJD);
  double dJDFraction = dJD - dJDDay;

  // gregorian calendar correction
  if (dJD >= 2299160.5) {
    double tmp = int( ( (dJDDay - 1867216.0) - 0.25 ) / 36524.25 );
    dJDDay += 1.0 + tmp - floor(0.25*tmp);
  }

  // correction for half day offset
  double dDayFraction = dJDFraction + 0.5;
  if (dDayFraction >= 1.0) {
    dDayFraction -= 1.0;
    dJDDay += 1.0;
  }

  // get time of day from day fraction
  int hour   = int(dDayFraction*24.0);
  int minute = int((dDayFraction*24.0 - double(hour))*60.0);
  double second = ((dDayFraction*24.0 - double(hour))*60.0 - double(minute))*60.0;

  if (accuracy >= 0) {
    second *= pow(10.0, accuracy);
    second  = floor(second+0.5);
    second /= pow(10.0,accuracy);
    if (second >= 60.0) {
      second -= 60.0;
      minute++;
      if (minute == 60) {
        minute = 0;
        hour++;
        if (hour == 24) {
          hour = 0;
        }
      }
    }
  }

  double j2 = dJDDay + 1524.0;
  double j3 = floor(6680.0 + ( (j2 - 2439870.0) - 122.1 )/365.25);
  double j4 = floor(j3 * 365.25);
  double j5 = floor((j2 - j4)/30.6001);

  int day = int(j2 - j4 - floor(j5*30.6001));
  int month = int(j5 - 1.0);
  if (month > 12) {
    month -= 12;
  }
  int year = int(j3 - 4715.0);
  if (month > 2) {
    --year;
  }
  if (year <= 0) {
    --year;
  }

  // check how many decimal places for the seconds we actually need to show
  if (accuracy > 0) {
    QString strSecond;

    strSecond.sprintf("%02.*f", accuracy, second);
    for (int i=strSecond.length()-1; i>0; i--) {
      if (strSecond.at(i) == '0') {
        accuracy--;
      } else if (!strSecond.at(i).isDigit()) {
        break;
      }
    }
  }

  if (accuracy < 0) {
    accuracy = 0;
  }

  if (accuracy == 0) {
    if (int(second) % 10 != 0) {
      length += 6;
    }
    else if (int(second) != 0) {
      length += 5;
    }
    else if (minute % 10 != 0) {
      length += 4;
    }
    else if (minute != 0) {
      length += 3;
    }
    else if (hour % 10 != 0) {
      length += 2;
    }
    else if (hour != 0) {
      length += 1;
    }
  } else {
    length += 6;
    length += accuracy;
  }

  switch (axisDisplay) {
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
      label.sprintf("%d/%02d/%02d %02d:%02d:%02.*f", year, month, day, hour, minute, accuracy, second);
      break;
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
      label.sprintf("%02d/%02d/%d %02d:%02d:%02.*f", day, month, year, hour, minute, accuracy, second);
      break;
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
      date.setYMD(year, month, day);
      label.sprintf("%s %02d:%02d:%02.*f", date.toString(Qt::TextDate).toAscii(),
                                            hour, minute, accuracy, second);
      break;
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      date.setYMD(year, month, day);
      label.sprintf("%s %02d:%02d:%02.*f", date.toString(Qt::LocalDate).toAscii(),
                                            hour, minute, accuracy, second);
      break;
    case AXIS_DISPLAY_KDE_SHORTDATE:
      label = KGlobal::locale()->formatDateTime(
                                               QDateTime(QDate(year, month, day),
                                               QTime(hour, minute, (int)second)), true, true);
      // handle the fractional seconds
      if (accuracy > 0) {
        QString strSecond;

        strSecond.sprintf(" + %0.*f seconds", accuracy, second - floor(second));
        label += strSecond;
      }
      break;
    case AXIS_DISPLAY_KDE_LONGDATE:
      label = KGlobal::locale()->formatDateTime(
                                               QDateTime(QDate(year, month, day),
                                               QTime(hour, minute, (int)second)), false, true);
      // handle the fractional seconds
      if (accuracy > 0) {
        QString strSecond;

        strSecond.sprintf(" + %0.*f seconds", accuracy, second - floor(second));
        label += strSecond;
      }
      break;
    default:
      break;
  }
}


void Kst2DPlot::convertTimeValueToString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, uint& length, double z, bool isLog, double logBase) {
  double value;

  if (isLog) {
    value = pow(logBase, z);
  } else {
    value = z;
  }

  value = convertTimeValueToJD(axisInterpretation, value);

  // print value in appropriate format
  switch (axisDisplay) {
    case AXIS_DISPLAY_YEAR:
      value -= JD1900 + 0.5;
      value /= 365.25;
      value += 1900.0;
      label = QString::number(value, 'g', FULL_PRECISION);
      length = label.length();
      break;
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
    case AXIS_DISPLAY_KDE_SHORTDATE:
    case AXIS_DISPLAY_KDE_LONGDATE:
      convertJDToDateString(axisInterpretation, axisDisplay, label, length, value);
      break;
    case AXIS_DISPLAY_JD:
      label = QString::number(value, 'g', FULL_PRECISION);
      length = label.length();
      break;
    case AXIS_DISPLAY_MJD:
      value -= JD_MJD;
      label = QString::number(value, 'g', FULL_PRECISION);
      length = label.length();
      break;
    case AXIS_DISPLAY_RJD:
      value -= JD_RJD;
      label = QString::number(value, 'g', FULL_PRECISION);
      length = label.length();
      break;
  }
}


void Kst2DPlot::convertDiffTimevalueToString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double& zdiff, double zbase, double zvalue, bool isLog, double scale) {
  if (isLog) {
    zdiff = pow(10.0, zvalue) - pow(10.0, zbase);
  } else {
    zdiff = zvalue - zbase;
  }

  zdiff = convertTimeDiffValueToDays(axisInterpretation, zdiff);

  // convert difference to desired format
  switch (axisDisplay) {
    case AXIS_DISPLAY_YEAR:
      zdiff /= 365.25;
      break;
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
    case AXIS_DISPLAY_KDE_SHORTDATE:
    case AXIS_DISPLAY_KDE_LONGDATE:
      zdiff *= 24.0 * 60.0 * 60.0;
      zdiff *= scale;
      break;
    case AXIS_DISPLAY_JD:
    case AXIS_DISPLAY_MJD:
    case AXIS_DISPLAY_RJD:
      break;
  }
}


void Kst2DPlot::genAxisTickLabelFullPrecision(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, uint& length, double z, bool isLog, double logBase, bool isInterpreted) {
  if (isInterpreted) {
    convertTimeValueToString(axisInterpretation, axisDisplay, label, length, z, isLog, logBase);
  } else if (isLog) {
    label = QString::number(pow(logBase, z), 'g', FULL_PRECISION);
    length = label.length();
  } else {
    label = QString::number(z, 'g', FULL_PRECISION);
    length = label.length();
  }
}


void Kst2DPlot::genAxisTickLabelDifference(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, double zbase, double zvalue, bool isLog, double logBase, bool isInterpreted, double scale) {
  double zdiff;

  if (isInterpreted) {
    convertDiffTimevalueToString(axisInterpretation, axisDisplay, zdiff, zbase, zvalue, isLog, scale);
  } else {
    if (isLog) {
      zdiff = pow(logBase, zvalue) - pow(logBase, zbase);
    } else {
      zdiff = zvalue - zbase;
    }
  }

  if (zdiff > 0.0) {
    label = i18nc("+[numeric value]", "+[%1]").arg(zdiff, 0, 'g', DIFFERENCE_PRECISION);
  } else if (zdiff < 0.0) {
    label = i18nc("-[numeric value]", "-[%1]").arg(-zdiff, 0, 'g', DIFFERENCE_PRECISION);
  } else {
    label = i18n("[0]");
  }
}


void Kst2DPlot::genOffsetLabel(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, QString& label, double zbase, double zvalue, double Min, double Max, bool isLog, double logBase, bool isInterpreted) {
  double zdiff;

  if (isInterpreted) {
    QString strPrefix;
    QString strUnits;
    double scale;
    double range;
    int base;

    if (isLog) {
      zbase = log10(zbase);
    }
    getPrefixUnitsScale(isInterpreted, axisInterpretation, axisDisplay, isLog, logBase, Min, Max, range, scale, base, strPrefix, strUnits);
    convertDiffTimevalueToString(axisInterpretation, axisDisplay, zdiff, zbase, zvalue, isLog, scale);
    label = QString("%1 %2").arg(zdiff, 0, 'g', DIFFERENCE_PRECISION).arg(strUnits);
  } else {
    if (isLog) {
      zdiff = pow(logBase, zvalue) - pow(logBase, zbase);
    } else {
      zdiff = zvalue - zbase;
    }
    label = QString("%1").arg(zdiff, 0, 'g', DIFFERENCE_PRECISION);
  }
}


void Kst2DPlot::genAxisTickLabel(QString& label, double z, bool isLog, double logBase) {
  if (isLog) {
    if (z > -4 && z < 4) {
      label = QString::number(pow(logBase, z), 'g', LABEL_PRECISION);
    } else {
      label = i18nc("%2 to the power of %1", "%2^{%1}").arg(z, 0, 'f', 0).arg(logBase, 0, 'f', 0);
    }
  } else {
    label = QString::number(z, 'g', LABEL_PRECISION);
  }
}


void Kst2DPlot::getPrefixUnitsScale(bool isInterpreted, KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, bool bLog, double logBase, double Min, double Max, double& range, double& scale, int& base, QString& strPrefix, QString& strUnits) {
  range = 1.0;
  scale = 1.0;
  base = 10;

  if (isInterpreted) {
    // convert time range to seconds
    switch (axisInterpretation) {
      case AXIS_INTERP_YEAR:
        range *= 365.25 * 24.0 * 60.0 * 60.0;
        break;
      case AXIS_INTERP_CTIME:
        break;
      case AXIS_INTERP_JD:
      case AXIS_INTERP_MJD:
      case AXIS_INTERP_RJD:
        range *= 24.0 * 60.0 * 60.0;
        break;
      case AXIS_INTERP_AIT:
        break;
    }

    switch (axisDisplay) {
      case AXIS_DISPLAY_YEAR:
        strPrefix = i18nc("Prefix for Julian Year", "J");
        strUnits  = i18n("years");
        scale /= 365.25 * 24.0 * 60.0 * 60.0;
        break;
      case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
      case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
      case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
      case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      case AXIS_DISPLAY_KDE_SHORTDATE:
      case AXIS_DISPLAY_KDE_LONGDATE:
        double value;
        
        if( bLog ) {
          value = ( pow( logBase, Max ) - pow( logBase, Min ) ) * range;
        } else {
          value = ( Max - Min ) * range;
        }
        if( value > 10.0 * 24.0 * 60.0 * 60.0 ) {
          scale /= 24.0 * 60.0 * 60.0;
          strUnits  = i18n("days");
        } else if( value > 10.0 * 24.0 * 60.0 ) {
          scale /= 60.0 * 60.0;
          strUnits  = i18n("hours");
          base = 24;
        } else if( value > 10.0 * 60.0 ) {
          scale /= 60.0;
          strUnits  = i18n("minutes");
          base = 60;
        } else {
          strUnits  = i18n("seconds");
          base = 60;
        }
        break;
      case AXIS_DISPLAY_JD:
        strPrefix = i18nc("Prefix for Julian Date", "JD");
        strUnits  = i18n("days");
        scale /= 24.0 * 60.0 * 60.0;
        break;
      case AXIS_DISPLAY_MJD:
        strPrefix = i18nc("Prefix for Modified Julian Date", "MJD");
        strUnits  = i18n("days");
        scale /= 24.0 * 60.0 * 60.0;
        break;
      case AXIS_DISPLAY_RJD:
        strPrefix = i18nc("Prefix for Reduced Julian Date", "RJD");
        strUnits  = i18n("days");
        scale /= 24.0 * 60.0 * 60.0;
        break;
    }
  }
}


void Kst2DPlot::genAxisTickLabels(TickParameters &tp,
                                  double Min, double Max, bool isLog, double logBase,
                                  KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay,
                                  bool isX, bool isInterpreted, bool isOffsetMode) {
  QString strTmp;
  QString strTmpOld;
  QString strUnits;
  QString strPrefix;
  double range;
  double scale = 1.0;
  bool bDuplicate = false;
  uint uiShortestLength = 1000;
  uint length;
  int iShort = 0;
  int base = 60;
  KstPlotLabel *tick_label;

  if (isX) {
    tick_label = _xTickLabel;
  } else {
    tick_label = _yTickLabel;
  }

  getPrefixUnitsScale(isInterpreted, axisInterpretation, axisDisplay, isLog, logBase, Min, Max, range, scale, base, strPrefix, strUnits);

  tp.maxWidth = 0.0;
  tp.maxHeight = 0.0;
  tp.oppMaxWidth = 0.0;
  tp.oppMaxHeight = 0.0;
  tp.labels.clear();
  tp.oppLabels.clear();
  tp.delta = false;
  
  if (isLog && isInterpreted) {
    setTicks(tp.tick, tp.org, Max + log10(range) + log10(scale), Min + log10(range) + log10(scale), isLog, logBase, isX, base);
    tp.org -= log10(range) + log10(scale);
  } else {
    setTicks(tp.tick, tp.org, Max*range*scale, Min*range*scale, isLog, logBase, isX, base);
    tp.tick /= range*scale;
    tp.org  /= range*scale;
  }
    
  tp.iLo = int((Min-tp.org)/tp.tick);
  tp.iHi = int((Max-tp.org)/tp.tick)+1;
  iShort = tp.iLo;

  // determine the values, and determine if we need to use delta values
  for (int i = tp.iLo; i < tp.iHi; i++) {
    genAxisTickLabel(strTmp, (double)i * tp.tick + tp.org, isLog, logBase);
    tick_label->setText(strTmp);
    QSize lsize = tick_label->size();

    tp.maxWidth = qMax(tp.maxWidth, double(lsize.width()));
    tp.maxHeight = qMax(tp.maxHeight, double(lsize.height()));
    if (strTmp == strTmpOld) {
      bDuplicate = true;
    } else {
      strTmpOld = strTmp;
    }
    tp.labels.append(strTmp);

    genAxisTickLabelFullPrecision(axisInterpretation, axisDisplay, strTmp, length, (double)i * tp.tick + tp.org, isLog, logBase, isInterpreted);
    if (length < uiShortestLength) {
      iShort = i;
      uiShortestLength = length;
    }
  }

  // also generate labels for opposite axis if needed
  if ((isX && _xTransformed) || (!isX && _yTransformed)) {
    for (int i = tp.iLo; i < tp.iHi; i++) {
      // original tick number
      double originalNumber = (double)i * tp.tick + tp.org;
      if (isLog) {
        originalNumber = pow(logBase, originalNumber);
      }
      // case insensitive replace
      QString replacedExp = isX ? _xTransformedExp : _yTransformedExp;
      replacedExp.replace(isX ? "x" : "y", QString::number(originalNumber), Qt::CaseInsensitive);
      bool transformedOK = false;
      double transformedNumber = Equation::interpret(replacedExp.toLatin1(), &transformedOK, replacedExp.length());
      tick_label->setText(QString::number(transformedNumber, 'g', LABEL_PRECISION));
      if (!transformedOK) {
        tick_label->setText("NaN");
      }
      tp.oppLabels.append(tick_label->text());

      // update the max height and width of opposite labels
      QSize lsize = tick_label->size();
      tp.oppMaxWidth = qMax(tp.oppMaxWidth, double(lsize.width()));
      tp.oppMaxHeight = qMax(tp.oppMaxHeight, double(lsize.height()));
    }
  }

  // determine the values when using delta values.
  if (bDuplicate || isInterpreted || isOffsetMode) {
    tp.maxWidth = 0.0;
    tp.maxHeight = 0.0;
    tp.delta = true;
    tp.labels.clear();
    for (int i = tp.iLo; i < tp.iHi; i++) {
      if (i == iShort) {
        genAxisTickLabelFullPrecision(axisInterpretation, axisDisplay, strTmp, length, (double)iShort * tp.tick + tp.org, isLog, logBase, isInterpreted);
        if (!strUnits.isEmpty()) {
          strTmp = i18nc( "<Prefix e.g. JD><Value> [Units]", "%1%2 [%3]" ).arg(strPrefix).arg(strTmp).arg(strUnits);
        }
        tp.labels.prepend(strTmp);
      }
      if (isLog) {
        genAxisTickLabelDifference(axisInterpretation, axisDisplay, strTmp,
                                  (double)iShort * tp.tick + tp.org, (double)i * tp.tick + tp.org,
                                  isLog, logBase, isInterpreted, scale);
      } else {
        genAxisTickLabelDifference(axisInterpretation, axisDisplay, strTmp, (double)iShort * tp.tick,
                                  (double)i * tp.tick, isLog, logBase, isInterpreted, scale);
      }
      tp.labels.append(strTmp);
      tick_label->setText(strTmp);
      QSize lsize = tick_label->size();
      tp.maxWidth = qMax(tp.maxWidth, double(lsize.width()));
      tp.maxHeight = qMax(tp.maxHeight, double(lsize.height()));
    }
  }
}


void Kst2DPlot::internalAlignment(KstPainter& p, QRect& plotRegion) {
  TickParameters tpx;
  TickParameters tpy;
  double x_min, x_max, y_min, y_max;
  double xleft_bdr_px, xright_bdr_px, ytop_bdr_px, ybot_bdr_px;
  bool offsetX, offsetY;
  double xtick_len_px, ytick_len_px;
  int x_px, y_px;

  // resize labels based on window size.
  x_px = geometry().width();
  y_px = geometry().height();
  _xLabel->updateAbsFontSize(x_px, y_px);
  _yLabel->updateAbsFontSize(x_px, y_px);
  _topLabel->updateAbsFontSize(x_px, y_px);
  _xTickLabel->updateAbsFontSize(x_px, y_px);
  _yTickLabel->updateAbsFontSize(x_px, y_px);
  _fullTickLabel->updateAbsFontSize(x_px, y_px);

  x_px = p.window().width();
  y_px = p.window().height();

  p.save();
  p.setWindow(0, 0, size().width(), size().height());
  updateScale();
  getLScale(x_min, y_min, x_max, y_max);
  set2dPlotTickPix(xtick_len_px, ytick_len_px, x_px, y_px);

  setBorders(xleft_bdr_px, xright_bdr_px, ytop_bdr_px, ybot_bdr_px,
             tpx, tpy, p, offsetX, offsetY, xtick_len_px, ytick_len_px);

  p.restore();
  plotRegion.setLeft( d2i(xleft_bdr_px) );
  plotRegion.setRight( d2i(xright_bdr_px) );
  plotRegion.setTop( d2i(ytop_bdr_px) );
  plotRegion.setBottom( d2i(ybot_bdr_px) );
}


void Kst2DPlot::set2dPlotTickPix(double& xtickpix, double& ytickpix, int x_pix, int y_pix) {
  // set tick size: 4 points on a full letter size plot
  if (x_pix < y_pix) {
    xtickpix = 4.0 * x_pix / 540.0;
    ytickpix = 4.0 * y_pix / 748.0;
  } else {
    ytickpix = 4.0 * y_pix / 540.0;
    xtickpix = 4.0 * x_pix / 748.0;
  }
  xtickpix = (xtickpix + ytickpix) / 2.0; // average of x and y scaling
  if (xtickpix < 2.0) {
    xtickpix = 2.0; // but at least 2 pixels
  }
  ytickpix = xtickpix;
}


void Kst2DPlot::setBorders(double& xleft_bdr_px, double& xright_bdr_px,
                         double& ytop_bdr_px, double& ybot_bdr_px,
                         TickParameters &tpx,  TickParameters &tpy,
                         QPainter& p, bool& bOffsetX, bool& bOffsetY,
                          double xtick_len_px, double ytick_len_px) {
#ifdef BENCHMARK
  QTime t;
  t.start();
#endif
  double x_min, y_min, x_max, y_max;
  QRect v = p.window();
  int x_px = v.width();
  int y_px = v.height();

  bOffsetX = false;
  bOffsetY = false;

  getLScale(x_min, y_min, x_max, y_max);
  genAxisTickLabels(tpx, x_min, x_max, _xLog, _xLogBase, _xAxisInterpretation, _xAxisDisplay, true, _isXAxisInterpreted, _xOffsetMode);

  // calculate the top border
  if (_suppressTop) {
    ytop_bdr_px = 0.0;
  } else {
    ytop_bdr_px = 1.1 * _topLabel->size().height(); // height include super/subscripts

    // if top is transformed, leave space for the numbers
    if (_xTransformed) {
      ytop_bdr_px += tpx.oppMaxHeight;
    }

    if (ytop_bdr_px < 1.0) {
      ytop_bdr_px = 0.5 * _xTickLabel->lineSpacing();
    }
  }

  // calculate the bottom border
  if (_suppressBottom) {
    ybot_bdr_px = 0.0;
  } else {
    ybot_bdr_px  = tpx.maxHeight;
    ybot_bdr_px += _xLabel->lineSpacing();
  }

  // calculate the left border
  genAxisTickLabels(tpy, y_min, y_max, _yLog, _yLogBase, _yAxisInterpretation, _yAxisDisplay, false, _isYAxisInterpreted, _yOffsetMode);

  if (_suppressLeft) {
    xleft_bdr_px = 0.0;
  } else {
    xleft_bdr_px  = tpy.maxWidth;
    xleft_bdr_px += 5.0 * _yLabel->lineSpacing() / 4.0;
    xleft_bdr_px += _yTickLabel->lineSpacing() / 4.0;
    //printf("mw: %g yls: %d lfls: %d xlbp: %g\n", tpy.maxWidth, _yLabel->lineSpacing(), _tickLabel->lineSpacing(), xleft_bdr_px);
  }

  // calculate the right border
  if (_suppressRight) {
    xright_bdr_px = 0.0;
  } else {
    xright_bdr_px = x_px / 30.0;
    // if right axis is transformed, leave space for the numbers
    if (_yTransformed) {
      xright_bdr_px += tpy.oppMaxWidth;
    }
  }

  int tpxLabelCount = tpx.labels.count();
  int tpyLabelCount = tpy.labels.count();
  int xFullTickLabelLineSpacing = 0;
  int yFullTickLabelLineSpacing = 0;
  int xFullTickLabelWidth = 0;
  int yFullTickLabelWidth = 0;
  int xLabelWidth = 0;
  int yLabelHeight = 0;

  _fullTickLabel->setRotation(0);

  if (tpx.delta && tpxLabelCount) {
    _fullTickLabel->setText(tpx.labels[0]);
    xFullTickLabelWidth = _fullTickLabel->size().width();
    xFullTickLabelLineSpacing = _fullTickLabel->lineSpacing();
    xLabelWidth = _xLabel->size().width();

  }
  if (tpy.delta && tpyLabelCount > 0) {
    _fullTickLabel->setText(tpy.labels[0]);
    yFullTickLabelWidth = _fullTickLabel->size().width();
    yFullTickLabelLineSpacing = _fullTickLabel->lineSpacing();
    yLabelHeight = _yLabel->size().height();
  }

  // calculate offsets if we are using delta values
  if (tpx.delta && tpxLabelCount > 0 && tpy.delta && tpyLabelCount > 0) {
    if (xFullTickLabelWidth + xleft_bdr_px + xFullTickLabelLineSpacing >= (x_px - xLabelWidth)/2.0) {
      ybot_bdr_px += xFullTickLabelLineSpacing;
      bOffsetX = true;
    } else if (xFullTickLabelLineSpacing > _xLabel->lineSpacing()) {
      ybot_bdr_px += xFullTickLabelLineSpacing - _xLabel->lineSpacing();
    }
    if (yFullTickLabelWidth + ybot_bdr_px >= (y_px - yLabelHeight)/2.0) {
      xleft_bdr_px += yFullTickLabelLineSpacing;
      bOffsetY = true;
    } else if (yFullTickLabelLineSpacing > _yLabel->lineSpacing()) {
      xleft_bdr_px += yFullTickLabelLineSpacing - _yLabel->lineSpacing();
    }
  } else if (tpx.delta && tpxLabelCount > 0) {
    if (xFullTickLabelWidth + xleft_bdr_px >= (x_px - xLabelWidth)/2.0) {
      ybot_bdr_px += xFullTickLabelLineSpacing;
      bOffsetX = true;
    } else if (xFullTickLabelLineSpacing > _xLabel->lineSpacing()) {
      ybot_bdr_px += xFullTickLabelLineSpacing - _xLabel->lineSpacing();
    }
  } else if (tpy.delta && tpyLabelCount > 0) {
    if (yFullTickLabelWidth + ybot_bdr_px >= (y_px - yLabelHeight)/2.0) {
      xleft_bdr_px += yFullTickLabelLineSpacing;
      bOffsetY = true;
    } else if (yFullTickLabelLineSpacing > _yLabel->lineSpacing()) {
      xleft_bdr_px += yFullTickLabelLineSpacing - _yLabel->lineSpacing();
    }
  }

  // add additional spacing if ticks are outside plot
  if (xTicksOutPlot()) {
    ytop_bdr_px += 2.0 * xtick_len_px;
    ybot_bdr_px += 2.0 * xtick_len_px;
  }
  if (yTicksOutPlot()) {
    xleft_bdr_px += 2.0 * ytick_len_px;
    xright_bdr_px += 2.0 * ytick_len_px;
  }

  if (_suppressLeft) {
    xleft_bdr_px = 0.0;
  }
  if (_suppressRight) {
    xright_bdr_px = 0.0;
  }
  if (_suppressTop) {
    ytop_bdr_px = 0.0;
  }
  if (_suppressBottom) {
    ybot_bdr_px = 0.0;
  }

  // round off all the border values
  xleft_bdr_px  = ceil(xleft_bdr_px);
  xright_bdr_px = ceil(xright_bdr_px);
  ytop_bdr_px   = ceil(ytop_bdr_px);
  ybot_bdr_px   = ceil(ybot_bdr_px);
#ifdef BENCHMARK
  qDebug() << "SET BORDERS CALLED on object " << tagName() << ", took " << t.elapsed() << endl;
#endif
}


void Kst2DPlot::drawGraphicSelectionAt(QPainter& p, const QPoint& pos) {
  if (PlotRegion.contains(pos)) {
    p.setCompositionMode(QPainter::CompositionMode_Xor);
    p.setPen(QPen(QColor("gray"), 1));
    p.drawRect(pos.x() - 2, pos.y() - 2, 4, 4);
    p.setCompositionMode(QPainter::CompositionMode_Source);
  }
}


void Kst2DPlot::drawDotAt(QPainter& p, double x, double y) {
  if (_xLog) {
    x = logXLo(x, _xLogBase);
  }
  if (_yLog) {
    y = logYLo(y, _yLogBase);
  }

  int X1 = d2i(_m_X * x + _b_X) + position().x();
  int Y1 = d2i(_m_Y * y + _b_Y) + position().y();

  if (_xReversed) {
    X1 = PlotRegion.right() - (X1 - PlotRegion.left());
  }

  if (_yReversed) {
    Y1 = PlotRegion.bottom() - (Y1 - PlotRegion.top());
  }

  if (PlotRegion.contains(X1, Y1)) {
    p.setCompositionMode(QPainter::CompositionMode_Xor);
    p.setPen(QPen(QColor("gray"), 1));
    p.drawEllipse(X1 - 3, Y1 - 3, 6, 6);
    p.setCompositionMode(QPainter::CompositionMode_Source);
  }
}


void Kst2DPlot::drawPlusAt(QPainter& p, double x, double y) {
  if (_xLog) {
    x = logXLo(x);
  }
  if (_yLog) {
    y = logYLo(y);
  }

  int X1 = d2i(_m_X * x + _b_X) + position().x();
  int Y1 = d2i(_m_Y * y + _b_Y) + position().y();

  if (PlotRegion.contains(X1, Y1)) {
    p.setCompositionMode(QPainter::CompositionMode_Xor);
    p.setPen(QPen(QColor("gray"), 1));
    p.drawLine(X1 - 3, Y1, X1 + 4, Y1);
    p.drawLine(X1, Y1 - 3, X1, Y1 + 4);
    p.setCompositionMode(QPainter::CompositionMode_Source);
  }
}


void Kst2DPlot::edit() {
  KstTopLevelViewPtr tlv = kst_cast<KstTopLevelView>(topLevelParent());
  showDialog(tlv, false);
}


void Kst2DPlot::move(const QPoint& pos) {
  QPoint offset = pos - _geom.topLeft();

  PlotRegion.translate(offset.x(), offset.y());
  WinRegion.translate(offset.x(), offset.y());
  PlotAndAxisRegion.translate(offset.x(), offset.y());

  KstPlotBase::move(pos);
}


void Kst2DPlot::parentResized() {
  KstPlotBase::parentResized();
  setDirty();
}


void Kst2DPlot::parentMoved(const QPoint& offset) {
  PlotRegion.translate(offset.x(), offset.y());
  WinRegion.translate(offset.x(), offset.y());
  PlotAndAxisRegion.translate(offset.x(), offset.y());

  KstPlotBase::parentMoved(offset);
}


void Kst2DPlot::resize(const QSize& size) {
  // FIXME
  // Horribly inefficient, but we need to update contentsRect() somehow
  // before the base class resize happens.
  _buffer.buffer() = QPixmap(size);
  assert(!_buffer.buffer().isNull()); // Want to find these crashes
  if (!_buffer.buffer().isNull()) {    // Because this is garbage
    _buffer.buffer().fill(backgroundColor());
    KstPainter p;
    p.begin(&_buffer.buffer());
    p.setWindow(0, 0, size.width(), size.height());
    draw(p);
    p.end();
  }
  KstPlotBase::resize(size);
  setDirty(false);
}


void Kst2DPlot::updateTieBox(QPainter& p) {
  QRect tr = GetTieBoxRegion();
  QColor fillColor;
  if (isTied()) {
    fillColor.setRgb((foregroundColor().red() + backgroundColor().red()) / 2,
                     (foregroundColor().green() + backgroundColor().green()) / 2,
                     (foregroundColor().blue() + backgroundColor().blue()) / 2);
  } else {
    fillColor = backgroundColor();
  }
  p.setPen(foregroundColor());
  p.setBrush(fillColor);
  p.drawRect(tr);
  if (_hasFocus) {
    tr.setSize(tr.size() / 2);
    tr.moveTopLeft(tr.topLeft() + QPoint(3*tr.width()/4, 3*tr.height()/4));
    p.fillRect(tr, foregroundColor());
  }
}


void Kst2DPlot::updateDirtyFromLabels() {
#if 0
  bool dirty = this->dirty();
  // FIXME: dirty for xlabel, ylabel, toplabel
  // don't know what there is to fix... plotlabels no longer cache (and don't need to).
  //dirty = dirty || XLabel->dirty() || YLabel->dirty() || TopLabel->dirty();
  XLabel->setDirty(false);
  YLabel->setDirty(false);
  TopLabel->setDirty(false);
  setDirty(dirty);
#endif
}


void Kst2DPlot::updateSelf() {
  bool wasDirty(dirty());
  KstPlotBase::updateSelf();
  const QSize sizeNew(size());
  const QRect alignment(KST::alignment.limits(geometry()));
  updateDirtyFromLabels();
  if (wasDirty || sizeNew != _oldSize || alignment != _oldAlignment) {
    forEachChild(&KstViewObject::setDirty, true); // FIXME: hack,remove
    draw();
    recursively(&KstViewObject::updateFromAspect); // alignment may have changed
  }
  _oldAlignment = alignment;
  _oldSize = sizeNew;
}


void Kst2DPlot::paintSelf(KstPainter& p, const QRegion& bounds) {
  if (p.type() == KstPainter::P_EXPORT || p.type() == KstPainter::P_PRINT) {
    p.save();
    p.translate(geometry().left(), geometry().top());
    draw(p);
    p.restore();
    KstPlotBase::paintSelf(p, bounds);
  } else {
    if (_zoomPaused) { // eventually eliminate this and put zoom rect in here
      return;
    }

    if (p.makingMask()) {
      p.setCompositionMode(QPainter::CompositionMode_Source);
      KstPlotBase::paintSelf(p, bounds);
    } else {
      const QRegion clip(clipRegion());
      KstPlotBase::paintSelf(p, bounds - clip);
      p.setClipRegion(bounds & clip);
    }

    //qDebug() << " -> paint plot " << tagName() << " with clipRegion: " << p.clipRegion() << endl;
    _buffer.paintInto(p, geometry());
    drawCursorPos(p);
    updateTieBox(p);

    KstViewWidget *view = dynamic_cast<KstViewWidget*>(p.device());
    if (view) {
      _copy_x = _copy_y = KST::NOPOINT;
      if (GetPlotRegion().contains(_mouse.tracker)) {
        updateMousePos(_mouse.tracker);
        if (KstApp::inst()->dataMode()) {
          highlightNearestDataPoint(false, &p, _mouse.tracker);
        }
      }
    }
    
    KstMouseModeType gzType = globalZoomType();
    if (view && GetPlotRegion().contains(_mouse.tracker)) {
      if (gzType == X_ZOOMBOX || gzType == Y_ZOOMBOX) {
        updateXYGuideline(view, QPoint(-1, -1), view->mapFromGlobal(QCursor::pos()), GetPlotRegion(), _mouse.mode);
      } else if (gzType == XY_ZOOMBOX) {
        updateXYGuideline(view, QPoint(-1, -1), view->mapFromGlobal(QCursor::pos()), GetPlotRegion(), _mouse.mode);
      } else {
        _mouse.lastGuideline = QPoint(-1, -1);        
      }
    } else {
      _mouse.lastGuideline = QPoint(-1, -1);        
    }
  }
}


void Kst2DPlot::draw() {
  if (_zoomPaused) {
    return;
  }

  // Precondition: w and h are both > 0
  _buffer.buffer() = QPixmap(size());
  assert(!_buffer.buffer().isNull()); // Want to find these crashes
  if (_buffer.buffer().isNull()) {    // Because this is garbage
    return;
  }

  _buffer.buffer().fill(backgroundColor());
  KstPainter p;
  p.begin(&_buffer.buffer());
  p.setWindow(0, 0, geometry().width(), geometry().height());
  draw(p);
  p.end();
}


void Kst2DPlot::draw(KstPainter& p) {
  if (_zoomPaused) {
    return;
  }

  QRect old_window = p.window();
  double in_xleft_bdr_px, in_xright_bdr_px;
  double in_ytop_bdr_px, in_ybot_bdr_px;
  double x_min, x_max, y_min, y_max;
  TickParameters tpx,  tpy;
  double xleft_bdr_px, xright_bdr_px, ytop_bdr_px, ybot_bdr_px;
  double x_orig_px, y_orig_px, xtick_len_px, ytick_len_px;
  double xtick_px, ytick_px;
  double Lx, Hx, Ly, Hy;
  double m_X, m_Y, b_X, b_Y;
  bool offsetX, offsetY;

#ifdef BENCHMARK
  ++KstDebug::self()->drawCounter()[tagName()];
  qDebug() << ">>>>>>>>>>>>>>>>>>>> DRAWING PLOT " << tagName() << endl;
  QTime benchTime;
  int i_bt = 0, bt[15];
  QString bt_label[15];
  benchTime.start();
#endif

  QRect winRect = geometry();
  int x_px = winRect.width();
  int y_px = winRect.height();

  if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
    updateScale();
  }
  getLScale(x_min, y_min, x_max, y_max);

  p.fillRect(0, 0, winRect.width(), winRect.height(), _backgroundColor);

  int penWidth = axisPenWidth() * p.lineWidthAdjustmentFactor();
  p.setPen(QPen(_foregroundColor, penWidth));

  set2dPlotTickPix(xtick_len_px, ytick_len_px, x_px, y_px);

  setBorders(xleft_bdr_px, xright_bdr_px, ytop_bdr_px, ybot_bdr_px,
             tpx, tpy, p, offsetX, offsetY, xtick_len_px, ytick_len_px);

  // use a common plot region for plots that are aligned and of the same
  //  dimension, in either the horizontal or vertical sense...
  KST::alignment.limits(geometry(), in_xleft_bdr_px, in_xright_bdr_px, in_ytop_bdr_px, in_ybot_bdr_px, 1);
    // only override if borders are not suppressed
  if (!_suppressLeft && in_xleft_bdr_px > 0.001) { // x-left border overridden
    xleft_bdr_px = in_xleft_bdr_px;
  }
  if (!_suppressRight && in_xright_bdr_px > 0.001) { // x-right border overridden
    xright_bdr_px = in_xright_bdr_px;
  }
  if (!_suppressBottom && in_ybot_bdr_px > 0.001) { // y-bottom border overridden
    ybot_bdr_px = in_ybot_bdr_px;
  }
  if (!_suppressTop && in_ytop_bdr_px > 0.001) { // y-top border overridden
    ytop_bdr_px = in_ytop_bdr_px;
  }

  QRect RelPlotRegion(d2i(xleft_bdr_px),
      d2i(ytop_bdr_px),
      d2i(x_px - xright_bdr_px - xleft_bdr_px + 1.0),
      d2i(y_px - ybot_bdr_px - ytop_bdr_px + 1.0));
  QRect RelPlotAndAxisRegion(d2i(_yLabel->lineSpacing() + 1.0),
                        d2i(ytop_bdr_px),
                        d2i(x_px - _yLabel->lineSpacing() - xright_bdr_px),
                        d2i(y_px - _xLabel->lineSpacing() - ytop_bdr_px));
  QRect RelWinRegion(0, 0, d2i(x_px), d2i(y_px));

  x_orig_px = (tpx.org - x_min) / (x_max - x_min) * double(RelPlotRegion.width()-1) + xleft_bdr_px;
  y_orig_px = (y_max - tpy.org) / (y_max - y_min) * double(RelPlotRegion.height()-1) + ytop_bdr_px;
  xtick_px = (tpx.tick / (x_max - x_min)) * double(RelPlotRegion.width()-1);
  ytick_px = (tpy.tick / (y_max - y_min)) * double(RelPlotRegion.height()-1);

  if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
    setPixRect(RelPlotRegion, RelWinRegion, RelPlotAndAxisRegion);
  } else {
    const QRect a(PlotRegion), b(WinRegion), c(PlotAndAxisRegion);
    setPixRect(RelPlotRegion, RelWinRegion, RelPlotAndAxisRegion);
    recursively(&KstViewObject::updateFromAspect); // alignment may have changed
    setPixRect(a, b, c);
  }

  // only attempt to draw if plot is big enough
  if (x_px - xright_bdr_px - xleft_bdr_px >= 10.0 &&
      y_px - ybot_bdr_px - ytop_bdr_px + 1.0 - ytop_bdr_px >= 10.0) {
    Lx = RelPlotRegion.left();
    Hx = RelPlotRegion.right();
    Ly = RelPlotRegion.top();
    Hy = RelPlotRegion.bottom();
    m_X =  double(RelPlotRegion.width()-1)/(x_max - x_min);
    m_Y = -double(RelPlotRegion.height()-1)/(y_max - y_min);
    b_X = Lx - m_X * x_min;
    b_Y = Ly - m_Y * y_max;
    if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
      _m_X = m_X;
      _m_Y = m_Y;
      _b_X = b_X;
      _b_Y = b_Y;
    }

#ifdef BENCHMARK
    bt_label[i_bt] = "Initialization";
    bt[i_bt++] = benchTime.elapsed();
#endif
    plotLabels(p, x_px, y_px, xleft_bdr_px, xright_bdr_px, ytop_bdr_px);
#ifdef BENCHMARK
    bt_label[i_bt] = "Plot Labels";
    bt[i_bt++] = benchTime.elapsed();
#endif

    // create the context
    KstCurveRenderContext context;
    // FIXME: someone document what these things mean
    context.p = &p;
    context.Lx = Lx;
    context.Hx = Hx;
    context.Ly = Ly;
    context.Hy = Hy;
    context.m_X = m_X;
    context.m_Y = m_Y;
    context.b_X = b_X;
    context.b_Y = b_Y;
    context.x_max = x_max;
    context.y_max = y_max;
    context.x_min = x_min;
    context.y_min = y_min;
    context.XMin = XMin;
    context.YMin = YMin;
    context.XMax = XMax;
    context.YMax = YMax;
    context.xLog = _xLog;
    context.yLog = _yLog;
    context.xLogBase = _xLogBase;
    context.yLogBase = _yLogBase;
    context.penWidth = p.lineWidthAdjustmentFactor();
    context.foregroundColor = _foregroundColor;
    context.backgroundColor = _backgroundColor;

    // paint the curves
    p.save();
    if (_xReversed) {
      p.scale(-1, 1);
      p.translate(d2i(-1.0 * Hx - Lx), 0);
    }
    if (_yReversed) {
      p.scale(1, -1);
      p.translate(0, d2i(-1.0 * Hy - Ly));
    }
    p.setClipRect(int(Lx), int(Ly), int(Hx - Lx), int(Hy - Ly));
    for (KstBaseCurveList::Iterator i = Curves.begin(); i != Curves.end(); ++i) {
      (*i)->paint(context);
    }
    p.restore();

#ifdef BENCHMARK
    bt_label[i_bt] = "Plot Curves";
    bt[i_bt++] = benchTime.elapsed();
#endif
    // must plot grid lines before axes
    plotGridLines(p, tpx.tick, xleft_bdr_px, xright_bdr_px, x_orig_px, xtick_px,
                  xtick_len_px, x_px, tpy.tick, ytop_bdr_px, ybot_bdr_px, y_orig_px,
                  ytick_px, ytick_len_px, y_px);

#ifdef BENCHMARK
    bt_label[i_bt] = "Plot Grid Lines";
    bt[i_bt++] = benchTime.elapsed();
#endif
    p.setPen(QPen(_foregroundColor, penWidth));
    plotAxes(p, RelPlotRegion,
        tpx, xleft_bdr_px, xright_bdr_px, x_orig_px, xtick_px, xtick_len_px, x_px,
        tpy, ytop_bdr_px, ybot_bdr_px, y_orig_px, ytick_px, ytick_len_px, y_px,
        offsetY);
#ifdef BENCHMARK
    bt_label[i_bt] = "Plot Axes";
    bt[i_bt++] = benchTime.elapsed();
#endif
    if (_xReversed) {
      p.scale(-1,1);
      p.translate(d2i(-1 * Hx - Lx), 0);
    }
    plotPlotMarkers(p, m_X, b_X, x_max, x_min, y_px, ytop_bdr_px, ybot_bdr_px);
    if (_xReversed) {
      p.scale(-1,1);
      p.translate(d2i(-1 * Hx - Lx), 0);
    }
#ifdef BENCHMARK
    bt_label[i_bt] = "Plot Markers";
    bt[i_bt++] = benchTime.elapsed();
#endif

#ifdef BENCHMARK
    bt_label[i_bt] = "Flush Painter";
    bt[i_bt++] = benchTime.elapsed();

    qDebug() << "Plot Benchmark stats:" << endl;
    qDebug() << "   " << bt_label[0] << ": " << bt[0] << "ms" << endl;
    for (int j = 1; j < i_bt; ++j) {
      qDebug() << "   " << bt_label[j] << ": " << bt[j]-bt[j-1] << "ms" << endl;
    }
    qDebug() << "Plot Total: " << bt[i_bt-1] << "ms" << endl;
#endif
  } else {
    // if the plot is too small to draw then denote this with a cross pattern
    p.fillRect(RelWinRegion, QBrush(foregroundColor(), Qt::DiagCrossPattern));
    p.drawRect(RelWinRegion);
  }
}


QRect Kst2DPlot::GetPlotRegion() const {
  return PlotRegion;
}


QRect Kst2DPlot::GetWinRegion() const {
  return WinRegion;
}


QRect Kst2DPlot::GetPlotAndAxisRegion() const {
  return PlotAndAxisRegion;
}


QRect Kst2DPlot::GetTieBoxRegion() const {
  int left, top;
  const int dim = 11;

  if (WinRegion.right() - PlotRegion.right() > dim + 3) {
    left = PlotRegion.right() + 2;
  } else {
    left = WinRegion.right() - dim - 1;
  }
  if (PlotRegion.top() - WinRegion.top() > dim + 3) {
    top = PlotRegion.top() - 2 - dim;
  } else {
    top = WinRegion.top()+1;
  }

  return QRect(left, top, dim, dim);
}


void Kst2DPlot::setPixRect(const QRect& RelPlotRegion, const QRect& RelWinRegion, const QRect& RelPlotAndAxisRegion) {
  PlotRegion = RelPlotRegion;
  PlotRegion.translate(geometry().x(), geometry().y());
  WinRegion = RelWinRegion;
  WinRegion.translate(geometry().x(), geometry().y());
  PlotAndAxisRegion = RelPlotAndAxisRegion;
  PlotAndAxisRegion.translate(geometry().x(), geometry().y());
}


KstObject::UpdateType Kst2DPlot::update(int update_counter) {
  bool force = dirty();
  //setDirty(false);
  KstObject::UpdateType update_state;

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  // TODO: check labels too

  bool updated = false;
  update_state = updateChildren(update_counter);

  updated = (UPDATE == KstPlotBase::update(update_counter)) || updated;
  if (update_state == NO_CHANGE) {
    update_state = updated ? UPDATE : NO_CHANGE;
  }
  return setLastUpdateResult(update_state);
}


void Kst2DPlot::save(QTextStream& ts, const QString& indent) {
  QString l2 = indent + "  ";
  ts << indent << "<" << type() << ">" << endl;
  ts << l2 << "<tag>" << Q3StyleSheet::escape(tagName()) << "</tag>" << endl;
  for (KstViewObjectList::Iterator i = _children.begin(); i != _children.end(); ++i) {
    (*i)->save(ts, indent + "  ");
  }
  ts << indent << "</" << type() << ">" << endl;
}


void Kst2DPlot::saveAttributes(QTextStream& ts, const QString& indent) {
  unsigned i;
  QString l2 = indent + "  ";

  KstPlotBase::saveAttributes(ts, indent);

  ts << indent << "<xscalemode>" << _xScaleMode << "</xscalemode>" << endl;
  ts << indent << "<yscalemode>" << _yScaleMode << "</yscalemode>" << endl;
  ts << indent << "<xscalemodedefault>" << _xScaleModeDefault << "</xscalemodedefault>" << endl;
  ts << indent << "<yscalemodedefault>" << _yScaleModeDefault << "</yscalemodedefault>" << endl;

  ts << indent << "<xmin>" << XMin << "</xmin>" << endl;
  ts << indent << "<xmax>" << XMax << "</xmax>" << endl;
  ts << indent << "<ymin>" << YMin << "</ymin>" << endl;
  ts << indent << "<ymax>" << YMax << "</ymax>" << endl;

  ts << indent << "<xreversed>" << _xReversed << "</xreversed>" << endl;
  ts << indent << "<yreversed>" << _yReversed << "</yreversed>" << endl;

  // Reparse the expressions, then write it back out in text so that we can update
  // any vectors or scalars that had name changes, but we don't get affected by
  // the optimizer
  if (_xScaleMode == EXPRESSION) {
    if (!(reparseToText(_xMinExp) && reparseToText(_xMaxExp))) {
      KstDebug::self()->log(i18n("X scale expression could not be reparsed while saving.  Resulting Kst file may have issues."), KstDebug::Warning);
    }
    ts << indent << "<xminexp>" << _xMinExp << "</xminexp>" << endl;
    ts << indent << "<xmaxexp>" << _xMaxExp << "</xmaxexp>" << endl;
  }

  if (_yScaleMode == EXPRESSION) {
    if (!(reparseToText(_yMinExp) && reparseToText(_yMaxExp))) {
      KstDebug::self()->log(i18n("Y scale expression could not be reparsed while saving.  Resulting Kst file may have issues."), KstDebug::Warning);
    }
    ts << indent << "<yminexp>" << _yMinExp << "</yminexp>" << endl;
    ts << indent << "<ymaxexp>" << _yMaxExp << "</ymaxexp>" << endl;
  }

  ts << indent << "<toplabel>" << endl;
  _topLabel->save(ts, l2, true);
  ts << indent << "</toplabel>" << endl;

  ts << indent << "<xlabel>" << endl;
  _xLabel->save(ts, l2, false);
  ts << indent << "</xlabel>" << endl;

  ts << indent << "<ylabel>" << endl;
  _yLabel->save(ts, l2, false);
  ts << indent << "</ylabel>" << endl;

  ts << indent << "<xticklabel>" << endl;
  _xTickLabel->save(ts, l2, false);
  ts << indent << "</xticklabel>" << endl;

  ts << indent << "<yticklabel>" << endl;
  _yTickLabel->save(ts, l2, false);
  ts << indent << "</yticklabel>" << endl;

  ts << indent << "<xfullticklabel>" << endl;
  _fullTickLabel->save(ts, l2, false);
  ts << indent << "</xfullticklabel>" << endl;

  if (isXLog()) {
    ts << indent << "<xlog/>" << endl;
  }
  if (isYLog()) {
    ts << indent << "<ylog/>" << endl;
  }

  ts << indent << "<xlogbase>" << _xLogBase << "</xlogbase>" << endl;
  ts << indent << "<ylogbase>" << _yLogBase << "</ylogbase>" << endl;

  for (KstBaseCurveList::Iterator j = Curves.begin(); j != Curves.end(); ++j) {
    (*j)->readLock();
    ts << indent << "<curvetag>" << Q3StyleSheet::escape((*j)->tagName()) << "</curvetag>" << endl;
    (*j)->unlock();
  }

  // save the plot colors, but only if they are different from default
  if (_foregroundColor != KstSettings::globalSettings()->foregroundColor) {
    ts << indent << "<plotforecolor>" << Q3StyleSheet::escape(_foregroundColor.name()) << "</plotforecolor>" << endl;
  }
  if (_backgroundColor != KstSettings::globalSettings()->backgroundColor) {
    ts << indent << "<plotbackcolor>" << Q3StyleSheet::escape(_backgroundColor.name()) << "</plotbackcolor>" << endl;
  }

  // save the plot markers
  // makes sure autogenerated markers are here
  updateMarkersFromCurve();
  updateMarkersFromVector();
  for (i = 0; i < _plotMarkers.count(); i++) {
    ts << indent << "<plotmarker value=\"" << _plotMarkers[i].value << "\"";
    if (_plotMarkers[i].isRising) {
      ts << " rising=\"1\"";
    } else if (_plotMarkers[i].isFalling) {
      ts << " falling=\"1\"";
    } else if (_plotMarkers[i].isVectorValue) {
      ts << " vector=\"1\"";
    }
    ts << " />" << endl;
  }
  if (hasCurveToMarkers()) {
    ts << indent << "<curvetomarkersname>" << _curveToMarkers->tagName() <<  "</curvetomarkersname>" <<endl;
    ts << indent << "<curvetomarkersrisingdetect>" << _curveToMarkersRisingDetect << "</curvetomarkersrisingdetect>" <<endl;
    ts << indent << "<curvetomarkersfallingdetect>" << _curveToMarkersFallingDetect << "</curvetomarkersfallingdetect>" <<endl;
  }

  if (hasVectorToMarkers()) {
    ts << indent << "<vectortomarkersname>" << _vectorToMarkers->tagName() <<  "</vectortomarkersname>" <<endl;
  }

  // save grid line settings
  ts << indent << "<xmajorgrid>" << _xMajorGrid << "</xmajorgrid>" << endl;
  ts << indent << "<ymajorgrid>" << _yMajorGrid << "</ymajorgrid>" << endl;
  ts << indent << "<xminorgrid>" << _xMinorGrid << "</xminorgrid>" << endl;
  ts << indent << "<yminorgrid>" << _yMinorGrid << "</yminorgrid>" << endl;
  ts << indent << "<majorgridcolor>" << _majorGridColor.name() << "</majorgridcolor>" << endl;
  ts << indent << "<minorgridcolor>" << _minorGridColor.name() << "</minorgridcolor>" << endl;
  ts << indent << "<majorgridcolordefault>" << _majorGridColorDefault << "</majorgridcolordefault>" << endl;
  ts << indent << "<minorgridcolordefault>" << _minorGridColorDefault << "</minorgridcolordefault>" << endl;

  ts << indent << "<majorpenwidth>" << majorPenWidth() << "</majorpenwidth>" << endl;
  ts << indent << "<minorpenwidth>" << minorPenWidth() << "</minorpenwidth>" << endl;
  ts << indent << "<axispenwidth>" << axisPenWidth() << "</axispenwidth>" << endl;

  // minor ticks
  ts << indent << "<xminorticks>" << _reqXMinorTicks << "</xminorticks>" << endl;
  ts << indent << "<yminorticks>" << _reqYMinorTicks << "</yminorticks>" << endl;

  // major ticks
  ts << indent << "<xmajorticks>" << _xMajorTicks << "</xmajorticks>" << endl;
  ts << indent << "<ymajorticks>" << _yMajorTicks << "</ymajorticks>" << endl;

  // tick placement
  ts << indent << "<xticksinplot>" << _xTicksInPlot << "</xticksinplot>" << endl;
  ts << indent << "<xticksoutplot>" << _xTicksOutPlot << "</xticksoutplot>" << endl;
  ts << indent << "<yticksinplot>" << _yTicksInPlot << "</yticksinplot>" << endl;
  ts << indent << "<yticksoutplot>" << _yTicksOutPlot << "</yticksoutplot>" << endl;

  // axis suppression
  ts << indent << "<suppresstop>" << _suppressTop << "</suppresstop>" << endl;
  ts << indent << "<suppressbottom>" << _suppressBottom << "</suppressbottom>" << endl;
  ts << indent << "<suppressleft>" << _suppressLeft << "</suppressleft>" << endl;
  ts << indent << "<suppressright>" << _suppressRight << "</suppressright>" << endl;

  // transformed axis
  ts << indent << "<xtransformed>" << _xTransformed << "</xtransformed>" << endl;
  ts << indent << "<ytransformed>" << _yTransformed << "</ytransformed>" << endl;
  ts << indent << "<xtransformedexp>" << _xTransformedExp << "</xtransformedexp>" << endl;
  ts << indent << "<ytransformedexp>" << _yTransformedExp << "</ytransformedexp>" << endl;

  // axis interpretation settings
  ts << indent << "<xinterpret>" << _isXAxisInterpreted << "</xinterpret>" << endl;
  ts << indent << "<xinterpretas>" << _xAxisInterpretation << "</xinterpretas>" << endl;
  ts << indent << "<xdisplayas>" << _xAxisDisplay << "</xdisplayas>" << endl;
  ts << indent << "<yinterpret>" << _isYAxisInterpreted << "</yinterpret>" << endl;
  ts << indent << "<yinterpretas>" << _yAxisInterpretation << "</yinterpretas>" << endl;
  ts << indent << "<ydisplayas>" << _yAxisDisplay << "</ydisplayas>" << endl;

  ts << indent << "<xoffsetmode>" << _xOffsetMode << "</xoffsetmode>" << endl;
  ts << indent << "<yoffsetmode>" << _yOffsetMode << "</yoffsetmode>" << endl;

  ts << indent << "<stylemarker>" << _lineStyleMarkers << "</stylemarker>" << endl;
  ts << indent << "<widthmarker>" << _lineWidthMarkers << "</widthmarker>" << endl;
  ts << indent << "<colormarker>" << _colorMarkers.name() << "</colormarker>" << endl;
  ts << indent << "<defaultcolormarker>" << _defaultMarkerColor << "</defaultcolormarker>" << endl;
}


void Kst2DPlot::pushScale() {
  KstPlotScale *ps = new KstPlotScale;
  ps->xmin = XMin;
  ps->ymin = YMin;
  ps->xmax = XMax;
  ps->ymax = YMax;
  ps->xscalemode = _xScaleMode;
  ps->yscalemode = _yScaleMode;
  ps->xlog = _xLog;
  ps->ylog = _yLog;
  ps->xMinExp = _xMinExp;
  ps->xMaxExp = _xMaxExp;
  ps->yMinExp = _yMinExp;
  ps->yMaxExp = _yMaxExp;

  _plotScaleList.append(ps);
}


bool Kst2DPlot::popScale() {
  if (_plotScaleList.count() > 1) {
    _plotScaleList.removeLast();
    KstPlotScale *ps = _plotScaleList.last();
    XMin = ps->xmin;
    XMax = ps->xmax;
    YMin = ps->ymin;
    YMax = ps->ymax;
    _xScaleMode = ps->xscalemode;
    _yScaleMode = ps->yscalemode;
    _xLog = ps->xlog;
    _yLog = ps->ylog;
    _xMinExp = ps->xMinExp;
    _xMaxExp = ps->xMaxExp;
    _yMinExp = ps->yMinExp;
    _yMaxExp = ps->yMaxExp;
    _xMinParsedValid = reparse(_xMinExp, &_xMinParsed);
    _xMaxParsedValid = reparse(_xMaxExp, &_xMaxParsed);
    _yMinParsedValid = reparse(_yMinExp, &_yMinParsed);
    _yMaxParsedValid = reparse(_yMaxExp, &_yMaxParsed);
    optimizeXExps();
    optimizeYExps();
    return true;
  }
  return false;
}


/****************************************************************/
/*                                                              */
/*        Place a '\' in front of special characters (ie, '_')  */
/*                                                              */
/****************************************************************/
static void EscapeSpecialChars(QString& label) {
  unsigned int i_char;

  for (i_char = 0; i_char < label.length(); i_char++) {
    if (label.at(i_char) == '_') {
      label.insert(i_char, '\\');
      i_char++;
    }
  }
}


void Kst2DPlot::generateDefaultLabels(bool xl, bool yl, bool tl) {
  QStringList xlabels, ylabels, toplabels;
  QString label, xlabel, ylabel, toplabel;
  int n_curves, i_curve, i_count;

  n_curves = Curves.count();

  // accumulate list of curve labels
  for (KstBaseCurveList::ConstIterator i = Curves.begin(); i != Curves.end(); ++i) {
    (*i)->readLock();
    if (xlabels.indexOf((*i)->xLabel()) == -1) {
      xlabels.append((*i)->xLabel());
    }
    if (ylabels.indexOf((*i)->yLabel()) == -1) {
      ylabels.append((*i)->yLabel());
    }
    if (toplabels.indexOf((*i)->topLabel()) == -1) {
      toplabels.append((*i)->topLabel());
    }
    (*i)->unlock();
  }

  // create the labels
  if (n_curves > 0) {
    // the x axis label
    i_count = xlabels.count();
    for (i_curve = 0; i_curve < i_count; i_curve++) {
      if (i_curve == i_count - 1) {
        xlabel += xlabels[i_curve];
      } else if (i_curve < i_count-2) {
        xlabel += i18nc("name in a list", "%1, ").arg(xlabels[i_curve]);
      } else if (i_curve == i_count-2) {
        xlabel += i18nc("penultimate name in a list", "%1 and ").arg(xlabels[i_curve]);
      }
    }

    // the y axis label
    i_count = ylabels.count();
    if (i_count < 4) {
      // only fill if there are 1, 2 or 3 different label....
      // otherwise a legend box should be used.
      for (i_curve = 0; i_curve < i_count; i_curve++) {
        if (i_curve == i_count - 1) {
          ylabel += ylabels[i_curve];
        } else if (i_curve < i_count-2) {
          ylabel += i18nc("name in a list", "%1, ").arg(ylabels[i_curve]);
        } else if (i_curve == i_count-2) {
          ylabel += i18nc("penultimate name in a list", "%1 and ").arg(ylabels[i_curve]);
        }
      }
    }

    // create the top label
    i_count = toplabels.count();
    for (i_curve = 0; i_curve < i_count; i_curve++) {
      if (i_curve == i_count - 1) {
        toplabel += toplabels[i_curve];
      } else if (i_curve < i_count-2) {
        toplabel += i18nc("name in a list", "%1, ").arg(toplabels[i_curve]);
      } else if (i_curve == i_count-2) {
        toplabel += i18nc("penultimate name in a list", "%1 and ").arg(toplabels[i_curve]);
      }
    }
  }

  EscapeSpecialChars(xlabel);
  EscapeSpecialChars(ylabel);
  EscapeSpecialChars(toplabel);

  if (xl) {
    _xLabel->setText(xlabel);
  }
  if (yl) {
    _yLabel->setText(ylabel);
  }
  if (tl) {
    _topLabel->setText(toplabel);
  }
}


void Kst2DPlot::setTicks(double& tick, double& org, double max, double min, bool is_log, double logBase, bool isX, int base) {
  double Exp;
  int auto_tick;
  int majorDensity = isX ? _xMajorTicks : _yMajorTicks;

  static double b10_ticks[] = {1.0, 2.0, 5.0, 10.0};
  static int b10_autominor[]= {  5,   4,   5,    5};
  static int n_b10_ticks = sizeof(b10_ticks) / sizeof(double);

  static double b24_ticks[] = {1.0, 2.0, 4.0, 6.0, 12.0, 24.0};
  static int b24_autominor[]= {  5,   4,   4,   6,    6,    6};
  static int n_b24_ticks = sizeof(b24_ticks) / sizeof(double);

  static double b60_ticks[] = {1.0, 2.0, 5.0, 10.0, 15.0, 20.0, 30.0, 60.0};
  static int b60_autominor[] = { 5,   4,   5,    5,    3,    4,    6,    6};
  static int n_b60_ticks = sizeof(b60_ticks) / sizeof(double);

  // check for hysteresis of y-axis tick spacing...
  double St = (max - min) / (double)majorDensity;
  if (!isX && is_log == _isLogLast && _stLast != 0.0 &&
        St/_stLast < TICK_HYSTERESIS_FACTOR &&
        St/_stLast > 1.0/TICK_HYSTERESIS_FACTOR) {
    St = _stLast;
    tick = _tickYLast;
    auto_tick = _autoTickYLast;
  } else if (is_log) {
    if (max - min <= (double)majorDensity*1.5) {
      // show in logarithmic mode with major ticks nicely labelled and the
      // specified number of minor ticks between each major label
      if (logBase == 2.0) {
        auto_tick = 10;
      } else if (logBase == 10.0) {
        auto_tick = 9;
      } else {
        auto_tick = 5;
      }
      tick = 1.0;
    } else {
      // show in logarithmic mode with major ticks nicely labelled and no minor ticks
      auto_tick = 0;
      tick = floor((max - min) / (double)majorDensity);
      if (tick == 1.0) {
        if (logBase == 2.0) {
          auto_tick = 0; // used to be 10... why?  what is 1/11 of a log interval?
        } else if (logBase == 10.0) {
          auto_tick = 9;
        } else {
          auto_tick = 5;
        }
      }
    }
  } else {
    double *ticks = b10_ticks;
    int nt = n_b10_ticks;
    int *autominor = b10_autominor;

    // determine tick interval
    Exp = 0.0;
    if (base == 60) {
      if (b60_ticks[0]*0.7 < St && b60_ticks[n_b60_ticks-1] > St*0.7) {
        Exp = 1.0;
        ticks = b60_ticks;
        autominor = b60_autominor;
        nt = n_b60_ticks;
      }
    } else if (base == 24) {
      if (b24_ticks[0]*0.7 < St && b24_ticks[n_b24_ticks-1] > St*0.7) {
        Exp = 1.0;
        ticks = b24_ticks;
        autominor = b24_autominor;
        nt = n_b24_ticks;
      }
    }

    if (Exp < 0.5) {
      Exp = pow(logBase, floor(log10(St)/log10(logBase)));
    }

    tick = ticks[0] * Exp;
    auto_tick = autominor[0];
    for (int i = 1; i < nt; i++) {
      if (fabs((ticks[i] * Exp) - St) < fabs(tick - St)) {
        tick = ticks[i] * Exp;
        auto_tick = autominor[i];
      }
    }
  }

  if (isX) {
    _xMinorTicks = (_reqXMinorTicks < 0) ? auto_tick : _reqXMinorTicks;
  } else {
    _yMinorTicks = (_reqYMinorTicks < 0) ? auto_tick : _reqYMinorTicks;
  }

  // determine location of the origin
  if (min > 0.0) {
    org = ceil(min / tick) * tick;
  } else if (max < 0.0) {
    org = floor(max / tick) * tick;
  } else {
    org = 0.0;
  }

  if (!isX) {
    _stLast = St;
    _tickYLast = tick;
    _autoTickYLast = auto_tick;
    _isLogLast = is_log;
  }
}


void Kst2DPlot::setLog(bool x_log, bool y_log) {
  _xLog = x_log;
  _yLog = y_log;
}


bool Kst2DPlot::isXLog() const {
  return _xLog;
}


bool Kst2DPlot::isYLog() const {
  return _yLog;
}


void Kst2DPlot::setXAxisInterpretation(bool isXAxisInterpreted, KstAxisInterpretation xAxisInterpretation, KstAxisDisplay xAxisDisplay) {
  _isXAxisInterpreted = isXAxisInterpreted;
  if (_isXAxisInterpreted) {
    _xAxisInterpretation = xAxisInterpretation;
    _xAxisDisplay = xAxisDisplay;
  }
}


void Kst2DPlot::getXAxisInterpretation( bool& isXAxisInterpreted, KstAxisInterpretation& xAxisInterpretation, KstAxisDisplay& xAxisDisplay) const {
  isXAxisInterpreted = _isXAxisInterpreted;
  xAxisInterpretation = _xAxisInterpretation;
  xAxisDisplay = _xAxisDisplay;
}


void Kst2DPlot::setYAxisInterpretation( bool isYAxisInterpreted, KstAxisInterpretation yAxisInterpretation, KstAxisDisplay yAxisDisplay) {
  _isYAxisInterpreted = isYAxisInterpreted;
  if (_isYAxisInterpreted) {
    _yAxisInterpretation = yAxisInterpretation;
    _yAxisDisplay = yAxisDisplay;
  }
}


void Kst2DPlot::getYAxisInterpretation( bool& isYAxisInterpreted, KstAxisInterpretation& yAxisInterpretation, KstAxisDisplay& yAxisDisplay) const {
  isYAxisInterpreted = _isYAxisInterpreted;
  yAxisInterpretation = _yAxisInterpretation;
  yAxisDisplay = _yAxisDisplay;
}


bool Kst2DPlot::isTied() const {
  return _isTied;
}


void Kst2DPlot::toggleTied() {
  _isTied = !_isTied;
}


void Kst2DPlot::setTied(bool in_tied) {
  _isTied = in_tied;
}


void Kst2DPlot::editCurve(int id) {
  KstBaseCurvePtr curve = *(Curves.findTag(_curveEditMap[id]));
  if (curve) {
    curve->readLock();
    curve->showDialog(false);
    curve->unlock();
  }
}


void Kst2DPlot::editObject(int id) {
  KstDataObjectPtr dop = *(KST::dataObjectList.findTag(_objectEditMap[id]));
  if (dop) {
    dop->readLock();
    dop->showDialog(false);
    dop->unlock();
  }
}


void Kst2DPlot::editVector(int id) {
  KstDialogs::self()->showVectorDialog(_objectEditMap[id], true);
}


void Kst2DPlot::matchAxes(int id) {
  Kst2DPlotPtr p = (Kst2DPlot*)_plotMap[id];
  if (p) {
    double x0, x1, y0, y1;
    p->getScale(x0, y0, x1, y1);
    setLog(p->isXLog(), p->isYLog());
    setXScaleMode(FIXED);
    setYScaleMode(FIXED);
    setXScale(x0, x1);
    setYScale(y0, y1);
    pushScale();
    if (isTied() && _menuView) {
      KstApp::inst()->tiedZoom(true, x0, x1, true, y0, y1, _menuView, tagName());
    }
    setDirty();
    if (_menuView) {
      _menuView->paint();
    }
  }
}


void Kst2DPlot::matchXAxis(int id) {
  Kst2DPlotPtr p = (Kst2DPlot*)_plotMap[id];
  if (p) {
    double x0, x1, y0, y1;
    p->getScale(x0, y0, x1, y1);
    setLog(p->isXLog(), isYLog());
    setXScaleMode(FIXED);
    setXScale(x0, x1);
    pushScale();
    if (isTied() && _menuView) {
      KstApp::inst()->tiedZoom(true, x0, x1, false, 0.0, 0.0, _menuView, tagName());
    }
    setDirty();
    if (_menuView) {
      _menuView->paint();
    }
  }
}


bool Kst2DPlot::tiedZoomPrev(QWidget *view) {
  bool updated = false;

  if (popScale()) {
    cancelZoom(view);
    setDirty();
    updated = true;
  }

  return updated;
}


bool Kst2DPlot::tiedZoomMode(ZoomType zoom, bool flag, double center, KstScaleModeType mode, KstScaleModeType modeExtra) {
  bool updated = true;

  switch (zoom) {
    case ZOOM_MOVE_HORIZONTAL:
      if(!moveSelfHorizontal(flag)) {
        updated = false;
      }
      break;
    case ZOOM_MOVE_VERTICAL:
      if(!moveSelfVertical(flag)) {
        updated = false;
      }
      break;
    case ZOOM_CENTER:
      moveSelfToCenter(center);
      break;
    case ZOOM_VERTICAL:
      if(!zoomSelfVertical(flag)) {
        updated = false;
      }
      break;
    case ZOOM_HORIZONTAL:
      if(!zoomSelfHorizontal(flag)) {
        updated = false;
      }
      break;
    case ZOOM_X_MODE:
      setXScaleMode(mode);
      break;
    case ZOOM_Y_MODE:
      setYScaleMode(mode);
      break;
    case ZOOM_XY_MODES:
      setXScaleMode(mode);
      setYScaleMode(modeExtra);
      break;
    case ZOOM_Y_LOCAL_MAX:
      zoomSelfYLocalMax(false);
      break;
    default:
      updated = false;
      break;
  }

  if (updated) {
    pushScale();
    setDirty();
  }

  return updated;
}

void Kst2DPlot::tiedZoom(bool x, double xmin, double xmax, bool y, double ymin, double ymax) {
  if (x && y) {
    setXScaleMode(FIXED);
    setYScaleMode(FIXED);
    setLScale(xmin, ymin, xmax, ymax);
    pushScale();
  } else if (x) {
    setXScaleMode(FIXED);
    setLXScale(xmin, xmax);
    pushScale();
  } else if (y) {
    setYScaleMode(FIXED);
    setLYScale(ymin, ymax);
    pushScale();
  }
  setDirty();
}


void Kst2DPlot::copyObject() {
  if (_layoutMenuView) {
    KstTopLevelViewPtr tlv = _layoutMenuView->viewObject();
    if (tlv) {
      KstViewWindow *vw = dynamic_cast<KstViewWindow*>(_layoutMenuView->parent());
      if (vw) {
        QStringList plotList;

        plotList.append(tagName());

        PlotMimeSource *newplots = new PlotMimeSource(vw->windowTitle(), plotList);

        QMimeData mimeData;
        mimeData.setData(newplots->format(0), newplots->encodedData(newplots->format(0)));

        QApplication::clipboard()->setMimeData(&mimeData, QClipboard::Clipboard);
      }
    }
  }
}


KstViewObject* Kst2DPlot::copyObjectQuietly(KstViewObject& parent, const QString& name) const {
  QString plotName;

  if (name.isEmpty()) {
    plotName = i18n("%1-copy").arg(tagName());
  } else {
    plotName = name;
  }

  Kst2DPlot *plot = new Kst2DPlot(*this, plotName);
  parent.appendChild(plot, true);

  return plot;
}


void Kst2DPlot::removeCurve(int id) {
  KstBaseCurvePtr curve = *(Curves.findTag(_curveRemoveMap[id]));
  if (curve) {
    removeCurve(curve);
    if (_menuView) {
      _menuView->paint();
    }
  }
}


bool Kst2DPlot::popupMenu(KMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent) {
  bool hasEntry = false;
  KstMouseModeType mode;

  mode = globalZoomType();

  KstTopLevelViewPtr tlv = kst_cast<KstTopLevelView>(topLevelParent);
  _menuView = tlv ? tlv->widget() : 0L;
  KstViewObject::popupMenu(menu, pos, topLevelParent);

  KMenu *submenu = new KMenu(menu);
  KMenu *submenu2 = new KMenu(menu);
  Kst2DPlotList pl = globalPlotList();
  int i = 0;
  _plotMap.clear();
  for (Kst2DPlotList::ConstIterator j = pl.begin(); j != pl.end(); ++j) {
    if ((*j).data() != this) {
      _plotMap[i] = *j; // don't think there is any way around this.
                        // We have to hope that it's safe until the menu is
                        // done.
      submenu->insertItem((*j)->tagName(), i);
      submenu->connectItem(i, this, SLOT(matchAxes(int)));
      submenu2->insertItem((*j)->tagName(), i);
      submenu2->connectItem(i++, this, SLOT(matchXAxis(int)));
      hasEntry = true;
    }
  }
  int id = menu->insertItem(i18n("&Match Axes"), submenu);
  menu->setItemEnabled(id, hasEntry);
  id = menu->insertItem(i18n("&Match X Axis"), submenu2);
  menu->setItemEnabled(id, hasEntry);
  hasEntry = false;

  submenu = new KMenu(menu);
  menu->insertItem(i18n("Z&oom"), submenu);
  submenu->insertItem(i18n("Zoom &Maximum"), this, SLOT(menuZoomMax()), Qt::Key_M);
  submenu->insertItem(i18n("Zoom Max &Spike Insensitive"),
                      this, SLOT(menuZoomSpikeInsensitiveMax()), Qt::Key_S);
  submenu->insertItem(i18n("Zoom P&revious"), this, SLOT(menuZoomPrev()), Qt::Key_R);
  submenu->insertItem(i18n("Y-Zoom Mean-centered"), this, SLOT(menuYZoomAc()), Qt::Key_A);
  submenu->insertSeparator();
  submenu->insertItem(i18n("X-Zoom Maximum"),
                        this, SLOT(menuXZoomMax()), Qt::CTRL + Qt::Key_M);
  submenu->insertItem(i18n("X-Zoom Out"),
                        this, SLOT(menuXZoomOut()), Qt::SHIFT + Qt::Key_Right);
  submenu->insertItem(i18n("X-Zoom In"),
                        this, SLOT(menuXZoomIn()), Qt::SHIFT + Qt::Key_Left);
  submenu->insertItem(i18n("Normalize X Axis to Y Axis"),
                        this, SLOT(menuXNormalize()), Qt::Key_N);
  submenu->insertItem(i18n("Toggle Log X Axis"),
                        this, SLOT(menuXLogSlot()), Qt::Key_G);
  submenu->insertSeparator();
  submenu->insertItem(i18n("Y-Zoom Local Maximum"),
                      this, SLOT(menuYZoomLocalMax()), Qt::SHIFT + Qt::Key_L);
  submenu->insertItem(i18n("Y-Zoom Maximum"),
                        this, SLOT(menuYZoomMax()), Qt::SHIFT + Qt::Key_M);
  submenu->insertItem(i18n("Y-Zoom Out"),
                        this, SLOT(menuYZoomOut()), Qt::SHIFT + Qt::Key_Up);
  submenu->insertItem(i18n("Y-Zoom In"),
                        this, SLOT(menuYZoomIn()), Qt::SHIFT + Qt::Key_Down);
  submenu->insertItem(i18n("Normalize Y Axis to X Axis"),
                        this, SLOT(menuYNormalize()), Qt::SHIFT + Qt::Key_N);
  submenu->insertItem(i18n("Toggle Log Y Axis"),
                        this, SLOT(menuYLogSlot()), Qt::Key_L);
  submenu->insertSeparator();
  submenu->insertItem(i18n("Next &Image Color Scale"),
                      this, SLOT(menuNextImageColorScale()), Qt::Key_I);
  
  submenu = new KMenu(menu);
  menu->insertItem(i18n("&Scroll"), submenu);
  submenu->insertItem(i18n("Left"), this, SLOT(menuMoveLeft()), Qt::Key_Left);
  submenu->insertItem(i18n("Right"), this, SLOT(menuMoveRight()), Qt::Key_Right);
  submenu->insertItem(i18n("Up"), this, SLOT(menuMoveUp()), Qt::Key_Up);
  submenu->insertItem(i18n("Down"), this, SLOT(menuMoveDown()), Qt::Key_Down);
  submenu->insertSeparator();

  // disable next or previous marker items if necessary
  #ifndef MARKER_NUM_SEGS
  #define MARKER_NUM_SEGS 50  //sort of get around rounding errors?  Also used in MoveToMarker function
  #endif
  double xmin, xmax;
  double tempVal;
  getLScale(xmin, tempVal, xmax, tempVal);
  double currCenter = ((xmax + xmin)/2.0) + (xmax - xmin)/MARKER_NUM_SEGS;
  if (_xLog) {
    currCenter = pow(_xLogBase, currCenter);
  }
  id = submenu->insertItem(i18n("Next Marker"), this, SLOT(menuNextMarker()), Qt::ALT + Qt::Key_Right);
  submenu->setItemEnabled(id, nextMarker(currCenter, tempVal));
  id = submenu->insertItem(i18n("Previous Marker"), this, SLOT(menuPrevMarker()), Qt::ALT + Qt::Key_Left);
  currCenter = ((xmax + xmin)/2.0) - (xmax - xmin)/MARKER_NUM_SEGS;
  if (_xLog) {
    currCenter = pow(_xLogBase, currCenter);
  }
  submenu->setItemEnabled(id, prevMarker(currCenter, tempVal) && (!_xLog || tempVal > 0));

  int n_curves = Curves.count();
  menu->insertSeparator();

  _objectEditMap.clear();
  _curveEditMap.clear();
  _curveFitMap.clear();
  _curveRemoveMap.clear();

  // Edit menu
  submenu = new KMenu(menu);
  // Fit menu
  KMenu *submenu4 = new KMenu(menu);
  // Filter menu
  submenu2 = new KMenu(menu);
  // Remove menu
  KMenu *submenu3 = new KMenu(menu);
  hasEntry = false;
  for (i = 0; i < n_curves; i++) {
    KstBaseCurvePtr c = Curves[i];
    c->readLock();
    const QString& tag = c->tagName();
    c->unlock();
    _curveEditMap[i] = tag;
    submenu->insertItem(i18nc("Type: Name", "Plot Object: %1").arg(tag), i);
    submenu->connectItem(i, this, SLOT(editCurve(int)));
    KstVCurvePtr vc = kst_cast<KstVCurve>(c);
    if (vc && vc->yVector()) {
      KstObjectPtr provider = vc->yVector()->provider();
      if (provider) {
        KstDataObjectPtr dop = kst_cast<KstDataObject>(provider);
        if (dop) {
          _objectEditMap[i + n_curves] = dop->tagName();
          submenu->insertItem(i18nc("Type: Name", "%1: %2").arg(dop->typeString()).arg(dop->tagName()), i + n_curves);
          submenu->connectItem(i + n_curves, this, SLOT(editObject(int)));
        }
      } else {
        KstRVectorPtr rv = kst_cast<KstRVector>(vc->yVector());
        if (rv) {
          _objectEditMap[i + n_curves] = rv->tagName();
          submenu->insertItem(i18nc("Type: Name", "Vector: %1").arg(rv->tagName()), i + n_curves);
          submenu->connectItem(i + n_curves, this, SLOT(editVector(int)));
        }
      }
    } else if (KstImagePtr img = kst_cast<KstImage>(c)) {
      KstObjectPtr provider = img->matrix()->provider();
      if (provider) {
        KstDataObjectPtr dop = kst_cast<KstDataObject>(provider);
        if (dop) {
          _objectEditMap[i + n_curves] = dop->tagName();
          submenu->insertItem(i18nc("Type: Name", "%1: %2").arg(dop->typeString()).arg(dop->tagName()), i + n_curves);
          submenu->connectItem(i + n_curves, this, SLOT(editObject(int)));
        }
      } else {
        KstRMatrixPtr rm = kst_cast<KstRMatrix>(img->matrix());
        if (rm) {
          _objectEditMap[i + n_curves] = rm->tagName();
          submenu->insertItem(i18nc("Type: Name", "Matrix: %1").arg(rm->tagName()), i + n_curves);
          submenu->connectItem(i + n_curves, this, SLOT(editMatrix(int)));
        }
      }
    }
    _curveFitMap[i] = tag;
    _curveRemoveMap[i] = tag;
    submenu4->insertItem(tag, i);
    submenu2->insertItem(tag, i);
    submenu3->insertItem(tag, i);
    submenu4->connectItem(i, this, SLOT(fitCurve(int)));
    submenu2->connectItem(i, this, SLOT(filterCurve(int)));
    submenu3->connectItem(i, this, SLOT(removeCurve(int)));
    hasEntry = true;
  }

  id = menu->insertItem(i18n("Edit"), submenu);
  menu->setItemEnabled(id, hasEntry);
  id = menu->insertItem(i18n("Fit"), submenu4);
  menu->setItemEnabled(id, hasEntry);
  id = menu->insertItem(i18n("Filter"), submenu2);
  menu->setItemEnabled(id, hasEntry);
  id = menu->insertItem(i18n("Remove"), submenu3);
  menu->setItemEnabled(id, hasEntry);

  return true;
}


bool Kst2DPlot::layoutPopupMenu(KMenu *menu, const QPoint& pos, KstViewObjectPtr topLevelParent) {
  KstTopLevelViewPtr tlv = kst_cast<KstTopLevelView>(topLevelParent);
  _layoutMenuView = tlv ? tlv->widget() : 0L;
  KstViewObject::layoutPopupMenu(menu, pos, topLevelParent);
  return true;
}


bool Kst2DPlot::mouseHandler() const {
  return true;
}


void Kst2DPlot::setHasFocus(bool has) {
  if (!has) {
    _mouse.tracker = _mouse.lastLocation = QPoint(-1, -1);
  }
  _hasFocus = has;
}


void Kst2DPlot::unsetCursorPos(QWidget *view) {
  if (_cursorOffset) {
    drawCursorPos(view);

    _cursor_x = KST::NOPOINT;
    _cursor_y = KST::NOPOINT;
    _cursorOffset = false;
  } else {
    setCursorPos(view);
  }
}


void Kst2DPlot::setCursorPos(QWidget *view) {
  QRect pr = GetPlotRegion();

  if (pr.contains(_mouse.tracker)) {
    QString name;
    double xmin, ymin;
    double xmax, ymax;

    drawCursorPos(view);
    if (KstApp::inst()->dataMode()) {
      double xpos, ypos;
      getCursorPos(_mouse.tracker, xpos, ypos, xmin, xmax, ymin, ymax);
      getNearestDataPoint(_mouse.tracker, name, _cursor_x, _cursor_y, xpos, ypos, xmin, xmax);
    } else {
      getCursorPos(_mouse.tracker, _cursor_x, _cursor_y, xmin, xmax, ymin, ymax);
    }
    _cursorOffset = true;
    drawCursorPos(view);
  }
}


void Kst2DPlot::updateMousePos(const QPoint& pos) {
  QString xlabel, ylabel;
  QString msgXOffset;
  QString msgYOffset;
  QRect pr = GetPlotRegion();
  double xmin, ymin, xmax, ymax, xpos, ypos;
  uint length;

  getLScale(xmin, ymin, xmax, ymax);


  if (_xReversed) {
    xpos = (double)(pr.right() - pos.x())/(double)pr.width();
  } else {
    xpos = (double)(pos.x() - pr.left())/(double)pr.width();
  }
  xpos = xpos * (xmax - xmin) + xmin;

  if (_yReversed) {
    ypos = (double)(pr.bottom() - pos.y())/(double)pr.height();
  } else {
    ypos = (double)(pos.y() - pr.top())/(double)pr.height();
  }
  ypos = ypos * (ymin - ymax) + ymax;

  _copy_x = xpos;
  _copy_y = ypos;

  if (_isXAxisInterpreted) {
    genAxisTickLabelFullPrecision(_xAxisInterpretation, _xAxisDisplay,
                                  xlabel, length, xpos, isXLog(), _xLogBase, true);
    if (_cursorOffset) {
      genOffsetLabel(_xAxisInterpretation, _xAxisDisplay, msgXOffset,
                  _cursor_x, xpos, xmin, xmax, isXLog(), _xLogBase, _isXAxisInterpreted);
    }
  } else {
    double xdelta;
    int iXPrecision;

    xdelta = (xmax-xmin)/(double)pr.width();
    if (isXLog()) {
      xpos = pow(_xLogBase, xpos);
      iXPrecision = (int)(ceil(log10(fabs(xpos))/log10(_xLogBase))-floor(log10(xpos*pow(_xLogBase, xdelta)-xpos)/log10(_xLogBase)));
    } else {
      iXPrecision = (int)(ceil(log10(fabs(xpos)))-floor(log10(xdelta)));
    }
    if (iXPrecision < 1) {
      iXPrecision = 1;
    }
    xlabel = QString::number(xpos,'G',iXPrecision);

    if (_cursorOffset) {
      msgXOffset = QString::number(xpos-_cursor_x,'G',iXPrecision);
    }
  }

  if (_isYAxisInterpreted) {
    genAxisTickLabelFullPrecision(_yAxisInterpretation, _yAxisDisplay,
                                  ylabel, length, ypos, isYLog(), _yLogBase, true);
    if (_cursorOffset) {
      genOffsetLabel(_yAxisInterpretation, _yAxisDisplay, msgYOffset,
                  _cursor_y, ypos, ymin, ymax, isYLog(), _yLogBase, _isYAxisInterpreted);
    }
  } else {
    double ydelta;
    int iYPrecision;

    ydelta = (ymax-ymin)/(double)pr.height();
    if (isYLog()) {
      ypos = pow(_yLogBase, ypos);
      iYPrecision = (int)(ceil(log10(fabs(ypos))/log10(_yLogBase))-floor(log10(ypos*pow(_yLogBase,ydelta)-ypos)/log10(_yLogBase)));
    } else {
      iYPrecision = (int)(ceil(log10(fabs(ypos)))-floor(log10(ydelta)));
    }
    if (iYPrecision < 1) {
      iYPrecision = 1;
    }
    ylabel = QString::number(ypos,'G',iYPrecision);

    if (_cursorOffset) {
      msgYOffset = QString::number(ypos-_cursor_y,'G',iYPrecision);
    }
  }

  if (_cursorOffset) {
    KstApp::inst()->slotUpdateDataMsg(i18nc("(x, y) [Offset: x,y]", "(%1, %2) [Offset: %3, %4]").arg(xlabel).arg(ylabel).arg(msgXOffset).arg(msgYOffset));
  } else {
    KstApp::inst()->slotUpdateDataMsg(i18nc("(x, y)", "(%1, %2)").arg(xlabel).arg(ylabel));
  }
}


void Kst2DPlot::getCursorPos(const QPoint& pos, double& xpos, double& ypos, double& xmin, double &xmax, double& ymin, double& ymax) {
  QRect pr = GetPlotRegion();

  getLScale(xmin, ymin, xmax, ymax);

  // find mouse location in plot units
  if (_xReversed) {
    xpos = (double)(pr.right() - pos.x())/(double)pr.width();
  } else {
    xpos = (double)(pos.x() - pr.left())/(double)pr.width();
  }
  xpos = xpos * (xmax - xmin) + xmin;
  if (isXLog()) {
    xpos = pow(_xLogBase, xpos);
  }

  if (_yReversed) {
    ypos = (double)(pr.bottom() - pos.y())/(double)pr.height();
  } else {
    ypos = (double)(pos.y() - pr.top())/(double)pr.height();
  }
  ypos = ypos * (ymin - ymax) + ymax;
  if (isYLog()) {
    ypos = pow(_yLogBase, ypos);
  }
}


bool Kst2DPlot::getNearestDataPoint(const QPoint& pos, QString& name, double &newxpos, double &newypos, double xpos, double ypos, double xmin, double xmax) {
  bool rc = false;

  // only makes sense to get nearest data point for vcurves
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  if (!vcurves.isEmpty()) {
    QRect pr = GetPlotRegion();
    double near_x, near_y;
    double distance;
    double best_distance = 1.0E300;
    double dx_per_pix;
    int i_near_x;

    // convert 1 pixel to plot units.
    if (_xReversed) {
      dx_per_pix = (double)(pr.right() + 2 - pos.x() + 2) / (double)pr.width() * (xmax - xmin) + xmin;
    } else {
      dx_per_pix = (double)(pos.x() + 2 - pr.left() + 2) / (double)pr.width() * (xmax - xmin) + xmin;
    }
    if (isXLog()) {
      dx_per_pix = pow(_xLogBase, dx_per_pix);
    }
    dx_per_pix -= xpos;

    for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
      i_near_x = (*i)->getIndexNearXY(xpos, dx_per_pix, ypos);
      (*i)->point(i_near_x, near_x, near_y);
      distance = fabs(ypos - near_y);

      if (distance < best_distance || !rc) {
        newypos = near_y;
        newxpos = near_x;
        best_distance = distance;
        name = (*i)->tagName();
        rc = true;
      }
    }
  }

  return rc;
}


void Kst2DPlot::drawCursorPos(QPainter& p) {
  if (_cursorOffset) {
    drawPlusAt(p, _cursor_x, _cursor_y);
  }
}


void Kst2DPlot::drawCursorPos(QWidget *view) {
  QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                    //  view->paint(GetPlotRegion());
  drawCursorPos(p);
}


template<class T>
inline T kstClamp(const T& x, const T& low, const T& high) {
  if (x < low)       return low;
  else if (high < x) return high;
  else               return x;
}


void Kst2DPlot::highlightNearestDataPoint(bool bRepaint, KstPainter *p, const QPoint& pos) {
  QString msg;

  if (!Curves.isEmpty()) {
    int precision = 15;
    QString msgXOffset;
    QString msgYOffset;
    QString name;
    double xpos, ypos;
    double newxpos, newypos;
    double xmin, ymin;
    double xmax, ymax;

    getCursorPos(pos, xpos, ypos, xmin, xmax, ymin, ymax);
    if (getNearestDataPoint(pos, name, newxpos, newypos, xpos, ypos, xmin, xmax)) {
      QString xlabel;
      QString ylabel;

#if 0
      // Determine if we need to expand precision
      precision = kstClamp(int(ceil(log10(1.0 / (xmax - xmin)))) + 2, precision, 15);
      precision = kstClamp(int(ceil(log10(1.0 / (ymax - ymin)))) + 2, precision, 15);
#endif

      if (_copy_x != newxpos || _copy_y != newypos) {
        if (bRepaint && _copy_x != KST::NOPOINT && _copy_y != KST::NOPOINT) {
          drawDotAt(*p, _copy_x, _copy_y);
        }
        _copy_x = newxpos;
        _copy_y = newypos;
        drawDotAt(*p, newxpos, newypos);
      }

      if (_isXAxisInterpreted) {
        uint length;
        genAxisTickLabelFullPrecision(_xAxisInterpretation, _xAxisDisplay,
                                      xlabel, length, newxpos, isXLog(), _xLogBase, true);
        if (_cursorOffset) {
          if (isXLog()) {
            genOffsetLabel(_xAxisInterpretation, _xAxisDisplay, msgXOffset,
                  _cursor_x, log10(newxpos), xmin, xmax, isXLog(), _xLogBase, _isXAxisInterpreted);
          } else {
            genOffsetLabel(_xAxisInterpretation, _xAxisDisplay, msgXOffset,
                  _cursor_x, newxpos, xmin, xmax, isXLog(), _xLogBase, _isXAxisInterpreted);
          }
        }
      } else {
        xlabel = QString::number(newxpos, 'G', precision);
        if (_cursorOffset) {
          msgXOffset = QString::number(newxpos - _cursor_x, 'G', precision);
        }
      }

      if (_isYAxisInterpreted) {
        uint length;
        genAxisTickLabelFullPrecision(_yAxisInterpretation, _yAxisDisplay,
                                      ylabel, length, newypos, isYLog(), _yLogBase, true);
        if (_cursorOffset) {
          if (isYLog()) {
            genOffsetLabel(_yAxisInterpretation, _yAxisDisplay, msgYOffset,
                  _cursor_y, log10(newypos), ymin, ymax, isYLog(), _yLogBase, _isYAxisInterpreted);
          } else {
            genOffsetLabel(_yAxisInterpretation, _yAxisDisplay, msgYOffset,
                  _cursor_y, newypos, ymin, ymax, isYLog(), _yLogBase, _isYAxisInterpreted);
          }
        }
      } else {
        ylabel = QString::number(newypos, 'G', precision);
        if (_cursorOffset) {
          msgYOffset = QString::number(newypos - _cursor_y, 'G', precision);
        }
      }

      if (_cursorOffset) {
        msg = i18nc("Curve name, (x, y) [Offset: x,y]", "%3 (%1, %2) [Offset: %4, %5]").arg(xlabel).arg(ylabel).arg(name).arg(msgXOffset).arg(msgYOffset);
      } else {
        msg = i18nc("Curve name, (x, y)", "%3 (%1, %2)").arg(xlabel).arg(ylabel).arg(name);
      }
    }

    // display the z value of the topmost image underneath cursor, if available...
    KstImageList images = kstObjectSubList<KstBaseCurve,KstImage>(Curves);
    if (images.count() > 0) {
      double zValue;
      bool found = false;
      int i = images.count() - 1;

      while (i >= 0 && !found) {
        if (images[i]->getNearestZ(xpos, ypos, zValue)) {
          found = true;
        }
        i--;
      }

      if (found) {
        QString xlabel;
        QString ylabel;

        if (_isXAxisInterpreted) {
          uint length;
          genAxisTickLabelFullPrecision(_xAxisInterpretation, _xAxisDisplay,
                                        xlabel, length, xpos, isXLog(), _xLogBase, true);
          if (_cursorOffset) {
            if (isXLog()) {
              genOffsetLabel(_xAxisInterpretation, _xAxisDisplay, msgXOffset,
                    _cursor_x, log10(xpos), xmin, xmax, isXLog(), _xLogBase, _isXAxisInterpreted);
            } else {
              genOffsetLabel(_xAxisInterpretation, _xAxisDisplay, msgXOffset,
                    _cursor_x, xpos, xmin, xmax, isXLog(), _xLogBase, _isXAxisInterpreted);
            }
          }
        } else {
          xlabel = QString::number(xpos, 'G', precision);
          if (_cursorOffset) {
            msgXOffset = QString::number(xpos - _cursor_x, 'G', precision);
          }
        }

        if (_isYAxisInterpreted) {
          uint length;
          genAxisTickLabelFullPrecision(_yAxisInterpretation, _yAxisDisplay,
                                        ylabel, length, ypos, isYLog(), _yLogBase, true);
          if (_cursorOffset) {
            if (isYLog()) {
              genOffsetLabel(_yAxisInterpretation, _yAxisDisplay, msgYOffset,
                    _cursor_y, log10(ypos), ymin, ymax, isYLog(), _yLogBase, _isYAxisInterpreted);
            } else {
              genOffsetLabel(_yAxisInterpretation, _yAxisDisplay, msgYOffset,
                    _cursor_y, ypos, ymin, ymax, isYLog(), _yLogBase, _isYAxisInterpreted);
            }
          }
        } else {
          ylabel = QString::number(ypos, 'G', precision);
          if (_cursorOffset) {
            msgYOffset = QString::number(ypos - _cursor_y, 'G', precision);
          }
        }

        if (!msg.isEmpty()) {
          msg = i18nc("Label, Image name (x, y, z)", "%5, %4 (%1, %2, %3)" ).arg(xlabel).arg(ylabel).arg(zValue,0,'G',precision).arg(images[i+1]->tagName()).arg(msg);
        } else {
          msg = i18nc("Image name (x, y, z)", "%4 (%1, %2, %3)" ).arg(xlabel).arg(ylabel).arg(zValue,0,'G', precision).arg(images[i+1]->tagName());
        }
      }
    }
  }

  KstApp::inst()->slotUpdateDataMsg(msg);
}


void Kst2DPlot::updateXYGuideline(QWidget *view, const QPoint& oldPos, const QPoint& newPos, const QRect& pr, KstMouseModeType gzType) {
  //qDebug() << "update guideline for old=" << oldPos << " new=" << newPos << endl;
  KstPainter p; // FIXME: Broken, just prepare and then trigger a
                //  view->paint(GetPlotRegion());
  p.begin(view);
  QPen newPen(Qt::black, 1, Qt::DotLine);
  p.setPen(newPen);
  p.setCompositionMode(QPainter::CompositionMode_Destination);

  if (pr.contains(oldPos)) {
    if (_mouse.lastGuidelineType == X_ZOOMBOX) {
      p.drawLine(oldPos.x(), pr.top(), oldPos.x(), pr.bottom());
    } else if (_mouse.lastGuidelineType == Y_ZOOMBOX) {
      p.drawLine(pr.left(), oldPos.y(), pr.right(), oldPos.y());
    }
  }

  _mouse.lastGuideline = QPoint(-1, -1);

  if (pr.contains(newPos)) {
    if (gzType == X_ZOOMBOX) {
      p.drawLine(newPos.x(), pr.top(), newPos.x(), pr.bottom());
      _mouse.lastGuidelineType = gzType;
      _mouse.lastGuideline = newPos;
    } else if (gzType == Y_ZOOMBOX) {
      p.drawLine(pr.left(), newPos.y(), pr.right(), newPos.y());
      _mouse.lastGuidelineType = gzType;
      _mouse.lastGuideline = newPos;
    }
  }

  p.end();
}


void Kst2DPlot::mouseMoveEvent(QWidget *view, QMouseEvent *e) {
  if (e->pos() == QPoint(-1, -1)) {
    setHasFocus(false);
    KstViewWidget *w = dynamic_cast<KstViewWidget*>(view);
    if (w) {
      w->paint();
    }
    return;
  }

  _mouse.tracker = e->pos();
  QRect pr = GetPlotRegion();

  KstMouseModeType gzType = globalZoomType();
  // Draw a helper guide in X or Y zoom modes
  if (gzType == X_ZOOMBOX || gzType == Y_ZOOMBOX) {
//     Qt::ButtonState s = e->stateAfter();
//     if (s == 0) {
//       updateXYGuideline(view, _mouse.lastGuideline, _mouse.tracker, pr, gzType);
//     } else if (s & Qt::ShiftModifier) {
//       updateXYGuideline(view, _mouse.lastGuideline, _mouse.tracker, pr, Y_ZOOMBOX);
//     } else if (s & Qt::ControlModifier) {
//       updateXYGuideline(view, _mouse.lastGuideline, _mouse.tracker, pr, X_ZOOMBOX);
//     } else {
      updateXYGuideline(view, _mouse.lastGuideline, QPoint(-1, -1), pr, gzType);
//     }
  } else if (gzType == XY_ZOOMBOX) {
//     Qt::ButtonState s = e->stateAfter();
//     if (s & Qt::ShiftModifier) {
//       if (e->state() & Qt::LeftButton && _mouse.zooming()) {
//         updateXYGuideline(view, _mouse.lastGuideline, QPoint(-1, -1), pr, Y_ZOOMBOX);
//       } else {
//         updateXYGuideline(view, _mouse.lastGuideline, _mouse.tracker, pr, Y_ZOOMBOX);
//       }
//     } else if (s & Qt::ControlModifier) {
//       if (e->state() & Qt::LeftButton && _mouse.zooming()) {
//         updateXYGuideline(view, _mouse.lastGuideline, QPoint(-1, -1), pr, X_ZOOMBOX);
//       } else {
//         updateXYGuideline(view, _mouse.lastGuideline, _mouse.tracker, pr, X_ZOOMBOX);
//       }
//     } else {
      updateXYGuideline(view, _mouse.lastGuideline, QPoint(-1, -1), pr, gzType);
//     }
  }

  // Note: we have one report of a system where this clip region is invalid
  // somehow.  Removing it might cause painting to overlap other objects though.
  if (!_hasFocus) {
    KstViewWidget *w = dynamic_cast<KstViewWidget*>(view);
    if (w) {
      w->viewObject()->recursively<bool>(&KstViewObject::setHasFocus, false);
    }
    setHasFocus(true);
    if (w) {
      w->paint();
    }
  }

  KstMouseModeType newType = _mouse.mode;
/*  if (e->state() & Qt::LeftButton && _mouse.zooming()) {
    // LEAVE BLANK
  } else */if (KstApp::inst()->dataMode() && pr.contains(e->pos())) {
    KstViewWidget *w = dynamic_cast<KstViewWidget*>(view);
    if (w) {
      w->paint(GetPlotRegion());
    }
  } else if (pr.contains(e->pos())) {
    updateMousePos(e->pos());
  } else {
    KstApp::inst()->slotUpdateDataMsg(QString::null);
  }

  int x, y;
  if (_mouse.mode == XY_ZOOMBOX) {
    if (e->x() > pr.right()) {
      x = pr.right() + 1;
    } else if (e->x() < pr.left()) {
      x = pr.left();
    } else {
      x = e->x();
    }

    if (e->y() > pr.bottom()) {
      y = pr.bottom() + 1;
    } else if (e->y() < pr.top()) {
      y = pr.top();
    } else {
      y = e->y();
    }

    zoomRectUpdate(view, newType, x, y);
    setCursorForMode(view, _mouse.mode, e->pos());
  } else if (_mouse.mode == Y_ZOOMBOX) {
    x = pr.right();

    if (e->y() > pr.bottom()) {
      y = pr.bottom() + 1;
    } else if (e->y() < pr.top()) {
      y = pr.top();
    } else {
      y = e->y();
    }

    zoomRectUpdate(view, newType, x, y);
    setCursorForMode(view, _mouse.mode, e->pos());
  } else if (_mouse.mode == X_ZOOMBOX) {
    if (e->x() > pr.right()) {
      x = pr.right() + 1;
    } else if (e->x() < pr.left()) {
      x = pr.left();
    } else {
      x = e->x();
    }

    y = pr.bottom();
    zoomRectUpdate(view, newType, x, y);
    setCursorForMode(view, _mouse.mode, e->pos());
  } else {
//     Qt::ButtonState s = e->stateAfter();
    if (pr.contains(e->pos())) {
//       if (s & Qt::ShiftModifier) {
//         setCursorForMode(view, Y_ZOOMBOX, e->pos());
//       } else if (s & Qt::ControlModifier) {
//         setCursorForMode(view, X_ZOOMBOX, e->pos());
//       } else {
        setCursorForMode(view, globalZoomType(), e->pos());
//       }
    } else {
      view->setCursor(QCursor(Qt::ArrowCursor));
    }
  }
}

void Kst2DPlot::mousePressEvent(QWidget *view, QMouseEvent *e) {
  QRect win_rect, plot_rect, tie_rect, plot_and_axis_rect;
  //KstApp *ParentApp = KstApp::inst();

  static_cast<KstViewWidget*>(view)->viewObject()->grabMouse(this);

  // find where the mouse was to determine which mode to be in which button
  if (e->button() == Qt::LeftButton) {
    win_rect = GetWinRegion();
    plot_rect = GetPlotRegion();
    tie_rect = GetTieBoxRegion();
    plot_and_axis_rect = GetPlotAndAxisRegion();
    //qDebug() << e->pos() << " " << win_rect << " " << plot_rect << endl;
    if (tie_rect.contains(e->pos())) {
      toggleTied();
      // So inefficient, but I have some sort of weird bug making it necessary
      static_cast<KstViewWidget*>(view)->paint();
      return;
    } else if (plot_rect.contains(e->pos())) {
//       if (e->state() & Qt::ShiftModifier) {
//         _mouse.mode = Y_ZOOMBOX;
//       } else if (e->state() & Qt::ControlModifier) {
//         _mouse.mode = X_ZOOMBOX;
//       } else {
        _mouse.mode = globalZoomType();
        assert(_mouse.mode != INACTIVE);
//       }
      _mouse.plotGeometry = GetPlotRegion();
      _mouse.zoomStart(_mouse.mode, e->pos());
      _zoomPaused = true;
      return;
    } else if (plot_and_axis_rect.contains(e->pos())) {
      if (e->pos().y() > plot_rect.bottom() && e->pos().x() < plot_rect.left()) {
        _tabToShow = RANGE_TAB;
      } else if (e->pos().y() > plot_rect.bottom()) {
        _tabToShow = X_AXIS_TAB;
      } else {
        _tabToShow = Y_AXIS_TAB;
      }
      KstTopLevelViewPtr tlv = kst_cast<KstTopLevelView>(topLevelParent());
      showDialog(tlv, false);
      _tabToShow = CONTENT_TAB;

      return;
    } else if (win_rect.contains(e->pos())) {
      _tabToShow = APPEARANCE_TAB;
      KstTopLevelViewPtr tlv = kst_cast<KstTopLevelView>(topLevelParent());
      showDialog(tlv, false);
      _tabToShow = CONTENT_TAB;

      return;
    }
  } else if (e->button() == Qt::RightButton) {
    win_rect = GetPlotRegion();
    if (win_rect.contains(e->pos())) {
      _mouse.mode = INACTIVE;
      _mouse.pressLocation = e->pos();
      return;
    }
  } else if (e->button() == Qt::MidButton) {
    win_rect = GetWinRegion();
    if (win_rect.contains(e->pos())) {
      _mouse.mode = INACTIVE;
      _mouse.pressLocation = e->pos();
      zoomPrev(static_cast<KstViewWidget *>(view));
      return;
    }
  } else {
    // cout << "unknown button: " << e->button() << "\n";
  }
}


void Kst2DPlot::mouseReleaseEvent(QWidget *view, QMouseEvent *e) {
  double xmin, xmax, ymin, ymax;
  double new_xmin, new_xmax, new_ymin, new_ymax;
  QRect plotregion;
  bool doUpdate = false;
  _zoomPaused = false;
  KstViewWidget* kstView = static_cast<KstViewWidget*>(view);

  kstView->viewObject()->releaseMouse(this);

  _mouse.tracker = e->pos();

  QRect newg = _mouse.mouseRect();
  if (_mouse.mode == XY_ZOOMBOX) {
    if (_mouse.rectBigEnough()) {
      QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                        //  view->paint(GetPlotRegion());
      p.setPen(Qt::DashLine);
      p.setCompositionMode(QPainter::CompositionMode_Destination);
      p.drawRect(newg);

      getLScale(xmin, ymin, xmax, ymax);
      plotregion = GetPlotRegion();

      if (_xReversed) {
        new_xmin = (double)(plotregion.right() - newg.right());
        new_xmax = (double)(plotregion.right() - newg.left() + 1);
      } else {
        new_xmin = (double)(newg.left() - plotregion.left());
        new_xmax = (double)(newg.right() - plotregion.left() + 1);
      }
      new_xmin = new_xmin/(double)plotregion.width() * (xmax - xmin) + xmin;
      new_xmax = new_xmax/(double)plotregion.width() * (xmax - xmin) + xmin;

      if (_yReversed) {
        new_ymin = (double)(plotregion.bottom() - newg.top() + 1);
        new_ymax = (double)(plotregion.bottom() - newg.bottom());
      } else {
        new_ymin = (double)(newg.bottom() - plotregion.top() + 1);
        new_ymax = (double)(newg.top() - plotregion.top());
      }
      new_ymin = new_ymin/(double)plotregion.height() * (ymin - ymax) + ymax;
      new_ymax = new_ymax/(double)plotregion.height() * (ymin - ymax) + ymax;

      setXScaleMode(FIXED);
      setYScaleMode(FIXED);
      if (setLScale(new_xmin, new_ymin, new_xmax, new_ymax)) {
        pushScale();
        doUpdate = true;
        _mouse.lastLocation = _mouse.pressLocation;
        if (isTied()) {
          KstApp::inst()->tiedZoom(true, new_xmin, new_xmax, true, new_ymin, new_ymax, kstView, tagName());
        }
      }
    }
  } else if (_mouse.mode == Y_ZOOMBOX) {
    if (newg.height() >= _mouse.minMove) {
      QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                        //  view->paint(GetPlotRegion());
      p.setPen(Qt::DashLine);
      p.setCompositionMode(QPainter::CompositionMode_Destination);
      p.drawRect(newg);

      getLScale(xmin, ymin, xmax, ymax);
      plotregion = GetPlotRegion();

      if (_yReversed) {
        new_ymin = (double)(plotregion.bottom() - newg.top() + 1);
        new_ymax = (double)(plotregion.bottom() - newg.bottom());
      } else {
        new_ymin = (double)(newg.bottom() - plotregion.top() + 1);
        new_ymax = (double)(newg.top() - plotregion.top());
      }
      new_ymin = new_ymin/(double)plotregion.height() * (ymin - ymax) + ymax;
      new_ymax = new_ymax/(double)plotregion.height() * (ymin - ymax) + ymax;

      setYScaleMode(FIXED);
      if (setLYScale(new_ymin, new_ymax)) {
        pushScale();
        doUpdate = true;
        _mouse.lastLocation = _mouse.pressLocation;
        if (isTied()) {
          KstApp::inst()->tiedZoom(false, 0.0, 0.0, true, new_ymin, new_ymax, kstView, tagName());
        }
      }
    }
  } else if (_mouse.mode == X_ZOOMBOX) {
    if (newg.width() >= _mouse.minMove) {
      QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                        //  view->paint(GetPlotRegion());
      p.setPen(Qt::DashLine);
      p.setCompositionMode(QPainter::CompositionMode_Destination);
      p.drawRect(newg);

      getLScale(xmin, ymin, xmax, ymax);
      plotregion = GetPlotRegion();

      if (_xReversed) {
        new_xmin = (double)(plotregion.right() - newg.right());
        new_xmax = (double)(plotregion.right() - newg.left() + 1);
      } else {
        new_xmin = (double)(newg.left() - plotregion.left());
        new_xmax = (double)(newg.right() - plotregion.left() + 1);
      }
      new_xmin = new_xmin/(double)plotregion.width() * (xmax - xmin) + xmin;
      new_xmax = new_xmax/(double)plotregion.width() * (xmax - xmin) + xmin;

      setXScaleMode(FIXED);
      if (setLXScale(new_xmin, new_xmax)) {
        pushScale();
        doUpdate = true;
        _mouse.lastLocation = _mouse.pressLocation;
        if (isTied()) {
          KstApp::inst()->tiedZoom(true, new_xmin, new_xmax, false, 0.0, 0.0, kstView, tagName());
        }
      }
    }
  }
 
  _mouse.mode = INACTIVE;
  setCursorForMode(view, _mouse.mode, e->pos());

  if (doUpdate) {
    qDebug() << "mouse release: do update" << endl;
    setDirty();
    static_cast<KstViewWidget*>(view)->paint();
  }
}


KstMouse::KstMouse() {
  mode = INACTIVE;
  label = -1;
  minMove = 2;
  lastLocation = QPoint(-1, -1);
  tracker = QPoint(-1, -1);
  lastGuideline = QPoint(-1, -1);
  lastGuidelineType = XY_ZOOMBOX;
}


void KstMouse::zoomUpdate(KstMouseModeType t, const QPoint& location) {
  mode = t;
  lastLocation = location;
}


void KstMouse::zoomStart(KstMouseModeType t, const QPoint& location) {
  mode = t;
  pressLocation = lastLocation = location;
}


bool KstMouse::rectBigEnough() const {
  QRect r = mouseRect();
  return r.width() >= minMove && r.height() >= minMove;
}


QRect KstMouse::mouseRect() const {
  QRect rc = QRect(qMin(pressLocation.x(), lastLocation.x()), qMin(pressLocation.y(), lastLocation.y()), qAbs(pressLocation.x() - lastLocation.x()), qAbs(pressLocation.y() - lastLocation.y()));
  switch (mode) {
    case X_ZOOMBOX:
      rc.setTop(plotGeometry.top());
      rc.setBottom(plotGeometry.bottom());
      break;
    case Y_ZOOMBOX:
      rc.setLeft(plotGeometry.left());
      rc.setRight(plotGeometry.right());
      break;
    default:
      break;
  }
  return rc;
}


bool KstMouse::zooming() const {
  return mode == XY_ZOOMBOX || mode == X_ZOOMBOX || mode == Y_ZOOMBOX;
}


void Kst2DPlot::zoomRectUpdate(QWidget *view, KstMouseModeType t, int x, int y) {
  QPoint newp(x, y);

  if (_mouse.lastLocation != newp) {
    QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                      //  view->paint(GetPlotRegion());
    p.setPen(Qt::DashLine);
    p.setCompositionMode(QPainter::CompositionMode_Destination);
    if (_mouse.rectBigEnough()) {
      p.drawRect(_mouse.mouseRect());
    }
    _mouse.zoomUpdate(t, newp);
    if (_mouse.rectBigEnough()) {
      p.drawRect(_mouse.mouseRect());
    }
  }
}


void Kst2DPlot::setCursorForMode(QWidget *view, KstMouseModeType mode, const QPoint& pos) {
  switch (mode) {
    case Y_ZOOMBOX:
      view->setCursor(QCursor(Qt::SizeVerCursor));
      break;
    case X_ZOOMBOX:
      view->setCursor(QCursor(Qt::SizeHorCursor));
      break;
    case XY_ZOOMBOX:
      view->setCursor(QCursor(Qt::CrossCursor));
      break;
    default:
      if (GetPlotRegion().contains(pos)) {
        view->setCursor(QCursor(Qt::CrossCursor));
      } else {
        view->setCursor(QCursor(Qt::ArrowCursor));
      }
      break;
  }
}


void Kst2DPlot::keyReleaseEvent(QWidget *view, QKeyEvent *e) {
  if (_mouse.mode != INACTIVE) {
    e->ignore();
    return;
  }

  KstMouseModeType newType = globalZoomType();
  QPoint c = _mouse.lastLocation;
  QRect pr = GetPlotRegion();
  int x = _mouse.pressLocation.x();
  int y = _mouse.pressLocation.y();

  if (newType == Y_ZOOMBOX) {
    if (c.y() > pr.bottom()) {
      y = pr.bottom() + 1;
    } else if (c.y() < pr.top()) {
      y = pr.top();
    } else {
      y = c.y();
    }
  } else if (newType == X_ZOOMBOX) {
    if (c.x() > pr.right()) {
      x = pr.right() + 1;
    } else if (c.x() < pr.left()) {
      x = pr.left();
    } else {
      x = c.x();
    }
  } else {
    if (c.x() > pr.right()) {
      x = pr.right() + 1;
    } else if (c.x() < pr.left()) {
      x = pr.left();
    } else {
      x = c.x();
    }

    if (c.y() > pr.bottom()) {
      y = pr.bottom() + 1;
    } else if (c.y() < pr.top()) {
      y = pr.top();
    } else {
      y = c.y();
    }
  }

  if (_mouse.zooming()) {
    QPoint newp(x, y);
    QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                      //  view->paint(GetPlotRegion());
    p.setPen(Qt::DashLine);
    p.setCompositionMode(QPainter::CompositionMode_Destination);
    if (_mouse.rectBigEnough()) {
      p.drawRect(_mouse.mouseRect());
    }

    _mouse.zoomUpdate(newType, newp);
    if (_mouse.rectBigEnough()) {
      p.drawRect(_mouse.mouseRect());
    }
  }

  if (e->key() == Qt::Key_Shift) {
    updateXYGuideline(view, _mouse.lastGuideline, QPoint(-1, -1), GetPlotRegion(), Y_ZOOMBOX);
  } else if (e->key() == Qt::Key_Control) {
    updateXYGuideline(view, _mouse.lastGuideline, QPoint(-1, -1), GetPlotRegion(), X_ZOOMBOX);
  }

  setCursorForMode(view, newType, c);
  e->accept();
}


void Kst2DPlot::cancelZoom(QWidget *view) {
  if (_mouse.rectBigEnough()) {
    QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                      //  view->paint(GetPlotRegion());
    p.setPen(Qt::DashLine);
    p.setCompositionMode(QPainter::CompositionMode_Destination);
    p.drawRect(_mouse.mouseRect());
  }

  _mouse.lastLocation = _mouse.pressLocation; // make rectBigEnough() false
  _mouse.mode = INACTIVE;
}


void Kst2DPlot::menuXZoomMax() {
  if (_menuView) {
    xZoomMax(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuYZoomMax() {
  if (_menuView) {
    yZoomMax(_menuView);
    _menuView->paint();
  }
}

void Kst2DPlot::menuYZoomLocalMax() {
  if (_menuView) {
    yZoomLocalMax(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuZoomMax() {
  if (_menuView) {
    zoomMax(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuXLogSlot() {
  if (_menuView) {
    xLogSlot(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuYLogSlot() {
  if (_menuView) {
    yLogSlot(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuZoomPrev() {
  if (_menuView) {
    zoomPrev(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuYZoomAc() {
  if (_menuView) {
    yZoomAc(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuZoomSpikeInsensitiveMax() {
  if (_menuView) {
    zoomSpikeInsensitiveMax(_menuView);
    _menuView->paint();
  }
}

void Kst2DPlot::menuNextImageColorScale() {
  if (_menuView) {
    nextImageColorScale();
    _menuView->paint();
  }
}

void Kst2DPlot::menuXZoomIn() {
  if (_menuView) {
    xZoomIn(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuXZoomOut() {
  if (_menuView) {
    xZoomOut(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuYZoomIn() {
  if (_menuView) {
    yZoomIn(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuYZoomOut() {
  if (_menuView) {
    yZoomOut(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuXNormalize() {
  if (_menuView) {
    xZoomNormal(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuYNormalize() {
  if (_menuView) {
    yZoomNormal(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuMoveUp() {
  if (_menuView) {
    moveUp(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuMoveDown() {
  if (_menuView) {
    moveDown(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuMoveLeft() {
  if (_menuView) {
    moveLeft(_menuView);
    _menuView->paint();
  }
}


void Kst2DPlot::menuMoveRight() {
  if (_menuView) {
    moveRight(_menuView);
    _menuView->paint();
  }
}

void Kst2DPlot::menuNextMarker() {
  if (_menuView) {
    moveToNextMarker(_menuView);
    _menuView->paint();
  }
}

void Kst2DPlot::menuPrevMarker() {
  if (_menuView) {
    moveToPrevMarker(_menuView);
    _menuView->paint();
  }
}

void Kst2DPlot::moveLeft(KstViewWidget *view) {
  if (moveSelfHorizontal(true)) {
    KstApp::inst()->tiedZoomMode(ZOOM_MOVE_HORIZONTAL, true, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::moveRight(KstViewWidget *view) {
  if (moveSelfHorizontal(false)) {
    KstApp::inst()->tiedZoomMode(ZOOM_MOVE_HORIZONTAL, false, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::moveUp(KstViewWidget *view) {
  if (moveSelfVertical(true)) {
    KstApp::inst()->tiedZoomMode(ZOOM_MOVE_VERTICAL, true, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::moveDown(KstViewWidget *view) {
  if (moveSelfVertical(false)) {
    KstApp::inst()->tiedZoomMode(ZOOM_MOVE_VERTICAL, false, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}

void Kst2DPlot::moveToNextMarker(KstViewWidget *view) {
  double newCenter, currCenter;
  double xmin, xmax, ymin, ymax;
  double new_xmin, new_xmax;

  getLScale(xmin, ymin, xmax, ymax);
  currCenter = ((xmax + xmin) / 2.0) + (xmax - xmin)/MARKER_NUM_SEGS;
  if (_xLog) {
    currCenter = pow(_xLogBase, currCenter);
  }
  if (nextMarker(currCenter, newCenter)) {
    if (_xLog) {
      newCenter = logXLo(newCenter);
    }
    new_xmin = newCenter - (xmax - xmin)/2.0;
    new_xmax = newCenter + (xmax - xmin)/2.0;
    setXScaleMode(FIXED);
    setLXScale(new_xmin, new_xmax);

    // now move all all the other tied plots to the same center
    if (_xLog) {
      newCenter = pow(_xLogBase, newCenter);
    }
    KstApp::inst()->tiedZoomMode(ZOOM_CENTER, true, newCenter, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
    view->paint();
  }
}

void Kst2DPlot::moveToPrevMarker(KstViewWidget *view) {
  double newCenter, currCenter;
  double xmin, xmax, ymin, ymax;
  double new_xmin, new_xmax;

  getLScale(xmin, ymin, xmax, ymax);
  currCenter = ((xmax + xmin) / 2.0) - (xmax - xmin)/MARKER_NUM_SEGS;
  if (_xLog) {
    currCenter = pow(_xLogBase, currCenter);
  }
  if (prevMarker(currCenter, newCenter)) {
    if (_xLog) {
      if (newCenter > 0) {
        newCenter = logXLo(newCenter);
      } else {
        return; //don't scroll left past 0 in log mode
      }
    }
    new_xmin = newCenter - (xmax - xmin)/2.0;
    new_xmax = newCenter + (xmax - xmin)/2.0;
    setXScaleMode(FIXED);
    setLXScale(new_xmin, new_xmax);

    // now move all the other tied plots to the same center
    if (_xLog) {
      newCenter = pow(_xLogBase, newCenter);
    }
    KstApp::inst()->tiedZoomMode(ZOOM_CENTER, true, newCenter, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
    view->paint();
  }
}


void Kst2DPlot::zoomSelfYLocalMax(bool unused) {
  Q_UNUSED(unused);

  double YMinCurve, YMaxCurve;

  // find local minimum and maximum
  YMin = 0.0;
  YMax = 1.0;
  bool first = true;

  // first check all the curves
  for (unsigned i = 0; i < Curves.count(); i++) {
    KstBaseCurvePtr c = Curves[i];
    c->readLock();
    if (!c->ignoreAutoScale()) {
      c->yRange(XMin, XMax, &YMinCurve, &YMaxCurve);
      if (first || YMinCurve < YMin ) {
        YMin = YMinCurve;
      }
      if (first || YMaxCurve > YMax ) {
        YMax = YMaxCurve;
      }
      first = false;
    }
    c->unlock();
  }

  // if curves/images had no variation in them
  if (YMax <= YMin) {
    YMin -= 0.1;
    YMax  = YMin + 0.2;
  }
  if (_yLog && YMin <= 0.0) {
    YMin = pow(_yLogBase, -350.0);
  }

  QPair<double, double> borders = computeAutoBorder(_yLog, _yLogBase, YMin, YMax);
  YMin = borders.first;
  YMax = borders.second;

  setYScaleMode(FIXED);
}

void Kst2DPlot::moveSelfToCenter(double center) {
  // log the center if necessary
  if (_xLog) {
    center = logXLo(center);
  }

  // refuse to move if it's not possible
  if (_xLog && center <= -350) {
    return;
  }

  double xmin, xmax, ymin, ymax;
  getLScale(xmin, ymin, xmax, ymax);

  double new_xmin = center - (xmax - xmin)/2.0;
  double new_xmax = center + (xmax - xmin)/2.0;
  setXScaleMode(FIXED);
  setLXScale(new_xmin, new_xmax);
}


bool Kst2DPlot::moveSelfHorizontal(bool left) {
  double xmin, xmax, ymin, ymax;
  double new_xmin, new_xmax;

  getLScale(xmin, ymin, xmax, ymax);

  if (left) {
    new_xmin = xmin - (xmax - xmin)*0.1;
    new_xmax = xmax - (xmax - xmin)*0.1;
  } else {
    new_xmin = xmin + (xmax - xmin)*0.1;
    new_xmax = xmax + (xmax - xmin)*0.1;
  }

  setXScaleMode(FIXED);
  return setLXScale(new_xmin, new_xmax);
}


bool Kst2DPlot::moveSelfVertical(bool up) {
  double xmin, xmax, ymin, ymax;
  double new_ymin, new_ymax;

  getLScale(xmin, ymin, xmax, ymax);

  if (up) {
    new_ymin = ymin + (ymax - ymin)*0.25;
    new_ymax = ymax + (ymax - ymin)*0.25;
  } else {
    new_ymin = ymin - (ymax - ymin)*0.25;
    new_ymax = ymax - (ymax - ymin)*0.25;
  }

  setYScaleMode(FIXED);
  return setLYScale(new_ymin, new_ymax);
}


bool Kst2DPlot::zoomSelfVertical(bool in) {
  double xmin, xmax, ymin, ymax;
  double new_ymin, new_ymax;

  getLScale(xmin, ymin, xmax, ymax);

  if (in) {
    new_ymin = ymin + (ymax - ymin)*0.1666666;
    new_ymax = ymax - (ymax - ymin)*0.1666666;
  } else {
    new_ymin = ymin - (ymax - ymin)*0.25;
    new_ymax = ymax + (ymax - ymin)*0.25;
  }

  setYScaleMode(FIXED);
  return setLYScale(new_ymin, new_ymax);
}


void Kst2DPlot::yZoomIn(KstViewWidget *view) {
  if (zoomSelfVertical(true)) {
    KstApp::inst()->tiedZoomMode(ZOOM_VERTICAL, true, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::yZoomOut(KstViewWidget *view) {
  if (zoomSelfVertical(false)) {
    KstApp::inst()->tiedZoomMode(ZOOM_VERTICAL, false, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}


bool Kst2DPlot::zoomSelfHorizontal(bool in) {
  double xmin, xmax, ymin, ymax;
  double new_xmin, new_xmax;

  getLScale(xmin, ymin, xmax, ymax);

  if (in) {
    new_xmin = xmin + (xmax - xmin)*0.1666666;
    new_xmax = xmax - (xmax - xmin)*0.1666666;
  } else {
    new_xmin = xmin - (xmax - xmin)*0.25;
    new_xmax = xmax + (xmax - xmin)*0.25;
  }

  setXScaleMode(FIXED);
  return setLXScale(new_xmin, new_xmax);
}


void Kst2DPlot::xZoomIn(KstViewWidget *view) {
  if (zoomSelfHorizontal(true)) {
    KstApp::inst()->tiedZoomMode(ZOOM_HORIZONTAL, true, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::xZoomOut(KstViewWidget *view) {
  if (zoomSelfHorizontal(false)) {
    KstApp::inst()->tiedZoomMode(ZOOM_HORIZONTAL, false, 0.0, AUTO, AUTO, view, tagName());
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::xZoomNormal(KstViewWidget *view) {
  if (!isXLog() && !isYLog()) {
    double xmin, xmax, ymin, ymax;
    double new_xmin, new_xmax;
    double mean;
    double range;

    getLScale(xmin, ymin, xmax, ymax);
    mean = xmin + ((xmax - xmin) / 2.0);
    range = (double)PlotRegion.width() * (ymax - ymin) / (double)PlotRegion.height();

    new_xmin = mean - (range / 2.0);
    new_xmax = mean + (range / 2.0);
    setXScaleMode(FIXED);
    setXScale(new_xmin, new_xmax);
    if (isTied()) {
      KstApp::inst()->tiedZoom(true, new_xmin, new_xmax, false, 0.0, 0.0, view, tagName());
    }
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::yZoomNormal(KstViewWidget *view) {
  if (!isXLog() && !isYLog()) {
    double xmin, xmax, ymin, ymax;
    double new_ymin, new_ymax;
    double mean;
    double range;

    getLScale(xmin, ymin, xmax, ymax);
    mean = ymin + ((ymax - ymin) / 2.0);
    range = (double)PlotRegion.height() * (xmax - xmin) / (double)PlotRegion.width();

    new_ymin = mean - (range / 2.0);
    new_ymax = mean + (range / 2.0);
    setYScaleMode(FIXED);
    setYScale(new_ymin, new_ymax);
    if (isTied()) {
      KstApp::inst()->tiedZoom(false, 0.0, 0.0, true, new_ymin, new_ymax, view, tagName());
    }
    pushScale();
    setDirty();
  }
}


void Kst2DPlot::xZoomMax(KstViewWidget *view) {
  setXScaleMode(_xScaleModeDefault);
  KstApp::inst()->tiedZoomMode(ZOOM_X_MODE, true, 0.0, _xScaleModeDefault, AUTO, view, tagName());
  pushScale();
  setDirty();
}


void Kst2DPlot::yZoomMax(KstViewWidget *view) {
  setYScaleMode(_yScaleModeDefault);
  KstApp::inst()->tiedZoomMode(ZOOM_Y_MODE, true, 0.0, _yScaleModeDefault, AUTO, view, tagName());
  pushScale();
  setDirty();
}


void Kst2DPlot::yZoomLocalMax(KstViewWidget *view) {
  zoomSelfYLocalMax(true);
  KstApp::inst()->tiedZoomMode(ZOOM_Y_LOCAL_MAX, true, 0.0, AUTO, AUTO, view, tagName());
  pushScale();
  setDirty();
}


void Kst2DPlot::zoomMax(KstViewWidget *view) {
  setXScaleMode(_xScaleModeDefault);
  setYScaleMode(_yScaleModeDefault);
  KstApp::inst()->tiedZoomMode(ZOOM_XY_MODES, true, 0.0, _xScaleModeDefault, _yScaleModeDefault, view, tagName());
  pushScale();
  setDirty();
}


void Kst2DPlot::xLogSlot(KstViewWidget *view) {
  Q_UNUSED(view)

  setLog(!isXLog(), isYLog());
  setDirty();
}


void Kst2DPlot::yLogSlot(KstViewWidget *view) {
  Q_UNUSED(view)

  setLog(isXLog(), !isYLog());
  setDirty();
}


void Kst2DPlot::zoomPrev(KstViewWidget *view) {
  if (popScale()) {
    if (isTied()) {
      KstApp::inst()->tiedZoomPrev(view, tagName());
    }
    setDirty();
  }
}


void Kst2DPlot::yZoomAc(KstViewWidget *view) {
  setYScaleMode(AC);
  pushScale();
  KstApp::inst()->tiedZoomMode(ZOOM_Y_MODE, true, 0.0, AC, AC, view, tagName());
  setDirty();
}


void Kst2DPlot::zoomSpikeInsensitiveMax(KstViewWidget *view) {
  setXScaleMode(NOSPIKE);
  setYScaleMode(NOSPIKE);
  KstApp::inst()->tiedZoomMode(ZOOM_XY_MODES, true, 0.0, NOSPIKE, NOSPIKE, view, tagName());
  pushScale();
  setDirty();
}

void Kst2DPlot::nextImageColorScale() {
  const double per[5] = {0.0, 0.0001, 0.001, 0.005, 0.02};
  if (++_i_per>=5) {
    _i_per = 0;
  }
  KstImageList images = kstObjectSubList<KstBaseCurve,KstImage>(Curves);
  for (KstImageList::Iterator i = images.begin(); i != images.end(); ++i) {
    (*i)->setThresholdToSpikeInsensitive(per[_i_per]);
  }
}

void Kst2DPlot::keyPressEvent(QWidget *vw, QKeyEvent *e) {
  KstViewWidget *view = static_cast<KstViewWidget*>(vw);
  bool handled = true;
  bool paint = true;

//   Qt::ButtonState s = e->stateAfter();
  QPoint cursorPos = _mouse.tracker;
  switch (e->key()) {
    case Qt::Key_A:
      yZoomAc(view);
      break;
    case Qt::Key_C:
//       if (s & Qt::ShiftModifier) {
//         unsetCursorPos(view);
//       } else {
        setCursorPos(view);
//       }
      break;
    case Qt::Key_E:
      edit();
      break;
    case Qt::Key_G:
      xLogSlot(view);
      break;
    case Qt::Key_L:
//       if (s & Qt::ShiftModifier) {
//         yZoomLocalMax(view);
//       } else {
        yLogSlot(view);
//       }
      break;
    case Qt::Key_M:
//       if (s & Qt::ShiftModifier) {
//         yZoomMax(view);
//       } else if (s & Qt::ControlModifier) {
//         xZoomMax(view);
//       } else {
        zoomMax(view);
//       }
      break;
    case Qt::Key_N:
//       if (s & Qt::ShiftModifier) {
//         yZoomNormal(view);
//       } else {
        xZoomNormal(view);
//       }
      break;
    case Qt::Key_P:
      pauseToggle();
      setDirty();
      break;
    case Qt::Key_R:
      if (_plotScaleList.count() <= 1) {
        // Don't paint
        handled = false;
      } else {
        zoomPrev(view);
      }
      break;
    case Qt::Key_S:
      zoomSpikeInsensitiveMax(view);
      break;
    case Qt::Key_I:
      nextImageColorScale();
      break;
    case Qt::Key_Z:
      zoomToggle();
      cancelZoom(view);
      setDirty();
      break;
    case Qt::Key_Left:
//       if (s & Qt::ShiftModifier) {
//         xZoomIn(view);
//       } else if (s & Qt::AltModifier) {
//         if (_xReversed) {
//           moveToNextMarker(view);
//         } else {
//           moveToPrevMarker(view);
//         }
//       } else {
        if (_xReversed) {
          moveRight(view);
        } else {
          moveLeft(view);
        }
//       }
      break;
    case Qt::Key_Right:
//       if (s & Qt::ShiftModifier) {
//         xZoomOut(view);
//       } else if (s & Qt::AltModifier) {
//         if (_xReversed) {
//           moveToPrevMarker(view);
//         } else {
//           moveToNextMarker(view);
//         }
//       } else {
        if (_xReversed) {
          moveLeft(view);
        } else {
          moveRight(view);
        }
//       }
      break;
    case Qt::Key_Up:
//       if (s & Qt::ShiftModifier) {
//         yZoomOut(view);
//       } else {
        if (_yReversed) {
          moveDown(view);
        } else {
          moveUp(view);
        }
//       }
      break;
    case Qt::Key_Down:
//       if (s & Qt::ShiftModifier) {
//         yZoomIn(view);
//       } else {
        if (_yReversed) {
          moveUp(view);
        } else {
          moveDown(view);
        }
//       }
      break;
    case Qt::Key_Insert:
      if (!e->isAutoRepeat() && GetPlotRegion().contains(cursorPos)) {
        double fakeCursorPos = cursorPos.x();
        if (_xReversed) {
          fakeCursorPos = PlotRegion.right() - (fakeCursorPos - PlotRegion.left());
        }
        if (_xLog) {
          setPlotMarker(pow(_xLogBase, ((fakeCursorPos - position().x() - _b_X) / _m_X)), false, false, false);
        } else {
          setPlotMarker((fakeCursorPos - position().x() - _b_X) / _m_X, false, false, false);
        }
        setDirty();
      }
      break;
    case Qt::Key_Shift:
      updateXYGuideline(view, _mouse.lastGuideline, _mouse.tracker, GetPlotRegion(), Y_ZOOMBOX);
      setCursorForMode(view, Y_ZOOMBOX, _mouse.tracker);
      paint = false;
      break;
    case Qt::Key_Control:
      updateXYGuideline(view, _mouse.lastGuideline, _mouse.tracker, GetPlotRegion(), X_ZOOMBOX);
      setCursorForMode(view, X_ZOOMBOX, _mouse.tracker);
      paint = false;
      break;
    default:
      handled = false;
      break;
  }

  if (handled) {
    if (paint) {
      view->paint();
    }
    e->accept();
    //emit modified();
    return;
  }

  if (_mouse.zooming()) {
    KstMouseModeType newType = _mouse.mode;

    if (e->key() == Qt::Key_Escape) {
      cancelZoom(view);
    } else {
      QPoint newp = _mouse.lastLocation;

      QPainter p(view); // FIXME: Broken, just prepare and then trigger a
                        //  view->paint(GetPlotRegion());
      p.setPen(Qt::DashLine);
      p.setCompositionMode(QPainter::CompositionMode_Destination);
      if (_mouse.rectBigEnough()) {
        p.drawRect(_mouse.mouseRect());
      }

      _mouse.zoomUpdate(newType, newp);
      if (_mouse.rectBigEnough()) {
        p.drawRect(_mouse.mouseRect());
      }
    }
    setCursorForMode(view, _mouse.mode, cursorPos);
  } else {
    if (_mouse.mode == INACTIVE && GetPlotRegion().contains(cursorPos)) {
//       if (s & Qt::ShiftModifier) {
//         setCursorForMode(view, Y_ZOOMBOX, cursorPos);
//       } else if (s & Qt::ControlModifier) {
//         setCursorForMode(view, X_ZOOMBOX, cursorPos);
//       } else {
        setCursorForMode(view, globalZoomType(), cursorPos);
//       }
    } else {
      e->ignore();
      return;
    }
  }
  e->accept();
}


KstMouseModeType Kst2DPlot::globalZoomType() const {
  switch (KstApp::inst()->getZoomRadio()) {
    case KstApp::XZOOM:
      return X_ZOOMBOX;
    case KstApp::YZOOM:
      return Y_ZOOMBOX;
    case KstApp::XYZOOM:
      return XY_ZOOMBOX;
    case KstApp::LAYOUT:
      return LAYOUT_TOOL;
    default:
      break;
  }

  return INACTIVE;
}


void Kst2DPlot::copy() {
  // Don't set the selection because while it does make sense, it
  // is far too easy to swipe over Kst while trying to paste a selection
  // from one window to another.

  // FIXME: we should also provide a custom mime source so that we can
  //        actually manipulate points of data within Kst.
  // FIXME: if we are over an image, we should also return Z.
  QString msg = i18n("%1 %2").arg(_copy_x, 0, 'G').arg(_copy_y, 0, 'G');
  QApplication::clipboard()->setText(msg);
}


Kst2DPlotPtr Kst2DPlot::findPlotByName(const QString& name) {
  Kst2DPlotPtr rc;
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *it = app->createIterator();
  if (it) {
    while (it->currentItem()) {
      KstViewWindow *view = dynamic_cast<KstViewWindow*>(it->currentItem());
      if (view) {
        rc = kst_cast<Kst2DPlot>(view->view()->findChild(name));
        if (rc) {
          break;
        }
      }
      it->next();
    }
    app->deleteIterator(it);
  }
  return rc;
}


// FIXME: this does not strip out dupes.  Why?  Because we can't have them
//        by definition at present.  this might change one day.  Unfortunately
//        removal of dupes makes O(n) code become O(n^2).
Kst2DPlotList Kst2DPlot::globalPlotList() {
  Kst2DPlotList rc;
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *it = app->createIterator();
  if (it) {
    while (it->currentItem()) {
      KstViewWindow *view = dynamic_cast<KstViewWindow*>(it->currentItem());
      if (view) {
        Kst2DPlotList sub = view->view()->findChildrenType<Kst2DPlot>(true);
        rc += sub;
      }
      it->next();
    }
    app->deleteIterator(it);
  }
  return rc;
}


#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

void Kst2DPlot::wheelEvent(QWidget *view, QWheelEvent *e) {
  KstViewWidget *vw = dynamic_cast<KstViewWidget*>(view);
  if (vw && GetPlotRegion().contains(e->pos())) {
    bool forward = e->delta() >= 0;
    int absDelta = forward ? e->delta() : -e->delta();
//     bool alt = e->state() & Qt::AltModifier;
//     if (e->state() & Qt::ControlModifier) {
//       for (int i = 0; i < absDelta/WHEEL_DELTA; ++i) {
//         if (forward) {
//           xZoomIn(vw);
//         } else {
//           xZoomOut(vw);
//         }
//       }
//       vw->paint();
//     } else if (e->state() & Qt::ShiftModifier) {
//       for (int i = 0; i < absDelta/WHEEL_DELTA; ++i) {
//         if (forward) {
//           yZoomIn(vw);
//         } else {
//           yZoomOut(vw);
//         }
//       }
//       vw->paint();
//     } else {
      for (int i = 0; i < absDelta/WHEEL_DELTA; ++i) {
        if (forward) {
//           if (alt) {
//             if (_yReversed) {
//               moveDown(vw);
//             } else {
//               moveUp(vw);
//             }
//           } else {
            if (_xReversed) {
              moveLeft(vw);
            } else {
              moveRight(vw);
            }
//           }
        } else {
//           if (alt) {
//             if (_yReversed) {
//               moveUp(vw);
//             } else {
//               moveDown(vw);
//             }
//           } else {
            if (_xReversed) {
              moveRight(vw);
            } else {
              moveLeft(vw);
            }
//           }
        }
      }
      vw->paint();
//     }

    e->accept();
  }
}


bool Kst2DPlot::setPlotMarker(const double xValue, bool isRising, bool isFalling, bool isVectorValue) {
  KstMarker marker;

  KstMarkerList::Iterator iter = _plotMarkers.begin();
  while (iter != _plotMarkers.end() && (*iter).value < xValue) {
    ++iter;
  }
  if (iter == _plotMarkers.end() || (*iter).value != xValue) {
    marker.value = xValue;
    marker.isRising = isRising;
    marker.isFalling = isFalling;
    marker.isVectorValue = isVectorValue;
    _plotMarkers.insert(iter, marker);

    return true;
  }

  return false;
}

/*
bool Kst2DPlot::removePlotMarker(const double xValue) {
  return _plotMarkers.remove(xValue);
}
*/

const KstMarkerList Kst2DPlot::plotMarkers(const double minX, const double maxX) const {
  KstMarkerList foundMarkers;
  KstMarkerList::ConstIterator marks_iter = _plotMarkers.begin();
  while (marks_iter != _plotMarkers.end()) {
    if ((*marks_iter).value < maxX && (*marks_iter).value > minX) {
      foundMarkers.append(*marks_iter);
    }
    ++marks_iter;
  }
  return KstMarkerList(foundMarkers);
}


const KstMarkerList& Kst2DPlot::plotMarkers() const {
  return _plotMarkers;
}


void Kst2DPlot::setPlotMarkerList(const KstMarkerList& newMarkers) {
  _plotMarkers = KstMarkerList(newMarkers);
}


bool Kst2DPlot::nextMarker(const double currentPosition, double& marker) {
  KstMarkerList::Iterator iter = _plotMarkers.begin();
  while (iter != _plotMarkers.end() && (*iter).value < currentPosition) {
    ++iter;
  }
  if (iter == _plotMarkers.end()) {
    return false;
  }
  marker = (*iter).value;

  return true;
}


bool Kst2DPlot::prevMarker(const double currentPosition, double& marker) {
  KstMarkerList::Iterator iter = _plotMarkers.begin();
  if (iter == _plotMarkers.end() || (*iter).value >= currentPosition) {
    return false;
  }
  while (iter != _plotMarkers.end() && (*iter).value < currentPosition) {
    ++iter;
  }
  --iter;
  marker = (*iter).value;
  return true;
}


void Kst2DPlot::setCurveToMarkers(KstVCurvePtr curve, bool risingDetect, bool fallingDetect) {
  _curveToMarkers = curve;
  _curveToMarkersRisingDetect = risingDetect;
  _curveToMarkersFallingDetect = fallingDetect;
}


bool Kst2DPlot::hasCurveToMarkers() const {
  return _curveToMarkers != 0L;
}


void Kst2DPlot::removeCurveToMarkers() {
  _curveToMarkers = 0L;
}


KstVCurvePtr Kst2DPlot::curveToMarkers() const {
  return _curveToMarkers;
}


void Kst2DPlot::updateMarkersFromCurve() {
  if (hasCurveToMarkers()) {
    _curveToMarkers->readLock();

    int count = _curveToMarkers->sampleCount();

    if (count > 0) {
      double prevX, prevY;
      double curX, curY;

      // scan through the whole curve
      _curveToMarkers->point(0, prevX, prevY);
      for (int i = 1; i < count; i++) {
        _curveToMarkers->point(i, curX, curY);
        if (_curveToMarkersRisingDetect && prevY == 0.0 && curY > 0.0) {
          setPlotMarker(curX, true, false, false);
        }
        if (_curveToMarkersFallingDetect && prevY > 0.0 && curY == 0.0) {
          setPlotMarker(prevX, false, true, false);
        }
        prevX = curX;
        prevY = curY;
      }
    }
    _curveToMarkers->unlock();
  }
}


bool Kst2DPlot::curveToMarkersRisingDetect() const {
  return _curveToMarkersRisingDetect;
}


bool Kst2DPlot::curveToMarkersFallingDetect() const {
  return _curveToMarkersFallingDetect;
}


void Kst2DPlot::setVectorToMarkers(KstVectorPtr vector) {
  _vectorToMarkers = vector;
}


bool Kst2DPlot::hasVectorToMarkers() const {
  return _vectorToMarkers != 0L;
}


void Kst2DPlot::removeVectorToMarkers() {
  _vectorToMarkers = 0L;
}


KstVectorPtr Kst2DPlot::vectorToMarkers() const {
  return _vectorToMarkers;
}


void Kst2DPlot::updateMarkersFromVector() {
  if (hasVectorToMarkers()) {
    _vectorToMarkers->readLock();

    int count = _vectorToMarkers->length();

    for (int i = 0; i < count; ++i) {
      setPlotMarker(_vectorToMarkers->value(i), false, false, true);
    }

    _vectorToMarkers->unlock();
  }
}


void Kst2DPlot::editMatrix(int id) {
  KstDialogs::self()->showMatrixDialog(_objectEditMap[id], true);
}


void Kst2DPlot::plotPlotMarkers(KstPainter& p,
                           double m_X, double b_X, double x_max, double x_min,
                           double y_px, double ytop_bdr_px, double ybot_bdr_px) {
  const int width = lineWidthMarkers() * p.lineWidthAdjustmentFactor();

  if (defaultColorMarker()) {
    p.setPen(QPen(foregroundColor(), width, KstLineStyle[lineStyleMarkers()]));
  } else {
    p.setPen(QPen(colorMarkers(), width, KstLineStyle[lineStyleMarkers()]));
  }

  updateMarkersFromCurve();
  updateMarkersFromVector();
  KstMarkerList marks;
  if (_xLog) {
    marks = plotMarkers(pow(_xLogBase, x_min), pow(_xLogBase, x_max));
  } else {
    marks = plotMarkers(x_min, x_max);
  }

  // plot each one
  KstMarkerList::iterator marks_iter = marks.begin();
  double mark_px;
  if (_xLog) {
    double new_x;

    while (marks_iter != marks.end()) {
      new_x = logXLo((*marks_iter).value);
      if (new_x <= x_max && new_x >= x_min) {
        mark_px = m_X * new_x + b_X;
        p.drawLine(d2i(mark_px),
                   d2i(ytop_bdr_px),
                   d2i(mark_px),
                   d2i(y_px - ybot_bdr_px));
      }
      ++marks_iter;
    }
  } else {
    while (marks_iter != marks.end()) {
      mark_px = m_X * (*marks_iter).value + b_X;
      p.drawLine(d2i(mark_px),
                 d2i(ytop_bdr_px),
                 d2i(mark_px),
                 d2i(y_px - ybot_bdr_px));
      ++marks_iter;
    }
  }
}


void Kst2DPlot::plotAxes(QPainter& p, QRect& plotRegion,
                         TickParameters tpx,
                         double xleft_bdr_px, double xright_bdr_px,
                         double x_orig_px, double xtick_px,
                         double xtick_len_px, int x_px,
                         TickParameters tpy,
                         double ytop_bdr_px, double ybot_bdr_px,
                         double y_orig_px, double ytick_px,
                         double ytick_len_px, int y_px,
                         bool offsetY) {
  QString TmpStr, TmpStrOld;
  double X1, Y1;
  double X2, Y2;
  int i, j;

  // draw axis
  p.drawRect(plotRegion);

  // tick length and position depends on tick display settings
  int xMajorTopTickTop = _xTicksOutPlot ? d2i(ytop_bdr_px - 2.0 * xtick_len_px) : d2i(ytop_bdr_px);
  int xMajorTopTickBottom = _xTicksInPlot ? d2i(ytop_bdr_px + 2.0 * xtick_len_px) : d2i(ytop_bdr_px);
  int xMajorBottomTickTop = _xTicksInPlot ? d2i(y_px - ybot_bdr_px - 2.0 * xtick_len_px) : d2i(y_px - ybot_bdr_px);
  int xMajorBottomTickBottom = _xTicksOutPlot ? d2i(y_px - ybot_bdr_px + 2.0 * xtick_len_px) : d2i(y_px - ybot_bdr_px);
  int xMinorTopTickTop = _xTicksOutPlot ? d2i(ytop_bdr_px - xtick_len_px) : d2i(ytop_bdr_px);
  int xMinorTopTickBottom = _xTicksInPlot ? d2i(ytop_bdr_px + xtick_len_px) : d2i(ytop_bdr_px);
  int xMinorBottomTickTop = _xTicksInPlot ? d2i(y_px - ybot_bdr_px - xtick_len_px) : d2i(y_px - ybot_bdr_px);
  int xMinorBottomTickBottom = _xTicksOutPlot ? d2i(y_px - ybot_bdr_px + xtick_len_px) : d2i(y_px - ybot_bdr_px);

  int yMajorLeftTickLeft = _yTicksOutPlot ? d2i(xleft_bdr_px - 2.0 * ytick_len_px - 1.0) : d2i(xleft_bdr_px);
  int yMajorLeftTickRight = _yTicksInPlot ? d2i(xleft_bdr_px + 2.0 * ytick_len_px - 1.0) : d2i(xleft_bdr_px);
  int yMajorRightTickLeft = _yTicksInPlot ? d2i(x_px - xright_bdr_px - 2.0 * ytick_len_px - 1.0) : d2i(x_px - xright_bdr_px);
  int yMajorRightTickRight = _yTicksOutPlot ? d2i(x_px - xright_bdr_px + 2.0 * ytick_len_px - 1.0) : d2i(x_px - xright_bdr_px);
  int yMinorLeftTickLeft = _yTicksOutPlot ? d2i(xleft_bdr_px - ytick_len_px) :d2i(xleft_bdr_px);
  int yMinorLeftTickRight = _yTicksInPlot ? d2i(xleft_bdr_px + ytick_len_px) :d2i(xleft_bdr_px);
  int yMinorRightTickLeft = _yTicksInPlot ? d2i(x_px - xright_bdr_px - ytick_len_px) : d2i(x_px - xright_bdr_px);
  int yMinorRightTickRight = _yTicksOutPlot ? d2i(x_px - xright_bdr_px + ytick_len_px) : d2i(x_px - xright_bdr_px);


  // draw x-ticks
  if (_xLog) {
    double XPos;

    i = (int)floor((double)_xMajorTicks*(xleft_bdr_px - 1.0 - x_orig_px)/xtick_px);
    for (;xtick_px * i + x_orig_px < x_px - xright_bdr_px + 1; i++) {
      // draw major ticks
      X1 = (x_orig_px + double(i) * xtick_px);
      if (_xReversed) {
        XPos = x_px - xright_bdr_px - (X1 - xleft_bdr_px);
      } else {
        XPos = X1;
      }
      if (XPos > xleft_bdr_px && XPos < x_px - xright_bdr_px) {
        p.drawLine(d2i(XPos),
                   xMajorTopTickTop,
                   d2i(XPos),
                   xMajorTopTickBottom);

        p.drawLine(d2i(XPos),
                   xMajorBottomTickBottom,
                   d2i(XPos),
                   xMajorBottomTickTop);
      }

      // draw minor ticks
      for (j = 1; j < _xMinorTicks; j++) {
        X2 = log10((double)j/((double)_xMinorTicks)*(pow(_xLogBase,tpx.tick)-1.0)+1.0)/log10(_xLogBase)/
             tpx.tick * (double)xtick_px + X1;
        if (_xReversed) {
          X2 = x_px - xright_bdr_px - (X2 - xleft_bdr_px);
        }
        if (X2 > xleft_bdr_px && X2 < x_px - xright_bdr_px) {
          p.drawLine(d2i(X2),
                     xMinorTopTickTop,
                     d2i(X2),
                     xMinorTopTickBottom);
          p.drawLine(d2i(X2),
                     xMinorBottomTickBottom,
                     d2i(X2),
                     xMinorBottomTickTop);
        }
      }
    }
  } else {
    i = (int)ceil( (double)_xMinorTicks * ( xleft_bdr_px - 1.0 - x_orig_px ) / xtick_px );
    for (; xtick_px * i / _xMinorTicks + x_orig_px < x_px - xright_bdr_px ; i++) {
      X1 = x_orig_px + (double)i * xtick_px / (double)_xMinorTicks;
      if (_xReversed) {
        X1 = x_px - xright_bdr_px - (X1 - xleft_bdr_px);
      }
      if (i % _xMinorTicks == 0) {
        p.drawLine(d2i(X1),
                   xMajorTopTickTop,
                   d2i(X1),
                   xMajorTopTickBottom);
        p.drawLine(d2i(X1),
                   xMajorBottomTickBottom,
                   d2i(X1),
                   xMajorBottomTickTop);
      } else {
        p.drawLine(d2i(X1),
                   xMinorTopTickTop,
                   d2i(X1),
                   xMinorTopTickBottom);
        p.drawLine(d2i(X1),
                   xMinorBottomTickBottom,
                   d2i(X1),
                   xMinorBottomTickTop);
      }
    }
  }

  // draw y ticks
  if (_yLog) {
    double YPos;

    i = (int)floor( (double)_yMajorTicks * ( ytop_bdr_px - 1.0 - y_orig_px ) / ytick_px );
    for (; ytick_px * i + y_orig_px < y_px - ybot_bdr_px + 1; i++) {
      // draw major ticks
      Y1 = y_orig_px + (double)i * ytick_px;
      if (_yReversed) {
        YPos = y_px - ybot_bdr_px - (Y1 - ytop_bdr_px);
      } else {
        YPos = Y1;
      }
      if (YPos > ytop_bdr_px && YPos < y_px - ybot_bdr_px) {
        p.drawLine(yMajorLeftTickLeft,
                   d2i(YPos),
                   yMajorLeftTickRight,
                   d2i(YPos));
        p.drawLine(yMajorRightTickRight,
                   d2i(YPos),
                   yMajorRightTickLeft,
                   d2i(YPos));
      }

      // draw minor ticks
      for (j = 1; j < _yMinorTicks; j++) {
        Y2 = (1.0 - log10((double)j/((double)_yMinorTicks)*(pow(_yLogBase,tpy.tick)-1.0)+1.0)/log10(_yLogBase)/tpy.tick) * (double)ytick_px + Y1;
        if (_yReversed) {
          Y2 = y_px - ybot_bdr_px - (Y2 - ytop_bdr_px);
        }
        if (Y2 > ytop_bdr_px && Y2 < y_px - ybot_bdr_px) {
          p.drawLine(yMinorLeftTickLeft,
                    d2i(Y2),
                    yMinorLeftTickRight,
                    d2i(Y2));
          p.drawLine(yMinorRightTickRight,
                    d2i(Y2),
                    yMinorRightTickLeft,
                    d2i(Y2));
        }
      }
    }
  } else {
    i = (int)ceil( (double)_yMinorTicks * ( ytop_bdr_px - 1.0 - y_orig_px ) / ytick_px );
    for (; ytick_px * i / _yMinorTicks + y_orig_px < y_px - ybot_bdr_px + 1; i++) {
      Y1 = y_orig_px + (double)i * ytick_px / (double)_yMinorTicks;
      if (_yReversed) {
        Y1 = y_px - ybot_bdr_px - (Y1 - ytop_bdr_px);
      }
      if (i % _yMinorTicks == 0) {
        p.drawLine(yMajorLeftTickLeft, d2i(Y1), yMajorLeftTickRight, d2i(Y1));
        p.drawLine(yMajorRightTickRight, d2i(Y1), yMajorRightTickLeft, d2i(Y1));
      } else {
        p.drawLine(yMinorLeftTickLeft, d2i(Y1), yMinorLeftTickRight, d2i(Y1));
        p.drawLine(yMinorRightTickRight, d2i(Y1), yMinorRightTickLeft, d2i(Y1));
      }
    }
  }

  // x axis numbers
  if (!_suppressBottom) {
    int yTickPos = d2i(y_px - ybot_bdr_px + _xTickLabel->lineSpacing()*0.15);
    if (xTicksOutPlot()) {
      yTickPos += d2i(2.0 * xtick_len_px);
    }

    if (tpx.delta && !tpx.labels.isEmpty()) {
      _fullTickLabel->setRotation(0);
      _fullTickLabel->setText(tpx.labels[0]);
      p.save();
      p.translate(d2i(xleft_bdr_px), yTickPos+_xTickLabel->size().height());
      _fullTickLabel->paint(p);
      p.restore();
      tpx.labels.pop_front();
    }

    // FIXME: inefficient.  We keep reparsing and re-rendering tpx.labels[.]
    for (i = tpx.iLo; i < tpx.iHi; i++) {
      double xTickPos = x_orig_px + (double)i * xtick_px;
      if (_xReversed) {
        xTickPos = x_px - xright_bdr_px - (xTickPos - xleft_bdr_px);
      }

      p.save();
      _xTickLabel->setText(tpx.labels[i - tpx.iLo]);
      if (_xTickLabel->rotation() == 0) {
        if (!((_suppressLeft  && d2i(xTickPos) - _xTickLabel->size().width() / 2 < xleft_bdr_px ) ||
              (_suppressRight && d2i(xTickPos) - _xTickLabel->size().width() / 2 > x_px - xright_bdr_px ) ) ) {
          p.translate(d2i(xTickPos) - _xTickLabel->size().width() / 2, yTickPos);
          _xTickLabel->paint(p);
        }
      } else if (_xTickLabel->rotation() > 0) {
        p.translate(xTickPos-0.5*_xTickLabel->lineSpacing()*sin(_xTickLabel->rotation()*M_PI/180.0), yTickPos);
        _xTickLabel->paint(p);
      } else if (_xTickLabel->rotation() < 0) {
        p.translate(xTickPos - _xTickLabel->size().width()-0.5*_xTickLabel->lineSpacing()*sin(_xTickLabel->rotation()*M_PI/180.0), yTickPos);
        _xTickLabel->paint(p);
      }
      p.restore();
    }
  }

  // if top axis is transformed, plot top axis numbers as well
  // FIXME: inefficient.  We keep reparsing and re-rendering tpx.oppLabels[.]
  if (_xTransformed && !_suppressTop) {
    int yTopTickPos = d2i(ytop_bdr_px);
    if (xTicksOutPlot()) {
      yTopTickPos -= d2i(2.0 * xtick_len_px);
    }
    for (i = tpx.iLo; i < tpx.iHi; i++) {
      double xTickPos = x_orig_px + (double)i * xtick_px;
      if (_xReversed) {
        xTickPos = x_px - xright_bdr_px - (xTickPos - xleft_bdr_px);
      }
      _xTickLabel->setText(tpx.oppLabels[i - tpx.iLo]);
      if (!((_suppressLeft  && d2i(xTickPos) - _xTickLabel->size().width() / 2 < xleft_bdr_px ) ||
            (_suppressRight && d2i(xTickPos) - _xTickLabel->size().width() / 2 > x_px - xright_bdr_px ) ) ) {
        p.save();
        p.translate(d2i(xTickPos) - _xTickLabel->size().width() / 2, yTopTickPos - _xTickLabel->size().height());
        _xTickLabel->paint(p);
        p.restore();
      }
    }
  }

  // y axis numbers
  if (!_suppressLeft) {
    if (tpy.delta && !tpy.labels.isEmpty()) {
      _fullTickLabel->setRotation(270);
      _fullTickLabel->setText(tpy.labels[0]);
      p.save();
      if (offsetY && !_yLabel->text().isEmpty()) {
        p.translate(_yLabel->lineSpacing(), d2i(y_px - ybot_bdr_px) - _fullTickLabel->size().height());
      } else {
        p.translate(0, d2i(y_px-ybot_bdr_px)-_fullTickLabel->size().height());
      }
      _fullTickLabel->paint(p);
      p.restore();
      tpy.labels.pop_front();
    }

    int xTickPos = d2i(xleft_bdr_px - _yTickLabel->lineSpacing() / 4);
    if (yTicksOutPlot()) {
      xTickPos -= d2i(2.0 * ytick_len_px);
    }
    // FIXME: inefficient.  We keep reparsing and re-rendering tpy.labels[.]
    for (i = tpy.iLo; i < tpy.iHi; i++) {
      double yTickPos = y_orig_px - (double)i * ytick_px;
      if (_yReversed) {
        yTickPos = y_px - ybot_bdr_px - (yTickPos - ytop_bdr_px);
      }
      _yTickLabel->setText(tpy.labels[i - tpy.iLo]);

      p.save();
      if (_yTickLabel->rotation() == 0 || _yTickLabel->rotation() <-89 || _yTickLabel->rotation() > 89) {
        if (!((_suppressBottom && d2i(yTickPos) + _yTickLabel->size().height() / 2 > y_px - ybot_bdr_px ) ||
              (_suppressTop    && d2i(yTickPos) - _yTickLabel->size().height() / 2 < ytop_bdr_px ) ) ) {
          p.translate(d2i(xTickPos) - _yTickLabel->size().width(), d2i(yTickPos) - _yTickLabel->size().height() / 2);
          _yTickLabel->paint(p);
        }
      } else if (_yTickLabel->rotation() < 0) {
        p.translate(d2i(xTickPos) - _yTickLabel->size().width(), d2i(yTickPos) - _yTickLabel->lineSpacing()/2);
        _yTickLabel->paint(p);
      } else if (_yTickLabel->rotation() > 0) {
        p.translate(d2i(xTickPos) - _yTickLabel->size().width(), d2i(yTickPos) - _yTickLabel->size().height() + _yTickLabel->lineSpacing()/2);
        _yTickLabel->paint(p);
      }
      p.restore();
    }
  }

  // if right axis is transformed, plot right axis numbers as well
  if (_yTransformed && !_suppressRight) {
    int xTopTickPos = d2i(x_px - xright_bdr_px + _yTickLabel->lineSpacing()*0.15);
    if (yTicksOutPlot()) {
      xTopTickPos += d2i(2.0 * ytick_len_px);
    }
    // FIXME: inefficient.  We keep reparsing and re-rendering tpy.oppLabels[.]
    for (i = tpy.iLo; i < tpy.iHi; i++) {
      double yTickPos = y_orig_px - (double)i * ytick_px;
      if (_yReversed) {
        yTickPos = y_px - ybot_bdr_px - (yTickPos - ytop_bdr_px);
      }
      _yTickLabel->setText(tpy.oppLabels[i - tpy.iLo]);
      if (!((_suppressBottom && d2i(yTickPos) + _yTickLabel->size().height() / 2 > y_px - ybot_bdr_px ) ||
            (_suppressTop    && d2i(yTickPos) - _yTickLabel->size().height() / 2 < ytop_bdr_px ) ) ) {
        p.save();
        p.translate(xTopTickPos, d2i(yTickPos) - _yTickLabel->size().height() / 2);
        _yTickLabel->paint(p);
        p.restore();
      }
    }
  }
}


void Kst2DPlot::plotLabels(QPainter &p, int x_px, int y_px, double xleft_bdr_px, double xright_bdr_px,  double ytop_bdr_px) {
  if (!_suppressBottom) {
    p.save();
    p.translate((x_px - _xLabel->size().width() + xleft_bdr_px) / 2, y_px - _xLabel->size().height());
    _xLabel->paint(p);
    p.restore();
  }

  if (!_suppressLeft) {
    p.save();
    p.translate((_yLabel->lineSpacing() - _yLabel->ascent())/2, (y_px - _yLabel->size().height()) / 2 - ytop_bdr_px/2);
    _yLabel->paint(p);
    p.restore();
  }

  if (!_suppressTop) {
    int xpos;

    p.save();
    switch (KST_JUSTIFY_H(_topLabel->justification())) {
      case KST_JUSTIFY_H_LEFT:
        xpos = d2i(xleft_bdr_px);
        break;
      case KST_JUSTIFY_H_RIGHT:
        xpos = d2i(x_px - xright_bdr_px) - _topLabel->size().width();
        break;
      case KST_JUSTIFY_H_CENTER:
        xpos = d2i( ( x_px + xleft_bdr_px - xright_bdr_px ) / 2.0 ) - ( _topLabel->size().width() / 2 );
        break;
      default:
        xpos = d2i(xleft_bdr_px);
        break;
    }
    p.translate(xpos, d2i(0.08 * _topLabel->size().height()));
    _topLabel->paint(p);
    p.restore();
  }
}


void Kst2DPlot::setGridLinesColor(const QColor& majorColor, const QColor& minorColor,
                    bool majorGridColorDefault, bool minorGridColorDefault) {
  _majorGridColor = majorColor;
  _minorGridColor = minorColor;
  _majorGridColorDefault = majorGridColorDefault;
  _minorGridColorDefault = minorGridColorDefault;
}


void Kst2DPlot::setXGridLines(bool xMajor, bool xMinor) {
  _xMajorGrid = xMajor;
  _xMinorGrid = xMinor;
}


void Kst2DPlot::setYGridLines(bool yMajor, bool yMinor) {
  _yMajorGrid = yMajor;
  _yMinorGrid = yMinor;
}


void Kst2DPlot::plotGridLines(KstPainter& p,
    double XTick, double xleft_bdr_px, double xright_bdr_px,
    double x_orig_px, double xtick_px, double xtick_len_px, int x_px,
    double YTick, double ytop_bdr_px, double ybot_bdr_px,
    double y_orig_px, double ytick_px, double ytick_len_px, int y_px) {
  QColor defaultGridColor( (_foregroundColor.red() + _backgroundColor.red()) / 2,
      (_foregroundColor.green() + _backgroundColor.green()) / 2,
      (_foregroundColor.blue() + _backgroundColor.blue()) / 2 );
  double X1, Y1;
  double X2, Y2;
  int i, j;

  int minor_width = p.lineWidthAdjustmentFactor() * minorPenWidth();
  int major_width = p.lineWidthAdjustmentFactor() * majorPenWidth();

  // draw X grid lines
  if (_xLog) {
    i = (int)floor( (double)_xMinorTicks * ( xleft_bdr_px - 1.0 - x_orig_px ) / xtick_px );
    for (;xtick_px * i + x_orig_px < x_px - xright_bdr_px + 1; i++) {

      // draw major lines
      X1 = (x_orig_px + (double)i * xtick_px);
      if (_xReversed) {
        X1 = x_px - xright_bdr_px - (X1 - xleft_bdr_px);
      }
      if (_xMajorGrid && X1 > xleft_bdr_px && X1 < x_px - xright_bdr_px) {
        if (_majorGridColorDefault) {
          p.setPen(QPen(defaultGridColor, major_width, Qt::DotLine));
        } else {
          p.setPen(QPen(_majorGridColor, major_width, Qt::DotLine));
        }
        p.drawLine(d2i(X1),
                  d2i(ytop_bdr_px + 2.0 * xtick_len_px),
                  d2i(X1),
                  d2i(y_px - ybot_bdr_px - 2.0 * xtick_len_px));
      }

      // draw minor lines
      for (j = 1; j < _xMinorTicks; j++) {
        X2 = log10((double)j/((double)_xMinorTicks)*(pow(_xLogBase,XTick)-1.0)+1.0)/log10(_xLogBase)/XTick * (double)xtick_px + X1;
        if (_xReversed) {
          X2 = x_px - xright_bdr_px - (X2 - xleft_bdr_px);
        }
        if (_xMinorGrid && X2 > xleft_bdr_px && X2 < x_px - xright_bdr_px) {
          if (_minorGridColorDefault) {
            p.setPen(QPen(defaultGridColor, minor_width, Qt::DotLine));
          } else {
            p.setPen(QPen(_minorGridColor, minor_width, Qt::DotLine));
          }
          p.drawLine(d2i(X2),
                    d2i(ytop_bdr_px + xtick_len_px),
                    d2i(X2),
                    d2i(y_px - ybot_bdr_px - xtick_len_px));
        }
      }
    }
  } else {
    i = (int)ceil( (double)_xMinorTicks * ( xleft_bdr_px - 1.0 - x_orig_px ) / xtick_px );
    for (; xtick_px * i / _xMinorTicks + x_orig_px < x_px - xright_bdr_px ; i++) {
      X1 = x_orig_px + (double)i * xtick_px / (double)_xMinorTicks;
      if (_xReversed) {
        X1 = x_px - xright_bdr_px - (X1 - xleft_bdr_px);
      }
      if (_xMajorGrid && i % _xMinorTicks == 0) {
        if (_majorGridColorDefault) {
          p.setPen(QPen(defaultGridColor, major_width, Qt::DotLine));
        } else {
          p.setPen(QPen(_majorGridColor, major_width, Qt::DotLine));
        }
        p.drawLine(d2i(X1),
                  d2i(ytop_bdr_px + 2.0 * xtick_len_px),
                  d2i(X1),
                  d2i(y_px - ybot_bdr_px - 2 * xtick_len_px));
      } else if (_xMinorGrid && i % _xMinorTicks != 0) {
        if (_minorGridColorDefault) {
          p.setPen(QPen(defaultGridColor, minor_width, Qt::DotLine));
        } else {
          p.setPen(QPen(_minorGridColor, minor_width, Qt::DotLine));
        }
        p.drawLine(d2i(X1),
                  d2i(ytop_bdr_px + xtick_len_px),
                  d2i(X1),
                  d2i(y_px - ybot_bdr_px - xtick_len_px));
      }
    }
  }

  // draw Y grid lines
  if (_yLog) {
    i = (int)floor( (double)_yMinorTicks * ( ytop_bdr_px - 1.0 - y_orig_px ) / ytick_px );
    for (; ytick_px * i + y_orig_px < y_px - ybot_bdr_px + 1; i++) {

      // draw major lines
      Y1 = y_orig_px + (double)i * ytick_px;
      if (_yReversed) {
        Y1 = y_px - ybot_bdr_px - (Y1 - ytop_bdr_px);
      }
      if (_yMajorGrid && Y1 > ytop_bdr_px) {
        if (_majorGridColorDefault) {
          p.setPen(QPen(defaultGridColor, major_width, Qt::DotLine));
        } else {
          p.setPen(QPen(_majorGridColor, major_width, Qt::DotLine));
        }
        p.drawLine(d2i(xleft_bdr_px + 2.0 * ytick_len_px - 1.0),
                  d2i(Y1),
                  d2i(x_px - xright_bdr_px - 2.0 * ytick_len_px - 1.0),
                  d2i(Y1));
      }

      // draw minor lines
      for (j = 1; j < _yMinorTicks; j++) {
        Y2 = (1.0 - (log10((double)j/((double)_yMinorTicks)*(pow(_yLogBase,YTick)-1.0)+1.0)/log10(_yLogBase)/YTick)) * (double)ytick_px + Y1;
        if (_yReversed) {
          Y2 = y_px - ybot_bdr_px - (Y2 - ytop_bdr_px);
        }
        if (_yMinorGrid && Y2 > ytop_bdr_px && Y2 < y_px - ybot_bdr_px) {
          if (_minorGridColorDefault) {
            p.setPen(QPen(defaultGridColor, minor_width, Qt::DotLine));
          } else {
            p.setPen(QPen(_minorGridColor, minor_width, Qt::DotLine));
          }
          p.drawLine(d2i(xleft_bdr_px + ytick_len_px),
                    d2i(Y2),
                    d2i(x_px - xright_bdr_px - ytick_len_px),
                    d2i(Y2));
        }
      }
    }
  } else {
    i = (int)ceil( (double)_yMinorTicks * ( ytop_bdr_px - 1.0 - y_orig_px ) / ytick_px );
    for (; ytick_px * i / _yMinorTicks + y_orig_px < y_px - ybot_bdr_px + 1; i++) {
      Y1 = y_orig_px + (double)i * ytick_px / (double)_yMinorTicks;
      if (_yReversed) {
        Y1 = y_px - ybot_bdr_px - (Y1 - ytop_bdr_px);
      }
      if (_yMajorGrid && i % _yMinorTicks == 0) {
        if (_majorGridColorDefault) {
          p.setPen(QPen(defaultGridColor, major_width, Qt::DotLine));
        } else {
          p.setPen(QPen(_majorGridColor, major_width, Qt::DotLine));
        }
        p.drawLine(d2i(xleft_bdr_px + 2.0 * ytick_len_px),
                  d2i(Y1),
                  d2i(x_px - xright_bdr_px - 2.0 * ytick_len_px),
                  d2i(Y1));
      } else if (_yMinorGrid && i % _yMinorTicks != 0) {
        if (_minorGridColorDefault) {
          p.setPen(QPen(defaultGridColor, minor_width, Qt::DotLine));
        } else {
          p.setPen(QPen(_minorGridColor, minor_width, Qt::DotLine));
        }
        p.drawLine(d2i(xleft_bdr_px + ytick_len_px),
                  d2i(Y1),
                  d2i(x_px - xright_bdr_px - ytick_len_px),
                  d2i(Y1));
      }
    }
  }
}


void Kst2DPlot::setXMinorTicks(int minorTicks) {
  if (minorTicks >= 0) {
    _reqXMinorTicks = minorTicks + 1;
  } else {
    _reqXMinorTicks = -1;
  }
}


void Kst2DPlot::setYMinorTicks(int minorTicks) {
  if (minorTicks >= 0) {
    _reqYMinorTicks = minorTicks + 1;
  } else {
    _reqYMinorTicks = -1;
  }
}


void Kst2DPlot::setXMajorTicks(int majorTicks) {
  if (majorTicks > 0) {
    _xMajorTicks = majorTicks;
  } else {
    _xMajorTicks = 1;
  }
}


void Kst2DPlot::setYMajorTicks(int majorTicks) {
  if (majorTicks > 0) {
    _yMajorTicks = majorTicks;
  } else {
    _yMajorTicks = 1;
  }
}


void Kst2DPlot::pushAdjustLineWidth(int adjustment) {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->pushLineWidth((*i)->lineWidth() + adjustment);
    (*i)->unlock();
  }
}


void Kst2DPlot::popLineWidth() {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->popLineWidth();
    (*i)->unlock();
  }
}


void Kst2DPlot::pushCurveColor(const QColor& c) {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->pushColor(c);
    (*i)->unlock();
  }
}


void Kst2DPlot::popCurveColor() {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->popColor();
    (*i)->unlock();
  }
}


void Kst2DPlot::pushPlotColors() {
  _colorStack.push(_backgroundColor);
  _colorStack.push(_foregroundColor);
  _colorStack.push(_majorGridColor);
  _colorStack.push(_minorGridColor);
  _backgroundColor = Qt::white;
  _foregroundColor = Qt::black;
  _majorGridColor = Qt::black;
  _minorGridColor = Qt::black;
}


void Kst2DPlot::popPlotColors() {
  _minorGridColor = _colorStack.pop();
  _majorGridColor = _colorStack.pop();
  _foregroundColor = _colorStack.pop();
  _backgroundColor = _colorStack.pop();
}


QString Kst2DPlot::menuTitle() const {
  return i18n("Plot: %1").arg(tagName());
}


void Kst2DPlot::mouseDoubleClickEvent(QWidget *view, QMouseEvent *e) {
  // allow user to edit a curve if click was close enough.
  Q_UNUSED(view)
  KstBaseCurvePtr curve;
  QRect pr = GetPlotRegion();
  QPoint pos = e->pos();
  double best_distance = 1.0E300;
  double xmin, ymin, xmax, ymax;
  double xpos, ypos;

  getCursorPos(pos, xpos, ypos, xmin, xmax, ymin, ymax);

  // calculate max x distance.
  double dx_per_pix;
  if (!isXLog()) {
    dx_per_pix = (xmax - xmin)/pr.width();
  } else {
    dx_per_pix = xpos*log(_xLogBase)*(xmax - xmin)/pr.width();
  }
  double dx = fabs(5.0*dx_per_pix); //~5 pixels.

  // calculate max y distance.
  double dy_per_pix;
  if (!isYLog()) {
    dy_per_pix = (ymin - ymax)/pr.height();
  } else {
    dy_per_pix = ypos*log(_yLogBase)*(ymin - ymax)/pr.height();
  }
  double dy = fabs(5.0*dy_per_pix); //~5 pixels.

  for (KstBaseCurveList::Iterator i = Curves.begin(); i != Curves.end(); ++i) {
    (*i)->readLock();
    double distance = (*i)->distanceToPoint(xpos, dx, ypos);
    (*i)->unlock();

    if (distance < best_distance || !curve) {
      best_distance = distance;
      curve = *i;
    }
  }

  if (curve && fabs(best_distance) <= dy) {
    curve->readLock();
    curve->showDialog(false);
    curve->unlock();
  }

  e->accept();
}


bool Kst2DPlot::setXExpressions(const QString& minExp, const QString& maxExp) {
  _xMinExp = minExp;
  _xMaxExp = maxExp;
  return (_xMinParsedValid = reparse(_xMinExp, &_xMinParsed)) &&
         (_xMaxParsedValid = reparse(_xMaxExp, &_xMaxParsed));
}


bool Kst2DPlot::setYExpressions(const QString& minExp, const QString& maxExp) {
  _yMinExp = minExp;
  _yMaxExp = maxExp;
  return (_yMinParsedValid = reparse(_yMinExp, &_yMinParsed)) &&
         (_yMaxParsedValid = reparse(_yMaxExp, &_yMaxParsed));
}


void Kst2DPlot::getXScaleExps(QString& minExp, QString& maxExp) const {
  minExp = _xMinExp;
  maxExp = _xMaxExp;
}


void Kst2DPlot::getYScaleExps(QString& minExp, QString& maxExp) const {
  minExp = _yMinExp;
  maxExp = _yMaxExp;
}


bool Kst2DPlot::reparse(const QString& stringExp, Equation::Node** eqNode) {
  bool eqValid = false;
  if (!stringExp.isEmpty()) {
    QMutexLocker ml(&Equation::mutex());
    yy_scan_string(stringExp.toLatin1());
    int rc = yyparse();
    if (rc == 0) {
      *eqNode = static_cast<Equation::Node*>(ParsedEquation);
      Equation::Context ctx;
      Equation::FoldVisitor vis(&ctx, eqNode);
      eqValid = true;
    } else {
      delete (Equation::Node*)ParsedEquation;
    }
    ParsedEquation = 0L;
  }
  return eqValid;
}


bool Kst2DPlot::reparseToText(QString& stringExp) {
  Equation::Node *en = 0L;
  bool toTextOK = reparse(stringExp, &en);
  if (toTextOK) {
    stringExp = en->text();
  }
  delete en;
  return toTextOK;
}


void Kst2DPlot::optimizeXExps() {
  if (_xMinParsedValid && _xMaxParsedValid && _xMinParsed->isConst() && _xMaxParsed->isConst()) {
    Equation::Context ctx;
    double min = _xMinParsed->value(&ctx);
    double max = _xMaxParsed->value(&ctx);
    if (min > max) {
      double tmp = max;
      max = min;
      min = tmp;
    } else if (XMin == XMax) {
      if (min == 0.0) {
        min = -0.5;
        max =  0.5;
      } else {
        max += fabs(max) * 0.01;
        min -= fabs(min) * 0.01;
      }
    }
    setXScale(min, max);
  }
}


void Kst2DPlot::optimizeYExps() {
  if (_yMinParsedValid && _yMaxParsedValid && _yMinParsed->isConst() && _yMaxParsed->isConst()) {
    Equation::Context ctx;
    double min = _yMinParsed->value(&ctx);
    double max = _yMaxParsed->value(&ctx);
    if (min > max) {
      double tmp = max;
      max = min;
      min = tmp;
    } else if (XMin == XMax) {
      if (min == 0.0) {
        min = -0.5;
        max =  0.5;
      } else {
        max += fabs(max) * 0.01;
        min -= fabs(min) * 0.01;
      }
    }
    setYScale(min, max);
  }
}


void Kst2DPlot::setXTicksInPlot(bool yes) {
  _xTicksInPlot = yes;
}


void Kst2DPlot::setXTicksOutPlot(bool yes) {
  _xTicksOutPlot = yes;
}


void Kst2DPlot::setYTicksInPlot(bool yes) {
  _yTicksInPlot = yes;
}


void Kst2DPlot::setYTicksOutPlot(bool yes) {
  _yTicksOutPlot = yes;
}


bool Kst2DPlot::xTicksInPlot() const {
  return _xTicksInPlot;
}


bool Kst2DPlot::xTicksOutPlot() const {
  return _xTicksOutPlot;
}


bool Kst2DPlot::yTicksInPlot() const {
  return _yTicksInPlot;
}


bool Kst2DPlot::yTicksOutPlot() const {
  return _yTicksOutPlot;
}


bool Kst2DPlot::suppressTop() const {
  return _suppressTop;
}


bool Kst2DPlot::suppressBottom() const {
  return _suppressBottom;
}


bool Kst2DPlot::suppressLeft() const {
  return _suppressLeft;
}


bool Kst2DPlot::suppressRight() const {
  return _suppressRight;
}


void Kst2DPlot::setSuppressTop(bool yes) {
  _suppressTop = yes;
}


void Kst2DPlot::setSuppressBottom(bool yes) {
  _suppressBottom = yes;
}


void Kst2DPlot::setSuppressLeft(bool yes) {
  _suppressLeft = yes;
}


void Kst2DPlot::setSuppressRight(bool yes) {
  _suppressRight = yes;
}


void Kst2DPlot::setXTransformedExp(const QString& exp) {
  _xTransformedExp = exp;
  _xTransformed = !exp.trimmed().isEmpty();
}


void Kst2DPlot::setYTransformedExp(const QString& exp) {
  _yTransformedExp = exp;
  _yTransformed = !exp.trimmed().isEmpty();
}


const QString& Kst2DPlot::xTransformedExp() const {
  return _xTransformedExp;
}


const QString& Kst2DPlot::yTransformedExp() const {
  return _yTransformedExp;
}


void Kst2DPlot::pushCurveHasPoints(bool yes) {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->pushHasPoints(yes);
    (*i)->unlock();
  }
}


void Kst2DPlot::popCurveHasPoints() {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->popHasPoints();
    (*i)->unlock();
  }
}


void Kst2DPlot::pushCurveHasLines(bool yes) {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->pushHasLines(yes);
    (*i)->unlock();
  }
}


void Kst2DPlot::popCurveHasLines() {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->popHasLines();
    (*i)->unlock();
  }
}


void Kst2DPlot::pushCurvePointDensity(int pointDensity) {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->pushPointDensity(pointDensity);
    (*i)->unlock();
  }
}


void Kst2DPlot::popCurvePointDensity() {
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    (*i)->popPointDensity();
    (*i)->unlock();
  }
}


void Kst2DPlot::changeToMonochrome(int pointStylePriority, int lineStylePriority, int lineWidthPriority,
                                   int maxLineWidth, int pointDensity) {
  // change plot background to white, foreground to black,
  // and set all curves to black, and cycle line styles and point styles
  pushPlotColors();
  pushCurveColor(Qt::black);

  if (pointStylePriority > -1) {
    pushCurvePointDensity(pointDensity);
    pushCurveHasPoints(true);
  }
  if (lineStylePriority > -1 || lineWidthPriority > -1) {
    pushCurveHasLines(true);
  }

  KstNumberSequence lineStyleSeq(0, KSTLINESTYLE_MAXTYPE - 1);
  KstNumberSequence pointStyleSeq(0, KSTPOINT_MAXTYPE - 1);
  KstNumberSequence lineWidthSeq(1, maxLineWidth);

  int maxSequences = -1;
  Q3PtrVector<KstNumberSequence> seqVect(3);
  if (pointStylePriority > -1) {
    seqVect.insert(pointStylePriority, &pointStyleSeq);
    ++maxSequences;
  }
  if (lineStylePriority > -1) {
    seqVect.insert(lineStylePriority, &lineStyleSeq);
    ++maxSequences;
  }
  if (lineWidthPriority > -1) {
    seqVect.insert(lineWidthPriority, &lineWidthSeq);
    ++maxSequences;
  }
  if (maxSequences < 0) {
    return;
  }
  seqVect.resize(maxSequences + 1);

  for (int i = 0; i < maxSequences; i++) {
    seqVect[i]->hookToNextSequence(seqVect[i+1]);
  }
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    if (pointStylePriority > -1) {
      (*i)->pushPointStyle(pointStyleSeq.current());
    }
    if (lineStylePriority > -1) {
      (*i)->pushLineStyle(lineStyleSeq.current());
    }
    if (lineWidthPriority > -1) {
      (*i)->pushLineWidth(5*lineWidthSeq.current());
    }
    (*i)->unlock();
    seqVect[0]->next();
  }
}



bool Kst2DPlot::undoChangeToMonochrome(int pointStylePriority, int lineStylePriority, int lineWidthPriority) {
  // pop everything back
  KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>(Curves);
  for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
    (*i)->writeLock();
    if (lineStylePriority > -1) {
      (*i)->popLineStyle();
    }
    if (pointStylePriority > -1) {
      (*i)->popPointStyle();
    }
    if (lineWidthPriority > -1) {
      (*i)->popLineWidth();
    }
    (*i)->unlock();
  }
  popPlotColors();
  popCurveColor();
  if (pointStylePriority > -1) {
    popCurvePointDensity();
    popCurveHasPoints();
  }
  if (lineStylePriority > -1 || lineWidthPriority > -1) {
    popCurveHasLines();
  }

  return true;
}


void Kst2DPlot::setXReversed(bool yes) {
  _xReversed = yes;
}


void Kst2DPlot::setYReversed(bool yes) {
  _yReversed = yes;
}


bool Kst2DPlot::xReversed() const {
  return _xReversed;
}


bool Kst2DPlot::yReversed() const {
  return _yReversed;
}

KstViewLabelPtr Kst2DPlot::convertLabelToViewLabel(const QDomElement &e) {
  KstViewLabelPtr label = new KstViewLabel("label");
  label->setRotation(0.0);
  label->setJustification(SET_KST_JUSTIFY(KST_JUSTIFY_H_CENTER, KST_JUSTIFY_V_BOTTOM));
  label->setTransparent(true);
  label->resizeFromAspect(0.05, 0.05, 0.05, 0.05);
  label->setDataPrecision(LABEL_PRECISION);

  float xPos = 0.0, yPos = 0.0;

  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if (!e.isNull()) { // the node was really an element.
      if (e.tagName() == "text") {
        label->setText(e.text());
      } else if  (e.tagName() == "justify") {
        label->setJustification((KstLJustifyType) e.text().toInt());
      } else if  (e.tagName() == "rotation") {
        label->setRotation(e.text().toFloat());
      } else if  (e.tagName() == "interpret") {
        label->setInterpreted(true);
      } else if  (e.tagName() == "xpos") {
        xPos = e.text().toFloat();
      } else if (e.tagName() == "ypos") {
        yPos = e.text().toFloat();
      } else if (e.tagName() == "fontfamily") {
        label->setFontName(e.text());
      } else if (e.tagName() == "symbolfontfamily") {
        // unsupported
      } else if (e.tagName() == "fontsize") {
        label->setFontSize(e.text().toInt() - 12);
      } else if (e.tagName() == "size") {
        label->setFontSize(e.text().toInt());
      } else if (e.tagName() == "useusercolor") {
        label->setTransparent(false);
      } else if (e.tagName() == "color") {
        label->setForegroundColor(QColor(e.text()));
      }
    }
    n = n.nextSibling();
  }

  label->resizeFromAspect(xPos, yPos, 0.05, 0.05);

  return label;
}


KstPlotLabel *Kst2DPlot::xLabel() const {
  return _xLabel;
}


KstPlotLabel *Kst2DPlot::yLabel() const {
  return _yLabel;
}


KstPlotLabel *Kst2DPlot::topLabel() const {
  return _topLabel;
}


KstPlotLabel *Kst2DPlot::xTickLabel() const {
  return _xTickLabel;
}


KstPlotLabel *Kst2DPlot::yTickLabel() const {
  return _yTickLabel;
}


KstPlotLabel *Kst2DPlot::fullTickLabel() const {
  return _fullTickLabel;
}


/** find the first legend owned by the plot, or return NULL if there is none */
KstViewLegendPtr Kst2DPlot::legend() const {
  for (KstViewObjectList::ConstIterator i = _children.begin(); i != _children.end(); ++i) {
    KstViewLegendPtr vl = kst_cast<KstViewLegend>(*i);
    if (vl) {
      return vl;
    }
  }
  return 0L;
}


/** Find the first legend owned by a plot, or create and populate one if there is none */
KstViewLegendPtr Kst2DPlot::getOrCreateLegend() {
  KstViewLegendPtr vl = legend();
  if (!vl) {
    vl = new KstViewLegend;
    appendChild(KstViewObjectPtr(vl), true);
    vl->resizeFromAspect(0.1, 0.1, 0.2, 0.1);
    for (KstBaseCurveList::ConstIterator it = Curves.begin(); it != Curves.end(); ++it) {
      vl->addCurve(*it);
    }
  }
  return vl;
}


void Kst2DPlot::timezoneChanged(const QString& tz, int utcOffset) {
  Q_UNUSED(tz)
  Q_UNUSED(utcOffset)
  setDirty();
  KstApp::inst()->activeView()->widget()->paint();
}


QRect Kst2DPlot::contentsRect() const {
  return PlotRegion;
}


double Kst2DPlot::verticalSizeFactor() {
  // roughly, geometry/dataRect.  But use an estimate rather
  // than calculating it, so that we get stable results.
  // this is used by cleanup to do a better job with
  // sizing plots with supressed borders.
  double f = 1.3;
  if (_suppressTop) {
    f -= 0.1;
  }
  if (_suppressBottom) {
    f -= 0.2;
  }

  return f;
}

double Kst2DPlot::horizontalSizeFactor() {
  double f = 1.3;
  if (_suppressLeft) {
    f -= 0.25;
  }
  if (_suppressRight) {
    f -= 0.05;
  }

  return f;
}

/** adjust the font size of all plot labels at once */
void Kst2DPlot::setPlotLabelFontSizes(int size){
  xLabel()->setFontSize(size);
  yLabel()->setFontSize(size);
  topLabel()->setFontSize(size);
  xTickLabel()->setFontSize(size);
  fullTickLabel()->setFontSize(size);
  yTickLabel()->setFontSize(size);
  if (KstViewLegendPtr vl = legend()) {
    vl->setFontSize(size);
  }
}


/****************************
 * 2dplot edit dialog stuff */

/** fill the custom widget with current properties */
/** Unlike most viewObject dialogs, here we let the dialog fill itself,
    rather than having kst2dplot fill it - kst2dplot is already too big! */
bool Kst2DPlot::fillConfigWidget(QWidget *w, bool isNew) const {
  Q_UNUSED(isNew)
  View2DPlotWidget *widget = dynamic_cast<View2DPlotWidget*>(w);
  if (!widget) {
    return false;
  }
  
  widget->fillWidget(this);
  widget->TabWidget->setCurrentIndex(_tabToShow);
  return false;
}

/** apply properties in the custom config widget to this */
/** Unlike most viewObject dialogs, here we let the dialog fill its plot,
    rather than having kst2dplot fill it - kst2dplot is already too big! */
bool Kst2DPlot::readConfigWidget(QWidget *w) {
  View2DPlotWidget *widget = dynamic_cast<View2DPlotWidget*>(w);
  if (!widget) {
    return false;
  }
  // FIXME: apply properties here

  widget->fillPlot(this);
  setDirty();
  return true;
}

void Kst2DPlot::connectConfigWidget(QWidget *parent, QWidget *w) const {
  View2DPlotWidget *widget = dynamic_cast<View2DPlotWidget*>(w);
  if (!widget) {
    return;
  }
  connect(widget, SIGNAL(changed()), parent, SLOT(modified()));
  connect(widget->_title, SIGNAL( textChanged(const QString&) ), parent, SLOT( modified() ) );
  connect(widget->plotColors, SIGNAL( bgChanged(const QColor&) ), parent, SLOT(modified()) );
  connect(widget->plotColors, SIGNAL( fgChanged(const QColor&) ), parent, SLOT( modified() ) );
  connect(widget->_axisPenWidth, SIGNAL( valueChanged(int) ), parent, SLOT( modified() ) );
  connect(widget->_majorPenWidth, SIGNAL( valueChanged(int) ), parent, SLOT( modified() ) );
  connect(widget->_minorPenWidth, SIGNAL( valueChanged(int) ), parent, SLOT( modified() ) );
  connect(widget->_majorGridColor, SIGNAL( changed(const QColor&) ), parent, SLOT( modified() ) );
  connect(widget->_minorGridColor, SIGNAL( changed(const QColor&) ), parent, SLOT( modified() ) );
  connect(widget->_checkBoxDefaultMajorGridColor, SIGNAL( stateChanged(int) ), parent, SLOT( modified() ) );

  connect( widget->ScalarList, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->TopLabelFontSize, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->TopLabelText, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->_comboBoxTopLabelJustify, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->YLabelFontSize, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->YAxisText, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->XLabelFontSize, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->XAxisText, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->NumberFontSize, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->_spinBoxXAngle, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->_spinBoxYAngle, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->AutoLabel, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->FontComboBox, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->ShowLegend, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->TrackContents, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_pushButtonEditLegend, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->appearanceThisPlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->appearanceThisWindow, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));

  connect( widget->_suppressTop, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_suppressBottom, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XIsLog, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_checkBoxXOffsetMode, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xReversed, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_checkBoxXInterpret, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_comboBoxXInterpret, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->_comboBoxXDisplay, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->_xTransformTop, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xTransformTopExp, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->_xMajorTickSpacing, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->_xMinorTicks, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xMinorTicksAuto, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xMarksInsidePlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xMarksOutsidePlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xMarksInsideAndOutsidePlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xMajorGrid, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_xMinorGrid, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XAxisThisPlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XAxisThisWindow, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  
  connect( widget->_suppressLeft, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_suppressRight, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YIsLog, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_checkBoxYOffsetMode, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yReversed, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_checkBoxYInterpret, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_comboBoxYInterpret, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->_comboBoxYDisplay, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->_yTransformRight, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yMajorTickSpacing, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->_yMinorTicks, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yMinorTicksAuto, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yMarksInsidePlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yMarksOutsidePlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yMarksInsideAndOutsidePlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yMajorGrid, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_yMinorGrid, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YAxisThisPlot, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YAxisThisWindow, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));

  connect( widget->XAuto, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XAutoBorder, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XAutoUp, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XNoSpikes, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XAC, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XACRange, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->XExpression, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->XExpressionMin, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->XExpressionMax, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->YAuto, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YAutoBorder, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YAutoUp, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YExpression, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YAC, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->YACRange, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->YExpressionMin, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->YExpressionMax, SIGNAL( textChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->AddPlotMarker, SIGNAL( clicked() ), parent, SLOT(modified()));
  connect( widget->RemovePlotMarker, SIGNAL( clicked() ), parent, SLOT(modified()));
  connect( widget->RemoveAllPlotMarkers, SIGNAL( clicked() ), parent, SLOT(modified()));
  connect( widget->UseCurve, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->CurveCombo, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->Rising, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->Falling, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->UseVector, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_vectorForMarkers, SIGNAL( selectionChanged(const QString&) ), parent, SLOT(modified()));
  connect( widget->_colorMarker, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_checkBoxDefaultMarkerColor, SIGNAL( stateChanged(int) ), parent, SLOT(modified()));
  connect( widget->_comboMarkerLineStyle, SIGNAL( activated(int) ), parent, SLOT(modified()));
  connect( widget->_spinBoxMarkerLineWidth, SIGNAL( valueChanged(int) ), parent, SLOT(modified()));

}

QWidget *Kst2DPlot::configWidget() {
  return new View2DPlotWidget;
}

namespace {
KstViewObject *create_Kst2DPlot() {
  return new Kst2DPlot;
}

KstGfxMouseHandler *handler_Kst2DPlot() {
  return new KstGfx2DPlotMouseHandler;
}

KST_REGISTER_VIEW_OBJECT(Plot, create_Kst2DPlot, handler_Kst2DPlot)
}


#undef LABEL_PRECISION
#include "kst2dplot.moc"

// vim: ts=2 sw=2 et
