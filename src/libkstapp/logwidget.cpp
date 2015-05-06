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



#include <QDebug>

namespace Kst {


LogWidget::LogWidget(QWidget *parent)
  : QTextBrowser(parent) {
  _show = Debug::Warning | Debug::Error | Debug::Notice | Debug::Trace;
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
    case Debug::Trace:
        sym = "<img src=\"DebugTrace\"/> ";
        break;
    default:
      return;
  }

  if ((_show & int(msg.level)) == 0) {
    return;
  }

  append(QString("%1<b>%2</b> %3").arg(sym).arg(msg.date.toString()).arg(msg.msg));
}


void LogWidget::setShowLevel(Debug::LogLevel level, bool show) {
  const int old = _show;
  if (show) {
    _show |= level;
  } else {
    _show &= ~level;
  }
  if (_show != old) {
    regenerate();
  }
}


void LogWidget::setShowError(bool show) {
  setShowLevel(Debug::Error, show);
}


void LogWidget::setShowWarning(bool show) {
  setShowLevel(Debug::Warning, show);
}


void LogWidget::setShowNotice(bool show) {
  setShowLevel(Debug::Notice, show);
}


void LogWidget::setShowTrace(bool show) {
  setShowLevel(Debug::Trace, show);
}

void LogWidget::regenerate() {
  clear();
  const QList<Debug::LogMessage> messages = Debug::self()->messages();
  foreach(const Debug::LogMessage& message, messages) {
    logAdded(message);
  }
}

}

// vim: ts=2 sw=2 et
