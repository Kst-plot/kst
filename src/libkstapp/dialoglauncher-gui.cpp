/***************************************************************************
                            dialoglauncher-gui.cpp
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

#include "dialoglauncher-gui.h"
#include "kstcsddialog.h"
#include "kstcurvedialog.h"
#include "ksteqdialog.h"
#include "ksthsdialog.h"
#include "kstimagedialog.h"
#include "kstmatrixdialog.h"
#include "kstplugindialog.h"
#include "kstbasicdialog.h"
#include "kstpsddialog.h"
#include "kstvectordialog.h"


KstGuiDialogs::KstGuiDialogs()
: KstDialogs() {
}


KstGuiDialogs::~KstGuiDialogs() {
}


void KstGuiDialogs::showHistogramDialog(const QString& name, bool edit) {
  if (!edit) {
    KstHsDialogI::globalInstance()->showNew(name);
  } else {
    KstHsDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::showCPluginDialog(const QString& name, bool edit) {
  if (!edit) {
    KstPluginDialogI::globalInstance()->showNew(name);
  } else {
    KstPluginDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::showBasicPluginDialog(const QString& name, bool edit) {
  if (!edit) {
    KstBasicDialogI::globalInstance()->showNew(name);
  } else {
    KstBasicDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::showEquationDialog(const QString& name, bool edit) {
  if (!edit) {
    KstEqDialogI::globalInstance()->showNew(name);
  } else {
    KstEqDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::showCSDDialog(const QString& name, bool edit) {
  if (!edit) {
    KstCsdDialogI::globalInstance()->showNew(name);
  } else {
    KstCsdDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::showPSDDialog(const QString& name, bool edit) {
  if (!edit) {
    KstPsdDialogI::globalInstance()->showNew(name);
  } else {
    KstPsdDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::newVectorDialog(QWidget *parent, const char *createdSlot, const char *selectedSlot, const char *updateSlot) {
  KstVectorDialogI *ad = new KstVectorDialogI(parent, "vector dialog");
  if (createdSlot) {
    QObject::connect(ad, SIGNAL(vectorCreated(KstVectorPtr)), parent, createdSlot);
  }
  if (selectedSlot) {
    QObject::connect(ad, SIGNAL(vectorCreated(KstVectorPtr)), parent, selectedSlot);
  }
  if (updateSlot) {
    QObject::connect(ad, SIGNAL(modified()), parent, updateSlot);
  }
  ad->show();
  ad->exec();
  delete ad;
}


void KstGuiDialogs::showVectorDialog(const QString& name, bool edit) {
  if (!edit) {
    KstVectorDialogI::globalInstance()->showNew(name);
  } else {
    KstVectorDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::newMatrixDialog(QWidget *parent, const char *createdSlot, const char *selectedSlot, const char *updateSlot) {
  KstMatrixDialogI *ad = new KstMatrixDialogI(parent, "matrix dialog");
  if (createdSlot) {
    QObject::connect(ad, SIGNAL(matrixCreated(KstMatrixPtr)), parent, createdSlot);
  }
  if (selectedSlot) {
    QObject::connect(ad, SIGNAL(matrixCreated(KstMatrixPtr)), parent, selectedSlot);
  }
  if (updateSlot) {
    QObject::connect(ad, SIGNAL(modified()), parent, updateSlot);
  }
  ad->show();
  ad->exec();
  delete ad;
}


void KstGuiDialogs::showMatrixDialog(const QString& name, bool edit) {
  if (!edit) {
    KstMatrixDialogI::globalInstance()->showNew(name);
  } else {
    KstMatrixDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::showImageDialog(const QString& name, bool edit) {
  if (!edit) {
    KstImageDialogI::globalInstance()->showNew(name);
  } else {
    KstImageDialogI::globalInstance()->showEdit(name);
  }
}


void KstGuiDialogs::showCurveDialog(const QString& name, bool edit) {
  if (!edit) {
    KstCurveDialogI::globalInstance()->showNew(name);
  } else {
    KstCurveDialogI::globalInstance()->showEdit(name);
  }
}

// vim: ts=2 sw=2 et
