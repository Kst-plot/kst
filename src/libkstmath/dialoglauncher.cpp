/***************************************************************************
                              dialoglauncher.cpp
                             -------------------
    begin                : Nov. 24, 2004
    copyright            : (C) 2004 The University of Toronto
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

#include "dialoglauncher.h"

#include <qapplication.h>

KstDialogs *KstDialogs::_self = 0L;
void KstDialogs::cleanup() {
    delete _self;
    _self = 0;
}


KstDialogs *KstDialogs::self() {
  if (!_self) {
    _self = new KstDialogs;
    qAddPostRoutine(KstDialogs::cleanup);
  }
  return _self;
}


void KstDialogs::replaceSelf(KstDialogs *newInstance) {
  delete _self;
  _self = 0L;
  _self = newInstance;
}


KstDialogs::KstDialogs() {
}


KstDialogs::~KstDialogs() {
}


void KstDialogs::showHistogramDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::showCPluginDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::showBasicPluginDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::showEquationDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::showCSDDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::showPSDDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::newVectorDialog(QWidget *parent, const char *createdSlot, const char *selectedSlot, const char *updateSlot) {
  Q_UNUSED(parent)
  Q_UNUSED(createdSlot)
  Q_UNUSED(selectedSlot)
  Q_UNUSED(updateSlot)
}


void KstDialogs::showVectorDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::newMatrixDialog(QWidget *parent, const char *createdSlot, const char *selectedSlot, const char *updateSlot) {
  Q_UNUSED(parent)
  Q_UNUSED(createdSlot)
  Q_UNUSED(selectedSlot)
  Q_UNUSED(updateSlot)
}


void KstDialogs::showMatrixDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::showImageDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


void KstDialogs::showCurveDialog(const QString& name, bool edit) {
  Q_UNUSED(name)
  Q_UNUSED(edit)
}


// vim: ts=2 sw=2 et
