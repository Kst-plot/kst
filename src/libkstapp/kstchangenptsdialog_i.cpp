/***************************************************************************
                    kstchangenptsdialog_i.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of Toronto
                           (C) 2003 C. Barth Netterfield
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
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtimer.h>

#include <kcombobox.h>
#include <knuminput.h>

#include "datarangewidget.h"
#include "kstchangenptsdialog_i.h"
#include "kstdatacollection.h"
#include "kstrvector.h"

KstChangeNptsDialogI::KstChangeNptsDialogI(QWidget* parent,
                                           const char* name,
                                           bool modal,
                                           WFlags fl)
: KstChangeNptsDialog(parent, name, modal, fl) {
    connect(Clear,     SIGNAL(clicked()),
            CurveList, SLOT(clearSelection()));
    connect(SelectAll, SIGNAL(clicked()),
            this,      SLOT(selectAll()));
    connect(Apply,     SIGNAL(clicked()),
            this,      SLOT(applyNptsChange()));
    connect(OK,        SIGNAL(clicked()),
            this,      SLOT(OKNptsChange()));
    connect(CurveList, SIGNAL(selected ( int )),
            this,      SLOT(updateDefaults( int )));
}


KstChangeNptsDialogI::~KstChangeNptsDialogI() {
}


void KstChangeNptsDialogI::selectAll() {
  CurveList->selectAll(true);
}


bool KstChangeNptsDialogI::updateChangeNptsDialog() {
  QStringList qsl;
  int inserted = 0;
  
  for (uint i_vector = 0; i_vector < CurveList->count(); i_vector++) {
    if (CurveList->isSelected(i_vector)) {
      qsl.append(CurveList->text(i_vector));
    }
  }
  CurveList->clear();
  
  KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
  // insert vectors into ChangeNptsCurveList
  CurveList->blockSignals(true); // avoid deadlock
  for (KstRVectorList::ConstIterator i = rvl.begin(); i != rvl.end(); ++i) {
    KstRVectorPtr vector = *i;
    vector->readLock();
    QString tag = vector->tag().displayString();
    CurveList->insertItem(tag, -1);
    if (qsl.contains(tag)) {
      CurveList->setSelected(inserted, true);
    }
    ++inserted;
    vector->unlock();
  }
  CurveList->blockSignals(false);
  return !qsl.isEmpty();
}


void KstChangeNptsDialogI::showChangeNptsDialog() {
  bool some_slected = updateChangeNptsDialog();
  updateDefaults(0);
  if (!some_slected) {
    CurveList->selectAll(true);
  }
  show();
  raise();
}

void KstChangeNptsDialogI::OKNptsChange() {
  applyNptsChange();
  reject();
}

void KstChangeNptsDialogI::applyNptsChange() {
  KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
  for (uint i = 0; i < CurveList->count(); ++i) {
    if (CurveList->isSelected(i)) {
      KstRVectorPtr vector = *(rvl.findTag(CurveList->text(i)));
      if (vector) {
        int f0, n;

        vector->readLock();
        KstDataSourcePtr ds = vector->dataSource();
        if (_kstDataRange->isStartRelativeTime() && ds) {
          ds->readLock();
          f0 = ds->sampleForTime(_kstDataRange->f0Value());
          ds->unlock();
        } else if (_kstDataRange->isStartAbsoluteTime() && ds) {
          ds->readLock();
          f0 = ds->sampleForTime(_kstDataRange->f0DateTimeValue());
          ds->unlock();
        } else {
          f0 = int(_kstDataRange->f0Value());
        }

        if (_kstDataRange->isRangeRelativeTime() && ds) {
          ds->readLock();
          double nValStored = _kstDataRange->nValue();
          if (_kstDataRange->CountFromEnd->isChecked()) {
            int frameCount = ds->frameCount(vector->field());
            double msCount = ds->relativeTimeForSample(frameCount - 1);
            n = frameCount - 1 - ds->sampleForTime(msCount - nValStored);
          } else {
            double fTime = ds->relativeTimeForSample(f0);
            n = ds->sampleForTime(fTime + nValStored) - ds->sampleForTime(fTime);
          }
          ds->unlock();
        } else {
          n = int(_kstDataRange->nValue());
        }
        vector->unlock();

        vector->writeLock();
        vector->changeFrames(
          (_kstDataRange->CountFromEnd->isChecked() ? -1 : f0),
          (_kstDataRange->ReadToEnd->isChecked() ? -1 : n),
          _kstDataRange->Skip->value(),
          _kstDataRange->DoSkip->isChecked(),
          _kstDataRange->DoFilter->isChecked());
        vector->unlock();
      }
    }
  }

  // avoid re-entering the dialog
  QTimer::singleShot(0, this, SLOT(emitDocChanged()));
}


void KstChangeNptsDialogI::emitDocChanged() {
  emit docChanged();
}


void KstChangeNptsDialogI::updateDefaults(int index) {
  KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
  if (rvl.isEmpty() || index >= (int)rvl.count() || index < 0) {
    return;
  }

  KstRVectorPtr vector = rvl[index];
  vector->readLock();

  _kstDataRange->_startUnits->setCurrentItem(0);
  _kstDataRange->_rangeUnits->setCurrentItem(0);

  /* fill the vector range entries */
  _kstDataRange->CountFromEnd->setChecked(vector->countFromEOF());
  _kstDataRange->setF0Value(vector->reqStartFrame());

  /* fill number of frames entries */
  _kstDataRange->ReadToEnd->setChecked(vector->readToEOF());
  _kstDataRange->setNValue(vector->reqNumFrames());

  /* fill in frames to skip box */
  _kstDataRange->Skip->setValue(vector->skip());
  _kstDataRange->DoSkip->setChecked(vector->doSkip());
  _kstDataRange->DoFilter->setChecked(vector->doAve());
  _kstDataRange->updateEnables();

  vector->unlock();
}


void KstChangeNptsDialogI::updateTimeCombo() {
  KstRVectorList rvl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
  uint cnt = CurveList->count();
  bool supportsTime = true;
  for (uint i = 0; i < cnt; ++i) {
    if (CurveList->isSelected(i)) {
      KstRVectorPtr vector = *(rvl.findTag(CurveList->text(i)));
      if (vector) {
        vector->readLock();
        KstDataSourcePtr ds = vector->dataSource();
        vector->unlock();
        if (ds) {
          ds->readLock();
          supportsTime = ds->supportsTimeConversions();
          ds->unlock();
          if (!supportsTime) {
            break;
          }
        }
      }
    }
  }
  _kstDataRange->setAllowTime(supportsTime);
}


#include "kstchangenptsdialog_i.moc"
// vim: ts=2 sw=2 et
