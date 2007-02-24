/***************************************************************************
             kstsettings.cpp: a collection of settings for kst
                             -------------------
    begin                : Nov 23, 2003
    copyright            : (C) 2003 The University of Toronto
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

// include files for KDE
#include <kconfig.h>
#include <kemailsettings.h>
#include <kprinter.h>
#include <kstaticdeleter.h>
#include "ksttimezones.h"

// application specific includes
#include "kstsettings.h"

KstSettings::KstSettings() {
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
  
  emailSMTPPort = 25;
  emailRequiresAuthentication = false;
  emailEncryption = EMailEncryptionNone;
  emailAuthentication = EMailAuthenticationPLAIN;

  timezone = "UTC";
  offsetSeconds = 0;
  
  setPrintingDefaults();
}


KstSettings::KstSettings(const KstSettings& x) {
  *this = x;
}


KstSettings& KstSettings::operator=(const KstSettings& x) {
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
  
  emailSender = x.emailSender;
  emailSMTPServer = x.emailSMTPServer;
  emailSMTPPort = x.emailSMTPPort;
  emailRequiresAuthentication = x.emailRequiresAuthentication;
  emailUsername = x.emailUsername;
  emailPassword = x.emailPassword;
  emailEncryption = x.emailEncryption;
  emailAuthentication = x.emailAuthentication;

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


KstSettings *KstSettings::_self = 0L;
static KStaticDeleter<KstSettings> kstsettingssd;

KstSettings *KstSettings::globalSettings() {
  if (!_self) {
    kstsettingssd.setObject(_self, new KstSettings);
    _self->reload();
  }

  return _self;
}


void KstSettings::setGlobalSettings(const KstSettings *settings) {
  globalSettings(); // force instantiation
  *_self = *settings;
}


void KstSettings::save() {
  KConfig c(QString("kstrc"));
  KConfigGroup cfg(&c, "Kst");

  cfg.writeEntry("Plot Update Timer", qlonglong(plotUpdateTimer));
  cfg.writeEntry("Plot Font Size", qlonglong(plotFontSize));
  cfg.writeEntry("Plot Font Min Size", qlonglong(plotFontMinSize));
  cfg.writeEntry("Background Color", backgroundColor);
  cfg.writeEntry("Foreground Color", foregroundColor);
  cfg.writeEntry("Prompt on Window Close", promptWindowClose);
  cfg.writeEntry("Show QuickStart", showQuickStart);
  cfg.writeEntry("Tied-zoom Global", tiedZoomGlobal);
  cfg.writeEntry("Curve Color Sequence", curveColorSequencePalette);
  
  cfg.writeEntry("Timezone", timezone);
  cfg.writeEntry("OffsetSeconds", offsetSeconds);
  
  cfg.changeGroup("Grid Lines");
  cfg.writeEntry("X Major", xMajor);
  cfg.writeEntry("Y Major", yMajor);
  cfg.writeEntry("X Minor", xMinor);
  cfg.writeEntry("Y Minor", yMinor);
  cfg.writeEntry("Major Color", majorColor);
  cfg.writeEntry("Minor Color", minorColor);
  cfg.writeEntry("Default Major Color", majorGridColorDefault);
  cfg.writeEntry("Default Minor Color", minorGridColorDefault);

  cfg.changeGroup("X Axis");
  cfg.writeEntry("Interpret", xAxisInterpret);
  cfg.writeEntry("Interpretation", int(xAxisInterpretation));
  cfg.writeEntry("Display", int(xAxisDisplay));

  cfg.changeGroup("Y Axis");
  cfg.writeEntry("Interpret", yAxisInterpret);
  cfg.writeEntry("Interpretation", int(yAxisInterpretation));
  cfg.writeEntry("Display", int(yAxisDisplay));

  cfg.changeGroup("Curve");
  cfg.writeEntry("DefaultLineWeight", defaultLineWeight);
  
  cfg.changeGroup("EMail");
  cfg.writeEntry("Sender", emailSender);
  cfg.writeEntry("Server", emailSMTPServer);
  cfg.writeEntry("Port", emailSMTPPort);
  cfg.writeEntry("Authenticate", emailRequiresAuthentication);
  cfg.writeEntry("Username", emailUsername);
  cfg.writeEntry("Password", emailPassword);
  cfg.writeEntry("Encryption", int(emailEncryption));
  cfg.writeEntry("Authentication", int(emailAuthentication));

  cfg.changeGroup("Printing");
  cfg.writeEntry("kde-pagesize", printing.pageSize);
  cfg.writeEntry("kde-orientation", printing.orientation);
  cfg.writeEntry("kst-plot-datetime-footer", printing.plotDateTimeFooter);
  cfg.writeEntry("kst-plot-maintain-aspect-ratio", printing.maintainAspect);
  cfg.writeEntry("kst-plot-curve-width-adjust", printing.curveWidthAdjust);
  cfg.writeEntry("kst-plot-monochrome", printing.monochrome);
  
  cfg.writeEntry("kst-plot-monochromesettings-enhancereadability", 
                 printing.monochromeSettings.enhanceReadability);
  cfg.writeEntry("kst-plot-monochromesettings-pointstyleorder",
                 printing.monochromeSettings.pointStyleOrder);
  cfg.writeEntry("kst-plot-monochromesettings-linestyleorder",
                 printing.monochromeSettings.lineStyleOrder);
  cfg.writeEntry("kst-plot-monochromesettings-linewidthorder",
                 printing.monochromeSettings.lineWidthOrder);
  cfg.writeEntry("kst-plot-monochromesettings-maxlinewidth",
                 printing.monochromeSettings.maxLineWidth);
  cfg.writeEntry("kst-plot-monochromesettings-pointdensity",
                 printing.monochromeSettings.pointDensity);

  c.sync();
}


void KstSettings::reload() {
  KConfig c("kstrc");
  KConfigGroup cfg(&c, "Kst");

  plotUpdateTimer = cfg.readEntry("Plot Update Timer", 200);
  plotFontSize    = cfg.readEntry("Plot Font Size", 12);
  plotFontMinSize = cfg.readEntry("Plot Font Min Size", 5);
  backgroundColor = cfg.readEntry("Background Color", backgroundColor);
  foregroundColor = cfg.readEntry("Foreground Color", foregroundColor);
  promptWindowClose = cfg.readEntry("Prompt on Window Close", true);
  showQuickStart = cfg.readEntry("Show QuickStart", true);
  tiedZoomGlobal = cfg.readEntry("Tied-zoom Global", true);
  curveColorSequencePalette = cfg.readEntry("Curve Color Sequence", "Kst Colors");
  
  timezone = cfg.readEntry("Timezone", "UTC");
  offsetSeconds = cfg.readEntry("OffsetSeconds", 0);
  
  cfg.changeGroup("Grid Lines");
  xMajor = cfg.readEntry("X Major", false);
  yMajor = cfg.readEntry("Y Major", false);
  xMinor = cfg.readEntry("X Minor", false);
  yMinor = cfg.readEntry("Y Minor", false);
  majorColor = cfg.readEntry("Major Color", majorColor);
  minorColor = cfg.readEntry("Minor Color", minorColor);
  majorGridColorDefault = cfg.readEntry("Default Major Color", true);
  minorGridColorDefault = cfg.readEntry("Default Minor Color", true);

  cfg.changeGroup("X Axis");
  xAxisInterpret = cfg.readEntry("Interpret", false);
  xAxisInterpretation = (KstAxisInterpretation)cfg.readEntry("Interpretation", (int)AXIS_INTERP_CTIME);
  xAxisDisplay = (KstAxisDisplay)cfg.readEntry("Display", static_cast<int>(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS));

  cfg.changeGroup("Y Axis");
  yAxisInterpret = cfg.readEntry("Interpret", false);
  yAxisInterpretation = (KstAxisInterpretation)cfg.readEntry("Interpretation", (int)AXIS_INTERP_CTIME);
  yAxisDisplay = (KstAxisDisplay)cfg.readEntry("Display", static_cast<int>(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS));

  cfg.changeGroup("Curve");
  defaultLineWeight = cfg.readEntry("DefaultLineWeight", 0);
  
  cfg.changeGroup("EMail");
  KEMailSettings es;
  emailSender = cfg.readEntry("Sender", es.getSetting(KEMailSettings::EmailAddress));
  emailSMTPServer = cfg.readEntry("Server", es.getSetting(KEMailSettings::OutServer));
  emailSMTPPort = cfg.readEntry("Port", 25); // FIXME: no KEMailSettings for this?
  emailRequiresAuthentication = cfg.readEntry("Authenticate", !es.getSetting(KEMailSettings::OutServerLogin).isEmpty());
  emailUsername = cfg.readEntry("Username", es.getSetting(KEMailSettings::OutServerLogin));
  emailPassword = cfg.readEntry("Password", es.getSetting(KEMailSettings::OutServerPass));
  emailEncryption = (EMailEncryption)cfg.readEntry("Encryption", static_cast<int>(es.getSetting(KEMailSettings::OutServerTLS) == "true" ? EMailEncryptionTLS : EMailEncryptionNone));
  emailAuthentication = (EMailAuthentication)cfg.readEntry("Authentication", static_cast<int>(EMailAuthenticationPLAIN)); // FIXME: no KEMailSettings for this?

  cfg.changeGroup("Printing");
  printing.pageSize = cfg.readEntry("kde-pagesize", QString::number((int)KPrinter::Letter));
  printing.orientation = cfg.readEntry("kde-orientation", "Landscape");
  printing.plotDateTimeFooter = cfg.readEntry("kst-plot-datetime-footer", "0");
  printing.maintainAspect = cfg.readEntry("kst-plot-maintain-aspect-ratio", "0");
  printing.curveWidthAdjust = cfg.readEntry("kst-plot-curve-width-adjust", "0");
  printing.monochrome = cfg.readEntry("kst-plot-monochrome", "0");
  printing.monochromeSettings.enhanceReadability = 
      cfg.readEntry("kst-plot-monochromesettings-enhancereadability", "0");
  printing.monochromeSettings.pointStyleOrder = 
      cfg.readEntry("kst-plot-monochromesettings-pointstyleorder", "0");
  printing.monochromeSettings.lineStyleOrder = 
      cfg.readEntry("kst-plot-monochromesettings-linestyleorder", "1");
  printing.monochromeSettings.lineWidthOrder = 
      cfg.readEntry("kst-plot-monochromesettings-linewidthorder", "2");
  printing.monochromeSettings.maxLineWidth = 
      cfg.readEntry("kst-plot-monochromesettings-maxlinewidth", "3");
  printing.monochromeSettings.pointDensity = 
      cfg.readEntry("kst-plot-monochromesettings-pointdensity", "2");
}


void KstSettings::checkUpdates() {
  // ### Do KDE3->4 update (?)
}


void KstSettings::setPrintingDefaults() {
  printing.pageSize = QString::number((int)KPrinter::Letter);
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


int KstSettings::utcOffset() {
  return offsetSeconds;
}


// vim: ts=2 sw=2 et
