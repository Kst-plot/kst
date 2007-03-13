/***************************************************************************
                              kstlogwidget.cpp
                             -------------------
    begin                : Fri Apr 01 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "kstlogwidget.h"
#include "kst.h"

#include <kglobal.h>
#include <klocale.h>

#include <qbitmap.h>
#include <qpainter.h>
#include <q3pointarray.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>


KstLogWidget::KstLogWidget(QWidget *parent, const char *name )
: Q3TextBrowser(parent, name) {
  //setTextFormat(LogText);
  //setMaxLogLines(KstDebug::self()->limit());
  setTextFormat(Qt::AutoText);
  _show = KstDebug::Warning | KstDebug::Error | KstDebug::Notice | KstDebug::Debug;
  generateImages();
  setMimeSourceFactory(&_msrc);
}


void KstLogWidget::setDebug(KstDebug *debug) {
  _debug = debug;
}


void KstLogWidget::logAdded(const KstDebug::LogMessage& msg) {
  QString sym;
  switch (msg.level) {
    case KstDebug::Warning:
      sym = "<img src=\"DebugWarning\"/> ";
      break;
    case KstDebug::Error:
      sym = "<img src=\"DebugError\"/> ";
      break;
    case KstDebug::Notice:
      sym = "<img src=\"DebugNotice\"/> ";
      break;
    case KstDebug::Debug:
      sym = "<img src=\"DebugDebug\"/> ";
      break;
    default:
      return;
  }

  if ((_show & int(msg.level)) == 0) {
    return;
  }

  append(i18n("%1<b>%2</b> %3").arg(sym).arg(KGlobal::locale()->formatDateTime(msg.date)).arg(msg.msg));
}


void KstLogWidget::setShowDebug(bool show) {
  int old = _show;
  if (show) {
    _show |= KstDebug::Debug;
  } else {
    _show &= ~KstDebug::Debug;
  }
  if (_show != old) {
    regenerate();
  }
}


void KstLogWidget::setShowNotice(bool show) {
  int old = _show;
  if (show) {
    _show |= KstDebug::Notice;
  } else {
    _show &= ~KstDebug::Notice;
  }
  if (_show != old) {
    regenerate();
  }
}


void KstLogWidget::setShowWarning(bool show) {
  int old = _show;
  if (show) {
    _show |= KstDebug::Warning;
  } else {
    _show &= ~KstDebug::Warning;
  }
  if (_show != old) {
    regenerate();
  }
}


void KstLogWidget::setShowError(bool show) {
  int old = _show;
  if (show) {
    _show |= KstDebug::Error;
  } else {
    _show &= ~KstDebug::Error;
  }
  if (_show != old) {
    regenerate();
  }
}


void KstLogWidget::clear() {
  Q3TextBrowser::clear();
  KstApp::inst()->destroyDebugNotifier();
}


void KstLogWidget::regenerate() {
  clear();
  Q3ValueList<KstDebug::LogMessage> msgs = KstDebug::self()->messages();

  Q3ValueListConstIterator<KstDebug::LogMessage> it = msgs.begin();
  while (it != msgs.end()) {
    logAdded(*it);
    ++it;
  }

  scrollToBottom();
}


void KstLogWidget::generateImages() {
  Q3PointArray pointArray;
  QPixmap pixmap;
  QPainter painter;

  int height = 14, margin = 1, step = (height - margin * 2) / 4;
  // Based on Kst code Copyright 2004 The University of British Columbia
  pixmap = QPixmap(height, height);
  pixmap.fill();
  painter.begin(&pixmap);
  painter.setBrush(QColor("LightSeaGreen"));
  painter.drawEllipse(margin, margin, height - margin * 2, height - margin * 2);
  painter.end();
  pixmap.setMask(pixmap.createHeuristicMask(true));
  _msrc.setPixmap("DebugNotice", pixmap);

  pixmap = QPixmap(height, height);
  pixmap.fill();
  painter.begin(&pixmap);
  pointArray.putPoints(0, 3, margin, margin,
      height - margin, margin,
      height / 2, height - margin);
  painter.setBrush(QColor("DarkOrange"));
  painter.drawPolygon(pointArray);
  painter.end();
  pixmap.setMask(pixmap.createHeuristicMask(true));
  _msrc.setPixmap("DebugWarning", pixmap);

  pixmap = QPixmap(height, height);
  pixmap.fill();
  painter.begin(&pixmap);
  painter.setBrush(QColor("Red"));
  pointArray.putPoints(0, 8,
      margin + ( 0 * step ), margin + ( 1 * step ),
      margin + ( 0 * step ), margin + ( 3 * step ),
      margin + ( 1 * step ), margin + ( 4 * step ),
      margin + ( 3 * step ), margin + ( 4 * step ),
      margin + ( 4 * step ), margin + ( 3 * step ),
      margin + ( 4 * step ), margin + ( 1 * step ),
      margin + ( 3 * step ), margin + ( 0 * step ),
      margin + ( 1 * step ), margin + ( 0 * step ));
  painter.drawPolygon(pointArray);
  painter.end();
  pixmap.setMask(pixmap.createHeuristicMask(true));
  _msrc.setPixmap("DebugError", pixmap);

  pixmap = QPixmap(height, height);
  pixmap.fill();
  painter.begin(&pixmap);
  painter.setBrush(QColor("DeepSkyBlue"));
  painter.drawRoundRect(margin, margin,
      height - 2 * margin, height - 2 * margin,
      (height - 2 * margin) / 3, (height - 2 * margin) / 3);
  painter.end();
  pixmap.setMask(pixmap.createHeuristicMask(true));
  _msrc.setPixmap("DebugDebug", pixmap);
}

#include "kstlogwidget.moc"
// vim: ts=2 sw=2 et
