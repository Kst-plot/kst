/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bugreportwizard.h"
#include "config.h"

#include "kst_i18n.h"

#include <QUrl>
#include <QDesktopServices>
#include <QDebug>

namespace Kst {

BugReportWizard::BugReportWizard(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  _kstVersion->setText(i18n("%1").arg(KSTVERSION));

#if defined(Q_OS_MAC9)
  _OS->setText("Mac OS 9");
#elif defined(Q_WS_MACX)
  _OS->setText("Mac OS X");
#elif defined(Q_WS_WIN32)
  _OS->setText("Windows 32-Bit");
#elif defined(Q_WS_WIN64)
  _OS->setText("Windows 64-Bit");
#else
  _OS->setText("Linux");
#endif

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
