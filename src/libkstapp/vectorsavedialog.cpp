/***************************************************************************
                   vectorsavedialog.cpp
                             -------------------
    begin                : 02/28/07
    copyright            : (C) 2007 The University of Toronto
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

#include "vectorsavedialog.h"

#include <kdeversion.h>
#include <kfiledialog.h>
#include <k3tempfile.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include "kstdatacollection.h"
#include "kstvector.h"
#include "kstdatacollection.h"

#include <kst_export.h>

VectorSaveDialog::VectorSaveDialog(QWidget *parent)
    : QDialog(parent) {
  setupUi(this);

 connect(pushButton2, SIGNAL(clicked()), this, SLOT(hide()));

 connect(_saveButton, SIGNAL(clicked()), this, SLOT(save()));

 connect(_vectorList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

 connect(_vectorList, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(save()));
}


VectorSaveDialog::~VectorSaveDialog() {}


void VectorSaveDialog::save() {
  KstVectorList toSave;

  KST::vectorList.lock().readLock();
  for (Q3ListBoxItem *i = _vectorList->firstItem(); i; i = i->next()) {
    if (i->isSelected()) {
      KstVectorPtr v = *KST::vectorList.findTag(i->text());
      if (v) {
        toSave += v;
      }
    }
  }
  KST::vectorList.lock().unlock();

  KUrl url = KFileDialog::getSaveUrl(KUrl(), QString::null, this, i18n("Save Vector As"));
  if (!url.isEmpty()) {
    bool interpolate = true;
    switch (_multiOptions->currentIndex()) {
    case 0:
      interpolate = false;
    case 1: {
        K3TempFile tf(KStandardDirs::locateLocal("tmp", "kstvectors"), "txt");
        tf.setAutoDelete(true);
        if (0 != KstData::self()->vectorsToFile(toSave, tf.file(), interpolate)) {
          KMessageBox::sorry(this, i18n("Error saving vector to %1.").arg(url.prettyUrl()), i18n("Kst"));
          return;
        }
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
        tf.sync();
#else

        tf.close();
#endif

#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)

        if (KIO::NetAccess::exists(url, false, this)) {
#else
        if (KIO::NetAccess::exists(url)) {
#endif
          int rc = KMessageBox::warningYesNo(this, i18n("File %1 exists.  Overwrite?").arg(url.prettyUrl()), i18n("Kst"));
          if (rc == KMessageBox::No) {
            return;
          }
        }
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
        KIO::NetAccess::file_copy(KUrl(tf.name()), url, -1, true, false, this);
#else

        KIO::NetAccess::upload(tf.name(), url);
#endif

      }
      break;
    case 2: {
        unsigned n = 0;
        for (KstVectorList::Iterator i = toSave.begin(); i != toSave.end(); ++i) {
          KUrl url2 = url;
          if (toSave.count() > 1) {
            url2.setFileName(url.fileName() + QString(".%1").arg(++n));
          } else {
            url2.setFileName(url.fileName());
          }
          K3TempFile tf(KStandardDirs::locateLocal("tmp", "kstvectors"), "txt");
          tf.setAutoDelete(true);
          if (0 != KstData::self()->vectorToFile(*i, tf.file())) {
            KMessageBox::sorry(this, i18n("Error saving vector to %1.").arg(url2.prettyUrl()), i18n("Kst"));
            return;
          }
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
          tf.sync();
#else

          tf.close();
#endif

#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)

          if (KIO::NetAccess::exists(url2, false, this)) {
#else
          if (KIO::NetAccess::exists(url2)) {
#endif
            int rc = KMessageBox::warningYesNo(this, i18n("File %1 exists.  Overwrite?").arg(url2.prettyUrl()), i18n("Kst"));
            if (rc == KMessageBox::No) {
              continue;
            }
          }
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
          KIO::NetAccess::file_copy(KUrl(tf.name()), url2, -1, true, false, this);
#else

          KIO::NetAccess::upload(tf.name(), url2);
#endif

        }
      }
      break;
    default:
      KMessageBox::sorry(this, i18n("Internal error.  Please report."), i18n("Kst"));
      break;
    }
  }
}

void VectorSaveDialog::show() {
  init();
  QDialog::show();
}

void VectorSaveDialog::init() {
  _vectorList->clear();
  KST::vectorList.lock().readLock();
  for (KstVectorList::ConstIterator i = KST::vectorList.begin(); i != KST::vectorList.end(); ++i) {
    (*i)->readLock();
    _vectorList->insertItem((*i)->tag().displayString());
    (*i)->unlock();
  }
  KST::vectorList.lock().unlock();
  _saveButton->setEnabled(false);
}


void VectorSaveDialog::selectionChanged() {
  int cnt = 0;
  // expensive but qlistbox provides nothing better at the moment
  for (Q3ListBoxItem *i = _vectorList->firstItem(); i; i = i->next()) {
    if (i->isSelected()) {
      if (++cnt > 1) {
        break;
      }
    }
  }
  _multiOptions->setEnabled(cnt > 1);
  _saveButton->setEnabled(cnt > 0);
}

#include "vectorsavedialog.moc"

// vim: ts=2 sw=2 et
