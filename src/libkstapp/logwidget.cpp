/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "logwidget.h"
#include <debug.h>
#include <events.h>
#include <logevents.h>

#include "kst_i18n.h"

#include <QDebug>

namespace Kst {

LogWidget::LogWidget(QWidget *parent)
  : QTextBrowser(parent) {
  _show = Debug::Warning | Debug::Error | Debug::Notice | Debug::DebugLog;
}


LogWidget::~LogWidget() {
}


void LogWidget::logAdded(const Debug::LogMessage& msg) {
  QString sym;
  switch (msg.level) {
    case Debug::Warning:
      sym = "<img src=\"DebugWarning\"/> ";
      break;
    case Debug::Error:
      sym = "<img src=\"DebugError\"/> ";
      break;
    case Debug::Notice:
      sym = "<img src=\"DebugNotice\"/> ";
      break;
    case Debug::DebugLog:
      sym = "<img src=\"DebugDebug\"/> ";
      break;
    default:
      return;
  }

  if ((_show & int(msg.level)) == 0) {
    return;
  }

  append(i18n("%1<b>%2</b> %3").arg(sym).arg(msg.date.toString()).arg(msg.msg));
}


void LogWidget::setShowDebug(bool show) {
  int old = _show;
  if (show) {
    _show |= Debug::DebugLog;
  } else {
    _show &= ~Debug::DebugLog;
  }
  if (_show != old) {
    regenerate();
  }
}


void LogWidget::setShowNotice(bool show) {
  int old = _show;
  if (show) {
    _show |= Debug::Notice;
  } else {
    _show &= ~Debug::Notice;
  }
  if (_show != old) {
    regenerate();
  }
}


void LogWidget::setShowWarning(bool show) {
  int old = _show;
  if (show) {
    _show |= Debug::Warning;
  } else {
    _show &= ~Debug::Warning;
  }
  if (_show != old) {
    regenerate();
  }
}


void LogWidget::setShowError(bool show) {
  int old = _show;
  if (show) {
    _show |= Debug::Error;
  } else {
    _show &= ~Debug::Error;
  }
  if (_show != old) {
    regenerate();
  }
}


void LogWidget::regenerate() {
  clear();
  foreach(Debug::LogMessage message, Debug::self()->messages()) {
    logAdded(message);
  }
}

}

// vim: ts=2 sw=2 et
