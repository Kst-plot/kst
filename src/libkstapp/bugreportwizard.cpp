/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <config.h>

#include "bugreportwizard.h"



#include <QUrl>
#include <QDesktopServices>
#include <QDebug>

namespace Kst {

BugReportWizard::BugReportWizard(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  QString mac_os9 = QT_TR_NOOP("Mac OS 9");
  QString mac_osx = QT_TR_NOOP("Mac OS X");
  QString win32 = QT_TR_NOOP("Windows 32-Bit");
  QString win64 = QT_TR_NOOP("Windows 64-Bit");
  QString lin = QT_TR_NOOP("Linux");

  _kstVersion->setText(KSTVERSION);

#if defined(Q_OS_MAC9)
  os_en = mac_os9;
#elif defined(Q_WS_MACX)
  os_en = mac_osx;
#elif defined(Q_OS_WIN32)
  os_en = win32;
#elif defined(Q_OS_WIN64)
  os_en = win64;
#else
  os_en = lin;
#endif

  _OS->setText(tr(os_en.toLatin1()));

  connect(_reportBugButton, SIGNAL(clicked()), this, SLOT(reportBug()));
}


BugReportWizard::~BugReportWizard() {
}


void BugReportWizard::reportBug() {
  QUrl url("http://bugs.kde.org/wizard.cgi");
  url.addQueryItem("os", _OS->text());
  url.addQueryItem("appVersion", _kstVersion->text());
  url.addQueryItem("package", "kst");
  url.addQueryItem("kbugreport", "1");
  url.addQueryItem("kdeVersion", "unspecified");

  QDesktopServices::openUrl(url);
}

}

// vim: ts=2 sw=2 et
