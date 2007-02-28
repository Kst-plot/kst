/***************************************************************************
                            kstdebugnotifier.cpp
                             -------------------
    begin                : Sep 13 2005
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

#include "kstdebugnotifier.h"

#include "kst.h"

#include <klocale.h>
#include <kstandarddirs.h>

#include <qtimer.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>
#include <QLabel>

#define TIMER_TICK 250
#define TIMER_ITERATIONS 10

KstDebugNotifier::KstDebugNotifier(QWidget *parent)
: QLabel(parent) {
  _animationStage = 0;
  _gotPress = false;
  QToolTip::add(this, i18n("An error has occurred.  Click on this icon for more details."));
#define IMAGE_STAGES 2
  _pm.resize(IMAGE_STAGES);
  _pm[0] = QPixmap(locate("data", "kst/pics/kst_error_1.png"));
  _pm[1] = QPixmap(locate("data", "kst/pics/kst_error_2.png"));
  setPixmap(_pm[0]);
  show();
  QTimer::singleShot(TIMER_TICK, this, SLOT(animate()));
}


KstDebugNotifier::~KstDebugNotifier() {
}


void KstDebugNotifier::close() {
  _animationStage = 999;
  deleteLater();
}


void KstDebugNotifier::showDebugLog() {
  KstApp::inst()->showDebugLog();
}


void KstDebugNotifier::animate() {
  if (_animationStage > TIMER_ITERATIONS) {
    setPixmap(_pm[0]);
    return;
  }

  setPixmap(_pm[_animationStage % IMAGE_STAGES]);
  QTimer::singleShot(TIMER_TICK, this, SLOT(animate()));
  ++_animationStage;
}


void KstDebugNotifier::reanimate() {
  if (_animationStage <= TIMER_ITERATIONS) {
    return;
  }

  _animationStage = 0;
  setPixmap(_pm[0]);
  QTimer::singleShot(TIMER_TICK, this, SLOT(animate()));
}


void KstDebugNotifier::mousePressEvent(QMouseEvent *e) {
  if (rect().contains(e->pos())) {
    _gotPress = true;
  }
}


void KstDebugNotifier::mouseReleaseEvent(QMouseEvent *e) {
  if (_gotPress && rect().contains(e->pos()) && e->button() == Qt::LeftButton) {
    showDebugLog();
    close();
  }
  _gotPress = false;
}

#undef TIMER_ITERATIONS
#include "kstdebugnotifier.moc"
// vim: ts=2 sw=2 et
