/***************************************************************************
                   vectorselector.cpp
                             -------------------
    begin                : 02/27/07
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

#include "vectorselector.h"

#include <QTimer>
#include <QListWidget>
#include <QAbstractItemView>

#include <kiconloader.h>

#include <kstdatacollection.h>
#include <kstcombobox.h>
#include <kstrvector.h>
#include <kstsvector.h>
#include <dialoglauncher.h>
#include <kstdataobject.h>

VectorSelector::VectorSelector(QWidget *parent)
    : QWidget(parent), _provideNoneVector(false) {
  setupUi(this);

  connect(_newVector, SIGNAL(clicked()), this, SLOT(createNewVector()));

  connect(_editVector, SIGNAL(clicked()), this, SLOT(editVector()));

  connect(this, SIGNAL(selectionChanged(const QString&)), this, SLOT(selectionWatcher(const QString&)));
}


VectorSelector::~VectorSelector() {}


void VectorSelector::init() {
  _newVector->setIcon(BarIcon("kst_vectornew"));
  _editVector->setIcon(BarIcon("kst_vectoredit"));
  _provideNoneVector = false;
  update();
  connect(_vector, SIGNAL(activated(const QString&)), this, SIGNAL(selectionChanged(const QString&))); // annoying that signal->signal doesn't seem to work in .ui files
}


void VectorSelector::allowNewVectors(bool allowed) {
  _newVector->setEnabled(allowed);
}


QString VectorSelector::selectedVector() {
  if (_provideNoneVector && _vector->currentIndex() == 0) {
    return QString::null;
  }
  return _vector->currentText();
}


void VectorSelector::update() {
  if (_vector->view()->isVisible()) {
    QTimer::singleShot(250, this, SLOT(update()));
    return;
  }

  blockSignals(true);
  QString prev = _vector->currentText();
  bool found = false;
  _vector->clear();
  if (_provideNoneVector) {
    _vector->addItem(tr("<None>"));
  }
  QStringList vectors;
  KST::vectorList.lock().readLock();
  //    kstdDebug() << "Populating Vector Selector" << endl;
  for (KstVectorList::ConstIterator i = KST::vectorList.begin(); i != KST::vectorList.end(); ++i) {
    (*i)->readLock();
    if (!(*i)->isScalarList()) {
      QString tag = (*i)->tag().displayString();
      //        kstdDebug() << "vector \"" << (*i)->tag().tagString() << "\": has display name \"" << (*i)->tag().displayString() << "\"" << endl;
      vectors << tag;
      if (!found && tag == prev) {
        found = true;
      }
    }
    (*i)->unlock();
  }
  KST::vectorList.lock().unlock();
  qSort(vectors);
  _vector->addItems(vectors);
  if (found) {
    _vector->setItemText(_vector->currentIndex(), prev);
  }
  blockSignals(false);
//FIXME PORT!
//   setEdit(_vector->currentText());
}


void VectorSelector::createNewVector() {
  KstDialogs::self()->newVectorDialog(this, SLOT(newVectorCreated(KstVectorPtr)), SLOT(setSelection(KstVectorPtr)), SLOT(update()));
}


void VectorSelector::selectionWatcher(const QString &tag) {
  QString label = "[" + tag + "]";
  emit selectionChangedLabel(label);
  setEdit(tag);
}


void VectorSelector::setSelection(const QString &tag) {
  if (tag.isEmpty()) {
    if (_provideNoneVector) {
      blockSignals(true);
      _vector->setCurrentIndex(0);
      blockSignals(false);

      _editVector->setEnabled(false);
    }
    return;
  }
  blockSignals(true);
  _vector->setItemText(_vector->currentIndex(), tag);  // What if it isn't in the combo?
  blockSignals(false);

  setEdit(tag);
}


void VectorSelector::newVectorCreated(KstVectorPtr v) {
  v->readLock();
  QString name = v->tagName();
  v->unlock();
  v = 0L; // deref
  emit newVectorCreated(name);
}


void VectorSelector::setSelection(KstVectorPtr v) {
  v->readLock();
  setSelection(v->tagName());
  v->unlock();
}


void VectorSelector::provideNoneVector(bool provide) {
  if (provide != _provideNoneVector) {
    _provideNoneVector = provide;
    update();
  }
}


void VectorSelector::editVector() {
  KST::vectorList.lock().readLock();
  KstVectorPtr vec = *KST::vectorList.findTag(_vector->currentText());
  KST::vectorList.lock().unlock();
  KstDataObjectPtr pro = 0L;
  if (vec) {
    pro = kst_cast<KstDataObject>(vec->provider());
  }
  if (pro) {
    pro->readLock();
    pro->showDialog(false);
    pro->unlock();
  } else {
    KstDialogs::self()->showVectorDialog(_vector->currentText(), true);
  }
}


void VectorSelector::setEdit(const QString &tag) {
  KST::vectorList.lock().readLock();
  KstVectorPtr vec = *KST::vectorList.findTag(tag);
  KST::vectorList.lock().unlock();
  KstRVectorPtr rvp = kst_cast<KstRVector>(vec);
  KstSVectorPtr svp = kst_cast<KstSVector>(vec);
  KstDataObjectPtr pro = 0L;
  if (vec) {
    pro = kst_cast<KstDataObject>(vec->provider());
  }
  _editVector->setEnabled(rvp||svp||pro);
}

#include "vectorselector.moc"

// vim: ts=2 sw=2 et
