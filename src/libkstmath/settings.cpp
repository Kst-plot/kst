/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2003 The University of Toronto                        *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qprinter.h>
#include "timezones.h"

#include <qsettings.h>
#include <qapplication.h>

// application specific includes
#include "settings.h"

namespace Kst {

Settings::Settings() {
  plotUpdateTimer = 200;
  plotFontSize    = 12;
  plotFontMinSize = 5;
  backgroundColor = QColor(255, 255, 255); // white
  foregroundColor = QColor(0,0,0); // black
  promptWindowClose = true;
  showQuickStart = true;
  tiedZoomGlobal = false;
  curveColorSequencePalette = "Kst Colors";
  
  xMajor = false;
  yMajor = false;
  xMinor = false;
  yMinor = false;
  majorColor = QColor(128,128,128);
  minorColor = QColor(128,128,128);
  majorGridColorDefault = true;
  minorGridColorDefault = true;

  xAxisInterpret = false;
  xAxisInterpretation = AXIS_INTERP_CTIME;
  xAxisDisplay = AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS;
  yAxisInterpret = false;
  yAxisInterpretation = AXIS_INTERP_CTIME;
  yAxisDisplay = AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS;
  
  defaultLineWeight = 0;
  
  timezone = "UTC";
  offsetSeconds = 0;
  
  setPrintingDefaults();
}


Settings::Settings(const Settings& x) {
  *this = x;
}


Settings& Settings::operator=(const Settings& x) {
  plotUpdateTimer = x.plotUpdateTimer;
  plotFontSize    = x.plotFontSize;
  plotFontMinSize = x.plotFontMinSize;
  backgroundColor = x.backgroundColor;
  foregroundColor = x.foregroundColor;
  promptWindowClose = x.promptWindowClose;
  showQuickStart = x.showQuickStart;
  tiedZoomGlobal = x.tiedZoomGlobal;
  curveColorSequencePalette = x.curveColorSequencePalette;
  
  timezone = x.timezone;
  offsetSeconds = x.offsetSeconds;
  
  xMajor = x.xMajor;
  yMajor = x.yMajor;
  xMinor = x.xMinor;
  yMinor = x.yMinor;
  majorColor = x.majorColor;
  minorColor = x.minorColor;
  majorGridColorDefault = x.majorGridColorDefault;
  minorGridColorDefault = x.minorGridColorDefault;

  xAxisInterpret = x.xAxisInterpret;
  xAxisInterpretation = x.xAxisInterpretation;
  xAxisDisplay = x.xAxisDisplay;
  yAxisInterpret = x.yAxisInterpret;
  yAxisInterpretation = x.yAxisInterpretation;
  yAxisDisplay = x.yAxisDisplay;

  defaultLineWeight = x.defaultLineWeight;
  
  printing.pageSize = x.printing.pageSize;
  printing.orientation = x.printing.orientation;
  printing.plotDateTimeFooter = x.printing.plotDateTimeFooter;
  printing.maintainAspect = x.printing.maintainAspect;
  printing.curveWidthAdjust = x.printing.curveWidthAdjust;
  printing.monochrome = x.printing.monochrome;
  
  printing.monochromeSettings.enhanceReadability = 
      x.printing.monochromeSettings.enhanceReadability;
  printing.monochromeSettings.pointStyleOrder = 
      x.printing.monochromeSettings.pointStyleOrder;
  printing.monochromeSettings.lineStyleOrder = 
      x.printing.monochromeSettings.lineStyleOrder;
  printing.monochromeSettings.lineWidthOrder = 
      x.printing.monochromeSettings.lineWidthOrder;
  printing.monochromeSettings.maxLineWidth = 
      x.printing.monochromeSettings.maxLineWidth;
  printing.monochromeSettings.pointDensity = 
      x.printing.monochromeSettings.pointDensity;

  return *this;
}


Settings *Settings::_self = 0L;
void Settings::cleanup() {
    delete _self;
    _self = 0;
}


Settings *Settings::globalSettings() {
  if (!_self) {
    _self = new Settings;
    qAddPostRoutine(Settings::cleanup);
    _self->reload();
  }

  return _self;
}


void Settings::setGlobalSettings(const Settings *settings) {
  globalSettings(); // force instantiation
  *_self = *settings;
}


void Settings::save() {
  QSettings cfg(QString("kstrc"));

  cfg.beginGroup("Kst");
  cfg.setValue("Plot Update Timer", qlonglong(plotUpdateTimer));
  cfg.setValue("Plot Font Size", qlonglong(plotFontSize));
  cfg.setValue("Plot Font Min Size", qlonglong(plotFontMinSize));
  cfg.setValue("Background Color", backgroundColor);
  cfg.setValue("Foreground Color", foregroundColor);
  cfg.setValue("Prompt on Window Close", promptWindowClose);
  cfg.setValue("Show QuickStart", showQuickStart);
  cfg.setValue("Tied-zoom Global", tiedZoomGlobal);
  cfg.setValue("Curve Color Sequence", curveColorSequencePalette);

  cfg.setValue("Timezone", timezone);
  cfg.setValue("OffsetSeconds", offsetSeconds);
  cfg.endGroup();

  cfg.beginGroup("Grid Lines");
  cfg.setValue("X Major", xMajor);
  cfg.setValue("Y Major", yMajor);
  cfg.setValue("X Minor", xMinor);
  cfg.setValue("Y Minor", yMinor);
  cfg.setValue("Major Color", majorColor);
  cfg.setValue("Minor Color", minorColor);
  cfg.setValue("Default Major Color", majorGridColorDefault);
  cfg.setValue("Default Minor Color", minorGridColorDefault);
  cfg.endGroup();

  cfg.beginGroup("X Axis");
  cfg.setValue("Interpret", xAxisInterpret);
  cfg.setValue("Interpretation", int(xAxisInterpretation));
  cfg.setValue("Display", int(xAxisDisplay));
  cfg.endGroup();

  cfg.beginGroup("Y Axis");
  cfg.setValue("Interpret", yAxisInterpret);
  cfg.setValue("Interpretation", int(yAxisInterpretation));
  cfg.setValue("Display", int(yAxisDisplay));
  cfg.endGroup();

  cfg.beginGroup("Curve");
  cfg.setValue("DefaultLineWeight", defaultLineWeight);
  cfg.endGroup();

  cfg.beginGroup("Printing");
  cfg.setValue("kde-pagesize", printing.pageSize);
  cfg.setValue("kde-orientation", printing.orientation);
  cfg.setValue("kst-plot-datetime-footer", printing.plotDateTimeFooter);
  cfg.setValue("kst-plot-maintain-aspect-ratio", printing.maintainAspect);
  cfg.setValue("kst-plot-curve-width-adjust", printing.curveWidthAdjust);
  cfg.setValue("kst-plot-monochrome", printing.monochrome);

  cfg.setValue("kst-plot-monochromesettings-enhancereadability",
                 printing.monochromeSettings.enhanceReadability);
  cfg.setValue("kst-plot-monochromesettings-pointstyleorder",
                 printing.monochromeSettings.pointStyleOrder);
  cfg.setValue("kst-plot-monochromesettings-linestyleorder",
                 printing.monochromeSettings.lineStyleOrder);
  cfg.setValue("kst-plot-monochromesettings-linewidthorder",
                 printing.monochromeSettings.lineWidthOrder);
  cfg.setValue("kst-plot-monochromesettings-maxlinewidth",
                 printing.monochromeSettings.maxLineWidth);
  cfg.setValue("kst-plot-monochromesettings-pointdensity",
                 printing.monochromeSettings.pointDensity);
  cfg.endGroup();

  cfg.sync();
}


void Settings::reload() {
  QSettings cfg("kstrc");

  cfg.beginGroup("Kst");
  plotUpdateTimer = cfg.value("Plot Update Timer", 200).toInt();
  plotFontSize    = cfg.value("Plot Font Size", 12).toInt();
  plotFontMinSize = cfg.value("Plot Font Min Size", 5).toInt();
  backgroundColor = cfg.value("Background Color", backgroundColor).value<QColor>();
  foregroundColor = cfg.value("Foreground Color", foregroundColor).value<QColor>();
  promptWindowClose = cfg.value("Prompt on Window Close", true).toBool();
  showQuickStart = cfg.value("Show QuickStart", true).toBool();
  tiedZoomGlobal = cfg.value("Tied-zoom Global", true).toBool();
  curveColorSequencePalette = cfg.value("Curve Color Sequence", "Kst Colors").toString();

  timezone = cfg.value("Timezone", "UTC").toString();
  offsetSeconds = cfg.value("OffsetSeconds", 0).toBool();
  cfg.endGroup();

  cfg.beginGroup("Grid Lines");
  xMajor = cfg.value("X Major", false).toBool();
  yMajor = cfg.value("Y Major", false).toBool();
  xMinor = cfg.value("X Minor", false).toBool();
  yMinor = cfg.value("Y Minor", false).toBool();
  majorColor = cfg.value("Major Color", majorColor).value<QColor>();
  minorColor = cfg.value("Minor Color", minorColor).value<QColor>();
  majorGridColorDefault = cfg.value("Default Major Color", true).toBool();
  minorGridColorDefault = cfg.value("Default Minor Color", true).toBool();
  cfg.endGroup();

  cfg.beginGroup("X Axis");
  xAxisInterpret = cfg.value("Interpret", false).toBool();
  xAxisInterpretation = (AxisInterpretationType)cfg.value("Interpretation", (int)AXIS_INTERP_CTIME).toInt();
  xAxisDisplay = (AxisDisplayType)cfg.value("Display", static_cast<int>(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS)).toInt();
  cfg.endGroup();

  cfg.beginGroup("Y Axis");
  yAxisInterpret = cfg.value("Interpret", false).toBool();
  yAxisInterpretation = (AxisInterpretationType)cfg.value("Interpretation", (int)AXIS_INTERP_CTIME).toInt();
  yAxisDisplay = (AxisDisplayType)cfg.value("Display", static_cast<int>(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS)).toInt();
  cfg.endGroup();

  cfg.beginGroup("Curve");
  defaultLineWeight = cfg.value("DefaultLineWeight", 0).toInt();
  cfg.endGroup();

  cfg.beginGroup("Printing");
  printing.pageSize = cfg.value("kde-pagesize", QString::number((int)QPrinter::Letter)).toString();
  printing.orientation = cfg.value("kde-orientation", "Landscape").toString();
  printing.plotDateTimeFooter = cfg.value("kst-plot-datetime-footer", "0").toString();
  printing.maintainAspect = cfg.value("kst-plot-maintain-aspect-ratio", "0").toString();
  printing.curveWidthAdjust = cfg.value("kst-plot-curve-width-adjust", "0").toString();
  printing.monochrome = cfg.value("kst-plot-monochrome", "0").toString();
  printing.monochromeSettings.enhanceReadability = 
      cfg.value("kst-plot-monochromesettings-enhancereadability", "0").toString();
  printing.monochromeSettings.pointStyleOrder = 
      cfg.value("kst-plot-monochromesettings-pointstyleorder", "0").toString();
  printing.monochromeSettings.lineStyleOrder = 
      cfg.value("kst-plot-monochromesettings-linestyleorder", "1").toString();
  printing.monochromeSettings.lineWidthOrder = 
      cfg.value("kst-plot-monochromesettings-linewidthorder", "2").toString();
  printing.monochromeSettings.maxLineWidth = 
      cfg.value("kst-plot-monochromesettings-maxlinewidth", "3").toString();
  printing.monochromeSettings.pointDensity = 
      cfg.value("kst-plot-monochromesettings-pointdensity", "2").toString();
  cfg.endGroup();
}


void Settings::checkUpdates() {
  // ### Do KDE3->4 update (?)
}


void Settings::setPrintingDefaults() {
  printing.pageSize = QString::number((int)QPrinter::Letter);
  printing.orientation = "Landscape";
  printing.plotDateTimeFooter = "0";
  printing.maintainAspect = "0";
  printing.curveWidthAdjust = "0";
  printing.monochrome = "0";
  printing.monochromeSettings.enhanceReadability = "0";
  printing.monochromeSettings.pointStyleOrder = "0";
  printing.monochromeSettings.lineStyleOrder = "1";
  printing.monochromeSettings.lineWidthOrder = "2";
  printing.monochromeSettings.maxLineWidth = "3";
  printing.monochromeSettings.pointDensity = "2";
}


int Settings::utcOffset() {
  return offsetSeconds;
}

}

// vim: ts=2 sw=2 et
