/***************************************************************************
                              debugnotifier.cpp
                              -----------------
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

#include "debugnotifier.h"

#include <QMouseEvent>
#include <QTimer>
#include <QToolTip>

#define TIMER_TICK 250
#define TIMER_ITERATIONS 10

namespace Kst {

DebugNotifier::DebugNotifier(QWidget *parent)
: QLabel(parent) {
  _animationStage = 0;
  _gotPress = false;
  setToolTip(tr("An error has occurred.  Click on this icon for more details."));
#define IMAGE_STAGES 2
  _pm.resize(IMAGE_STAGES);
  _pm[0] = QPixmap("kst/pics/kst_error_1.png"); // FIXME
  _pm[1] = QPixmap("kst/pics/kst_error_2.png"); // FIXME
  setPixmap(_pm[0]);
  show();
  QTimer::singleShot(TIMER_TICK, this, SLOT(animate()));
}


DebugNotifier::~DebugNotifier() {
}


void DebugNotifier::close() {
  _animationStage = 999;
  deleteLater();
}


void DebugNotifier::animate() {
  if (_animationStage > TIMER_ITERATIONS) {
    setPixmap(_pm[0]);
    return;
  }

  setPixmap(_pm[_animationStage % IMAGE_STAGES]);
  QTimer::singleShot(TIMER_TICK, this, SLOT(animate()));
  ++_animationStage;
}


void DebugNotifier::reanimate() {
  if (_animationStage <= TIMER_ITERATIONS) {
    return;
  }

  _animationStage = 0;
  setPixmap(_pm[0]);
  QTimer::singleShot(TIMER_TICK, this, SLOT(animate()));
}


void DebugNotifier::mousePressEvent(QMouseEvent *e) {
  if (rect().contains(e->pos())) {
    _gotPress = true;
  }
}


void DebugNotifier::mouseReleaseEvent(QMouseEvent *e) {
  if (_gotPress && rect().contains(e->pos()) && e->button() == Qt::LeftButton) {
    emit showDebugLog();
    close();
  }
  _gotPress = false;
}

}

#undef TIMER_ITERATIONS
#include "debugnotifier.moc"
// vim: ts=2 sw=2 et
