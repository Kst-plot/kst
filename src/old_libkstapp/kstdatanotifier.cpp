/***************************************************************************
                             kstdatanotifier.cpp
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

#include "kstdatanotifier.h"

#include "kst.h"

#include <klocale.h>

#include <qcolor.h>
#include <qtimer.h>
#include <qtooltip.h>

KstDataNotifier::KstDataNotifier(QWidget *parent)
: KLed(QColor(0, 255, 0), parent) {
  off();
  _animationStage = 0;
  setToolTip(i18n("Indicates that new data has arrived."));
  show();
}


KstDataNotifier::~KstDataNotifier() {
}


// FIXME: add more levels to make it smoother
static const int delays[] = { 900, 675, 425, 300, 200, 100,  80,  40,  20 };
static const int dark[] =   { 100, 150, 200, 250, 300, 375, 450, 525, 600 };
#define TIMER_ITERATIONS sizeof(delays)/sizeof(int)

// FIXME: use a QTimer object instead.  faster?
void KstDataNotifier::arrived() {
  if (state() == KLed::On) {
    _animationStage = 0;
    setColor(QColor(0, 255, 0));
  } else {
    QTimer::singleShot(0, this, SLOT(animate()));
  }
}


// FIXME: pre-cache the colors, not just the dark numbers
void KstDataNotifier::animate() {
  if (state() == KLed::Off) {
    on();
    setColor(QColor(0, 255, 0));
    _animationStage = 0;
    QTimer::singleShot(delays[_animationStage], this, SLOT(animate()));
    return;
  }

  if (++_animationStage < TIMER_ITERATIONS) {
    setColor(QColor(0, 255, 0).dark(dark[_animationStage]));
    QTimer::singleShot(delays[_animationStage], this, SLOT(animate()));
  } else {
    off();
  }
}

#undef TIMER_ITERATIONS
#include "kstdatanotifier.moc"
// vim: ts=2 sw=2 et
