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

#ifdef KST_HAVE_REVISION_H
#include "kstrevision.h"
#endif

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

  QString version = QString(KSTVERSION)
#ifdef KST_REVISION
      +KST_REVISION
#endif
      ;

  _kstVersion->setText(version);

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
  QUrl url("https://bugs.kde.org/enter_bug.cgi");
  QUrlQuery query;

  query.addQueryItem("product", "kst");
  query.addQueryItem("op_sys", _OS->text());

  // query.addQueryItem("os", _OS->text());
  // query.addQueryItem("appVersion", _kstVersion->text());
  // query.addQueryItem("package", "kst");
  // query.addQueryItem("kbugreport", "1");
  // query.addQueryItem("kdeVersion", "unspecified");
  url.setQuery(query);
  QDesktopServices::openUrl(url);
}

}

// vim: ts=2 sw=2 et
