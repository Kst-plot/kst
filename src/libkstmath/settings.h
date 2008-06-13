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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qcolor.h>

#include "plotdefines.h"
#include "kst_export.h"

namespace Kst {

enum EMailEncryption {
  EMailEncryptionNone,
  EMailEncryptionSSL,
  EMailEncryptionTLS,
  EMailEncryptionMAXIMUM
};

enum EMailAuthentication {
  EMailAuthenticationPLAIN,
  EMailAuthenticationLOGIN,
  EMailAuthenticationCRAMMD5,
  EMailAuthenticationDIGESTMD5,
  EMailAuthenticationMAXIMUM
};

class Settings {
  public:
    KST_EXPORT Settings();
    KST_EXPORT Settings(const Settings&);
    KST_EXPORT Settings& operator=(const Settings&);

    // do not delete this object
    KST_EXPORT static Settings *globalSettings();
    KST_EXPORT static void setGlobalSettings(const Settings *settings);
    KST_EXPORT static void checkUpdates();
    KST_EXPORT void save();
    void reload();
    
    KST_EXPORT void setPrintingDefaults(); // set printing settings to default

    long plotUpdateTimer;
    long plotFontSize;
    long plotFontMinSize;
    QColor backgroundColor;
    QColor foregroundColor;
    QString curveColorSequencePalette;
    
    bool promptWindowClose : 1;
    bool showQuickStart : 1;
    bool tiedZoomGlobal : 1;
    bool xMajor : 1;
    bool yMajor : 1;
    bool xMinor : 1;
    bool yMinor : 1;
    bool majorGridColorDefault : 1;
    bool minorGridColorDefault : 1;
    bool xAxisInterpret : 1;
    bool yAxisInterpret : 1;
    bool emailRequiresAuthentication : 1;

    QColor majorColor;
    QColor minorColor;
    AxisInterpretationType xAxisInterpretation;
    AxisDisplayType xAxisDisplay;
    AxisInterpretationType yAxisInterpretation;
    AxisDisplayType yAxisDisplay;

    int defaultLineWeight;
    
    QString emailSender;
    QString emailSMTPServer;
    QString emailUsername;
    QString emailPassword;
    int     emailSMTPPort;
    EMailEncryption emailEncryption;
    EMailAuthentication emailAuthentication;

    KST_EXPORT int utcOffset();
    QString timezone;
    int offsetSeconds;

    struct PrintingMonochrome {
      QString enhanceReadability;
      
      // order of "-1" means property is not included in cycling
      QString pointStyleOrder;
      QString lineStyleOrder;
      QString lineWidthOrder;
      
      QString maxLineWidth;
      QString pointDensity;
    };
    struct Printing {
      QString pageSize;
      QString orientation;
      QString plotDateTimeFooter;
      QString maintainAspect;
      QString curveWidthAdjust;
      QString monochrome;
      PrintingMonochrome monochromeSettings;
    };
    Printing printing;

  private:
    static Settings *_self;
    static void cleanup();
};

}
#endif
// vim: ts=2 sw=2 et
