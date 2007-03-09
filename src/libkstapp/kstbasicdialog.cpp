/***************************************************************************
                     kstbasicdialog.cpp  -  Part of KST
                             -------------------
    begin                : 09/15/06
    copyright            : (C) 2006 The University of Toronto
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

// include files for Qt
#include <q3vbox.h>
#include <qlayout.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QGridLayout>
#include <Q3Frame>
#include <QLabel>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>

#include "kstbasicdialog.h"

// application specific includes
#include "kst.h"
#include "kstdoc.h"
#include "scalarselector.h"
#include "stringselector.h"
#include "vectorselector.h"
#include "kstdefaultnames.h"
#include "kstdataobjectcollection.h"

const QString& KstBasicDialogI::defaultTag = KGlobal::staticQString("<Auto Name>");

QPointer<KstBasicDialogI> KstBasicDialogI::_inst;

KstBasicDialogI *KstBasicDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstBasicDialogI(KstApp::inst());
  }
  return _inst;
}


KstBasicDialogI::KstBasicDialogI(QWidget* parent, Qt::WindowFlags fl)
: KstDataDialog(parent, fl) {
  setMultiple(false);

  _w = new Ui::KstBasicDialog;
  _w->setupUi(_contents);

  _pluginName = QString::null;
  _grid = 0L;
}


KstBasicDialogI::~KstBasicDialogI() {
  delete _w;
}

void KstBasicDialogI::init() {

  KstBasicPluginPtr ptr;
  if (_newDialog) {
    ptr = kst_cast<KstBasicPlugin>(KstDataObject::plugin(_pluginName));
  } else {
    ptr = kst_cast<KstBasicPlugin>(findObject(_pluginName));
  }

  Q_ASSERT(ptr); //shouldn't happen

  if (_grid) { //reset
    QLayoutIterator it = _grid->iterator();
    while(QLayoutItem *item = it.takeCurrent()) {
      delete item->widget();
      delete item;
    }
    delete _grid;
    _grid = 0;
  }

  int cnt = 0;
  int numInputOutputs = ptr->inputVectorList().count()
                      + ptr->inputScalarList().count()
                      + ptr->inputStringList().count()
                      + ptr->outputVectorList().count()
                      + ptr->outputScalarList().count()
                      + ptr->outputStringList().count();

  _grid = new QGridLayout(_w->_frame, numInputOutputs + 1, 2, 0, 8);
  _grid->setColumnStretch(1,1);
  _grid->setColumnStretch(0,0);

  //create input widgets
  //First, the inputVectors...
  QStringList iv = ptr->inputVectorList();
  QStringList::ConstIterator ivI = iv.begin();
  for (; ivI != iv.end(); ++ivI) {
      createInputVector(*ivI, ++cnt);
  }

  //Now, the inputScalars...
  QStringList is = ptr->inputScalarList();
  QStringList::ConstIterator isI = is.begin();
  for (; isI != is.end(); ++isI) {
      createInputScalar(*isI, ++cnt);
  }

  //Finally, the inputStrings...
  QStringList istr = ptr->inputStringList();
  QStringList::ConstIterator istrI = istr.begin();
  for (; istrI != istr.end(); ++istrI) {
      createInputString(*istrI, ++cnt);
  }

  //create sep
  cnt++;
  Q3Frame* line = new Q3Frame(_w->_frame);
  line->setFrameShadow(Q3Frame::Sunken);
  line->setFrameShape(Q3Frame::HLine);
  _grid->addMultiCellWidget(line, cnt, cnt, 0, 1);
  line->show();
  cnt++;

  //create output widgets
  //output vectors...
  QStringList ov = ptr->outputVectorList();
  QStringList::ConstIterator ovI = ov.begin();
  for (; ovI != ov.end(); ++ovI) {
      createOutputWidget(*ovI, ++cnt);
  }

  //output scalars...
  QStringList os = ptr->outputScalarList();
  QStringList::ConstIterator osI = os.begin();
  for (; osI != os.end(); ++osI) {
      createOutputWidget(*osI, ++cnt);
  }

  //ouput strings...
  QStringList ostr = ptr->outputStringList();
  QStringList::ConstIterator ostrI = ostr.begin();
  for (; ostrI != ostr.end(); ++ostrI) {
      createOutputWidget(*ostrI, ++cnt);
  }
}


void KstBasicDialogI::createInputVector(const QString &name, int row) {
  QLabel *label = new QLabel(name + ":", _w->_frame);

  VectorSelector *widget = new VectorSelector(_w->_frame,
                                              name.toLatin1());
  connect(widget, SIGNAL(newVectorCreated(const QString&)),
          this, SIGNAL(modified()));

  _grid->addWidget(label, row, 0);
  label->show();
  _grid->addWidget(widget, row, 1);
  widget->show();
}


void KstBasicDialogI::createInputScalar(const QString &name, int row) {
  QLabel *label = new QLabel(name + ":", _w->_frame);

  ScalarSelector *widget = new ScalarSelector(_w->_frame,
                                              name.toLatin1());
  connect(widget, SIGNAL(newScalarCreated()),
          this, SIGNAL(modified()));
  widget->allowDirectEntry(true);

  _grid->addWidget(label, row, 0);
  label->show();
  _grid->addWidget(widget, row, 1);
  widget->show();
}


void KstBasicDialogI::createInputString(const QString &name, int row) {
  QLabel *label = new QLabel(name + ":", _w->_frame);

  StringSelector *widget = new StringSelector(_w->_frame,
                                              name.toLatin1());
  connect(widget, SIGNAL(newStringCreated()),
          this, SIGNAL(modified()));

  _grid->addWidget(label, row, 0);
  label->show();
  _grid->addWidget(widget, row, 1);
  widget->show();
}


void KstBasicDialogI::createOutputWidget(const QString &name, int row) {
  QLabel *label = new QLabel(name + ":", _w->_frame);
  QLineEdit *widget = new QLineEdit(_w->_frame, name.toLatin1());
  _grid->addWidget(label, row, 0);
  label->show();
  _grid->addWidget(widget, row, 1);
  widget->show();
}


void KstBasicDialogI::update() {
  //called upon showing the dialog either in 'edit' mode or 'new' mode
}


bool KstBasicDialogI::newObject() {
  //called upon clicking 'ok' in 'new' mode
  //return false if the specified objects can't be made, otherwise true

  //Need to create a new object rather than use the one in KstDataObject pluginList
  KstBasicPluginPtr ptr = kst_cast<KstBasicPlugin>(
      KstDataObject::createPlugin(_pluginName));
  Q_ASSERT(ptr); //should never happen...

  KstWriteLocker pl(ptr);

  QString tagName = _tagName->text();

  if (tagName != defaultTag && KstData::self()->dataTagNameNotUnique(tagName, true, this)) {
    _tagName->setFocus();
    return false;
  }

  if (tagName == defaultTag) {
    tagName = KST::suggestPluginName(ptr->propertyString());
  }
  ptr->setTagName(KstObjectTag::fromString(tagName));

  // Save the vectors and scalars
  if (!editSingleObject(ptr) || !ptr->isValid()) {
    KMessageBox::sorry(this, i18n("There is an error in the values you entered."));
    return false;
  }

  //set the outputs
  //output vectors...
  QStringList ov = ptr->outputVectorList();
  QStringList::ConstIterator ovI = ov.begin();
  for (; ovI != ov.end(); ++ovI) {
    if (QLineEdit *w = output(*ovI)) {
      ptr->setOutputVector(*ovI, w->text());
    }
  }

  //output scalars...
  QStringList os = ptr->outputScalarList();
  QStringList::ConstIterator osI = os.begin();
  for (; osI != os.end(); ++osI) {
    if (QLineEdit *w = output(*osI)) {
      ptr->setOutputScalar(*osI, w->text());
    }
  }

  //ouput strings...
  QStringList ostr = ptr->outputStringList();
  QStringList::ConstIterator ostrI = ostr.begin();
  for (; ostrI != ostr.end(); ++ostrI) {
    if (QLineEdit *w = output(*ostrI)) {
      ptr->setOutputString(*ostrI, w->text());
    }
  }

  if (!ptr || !ptr->isValid()) {
    KMessageBox::sorry(this, i18n("There is an error in the plugin you entered."));
    return false;
  }

  ptr->setDirty();
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(ptr.data());
  KST::dataObjectList.lock().unlock();
  ptr = 0L; // drop the reference
  emit modified();

  return true;
}


bool KstBasicDialogI::editObject() {
  //called upon clicking 'ok' in 'edit' mode
  //return false if the specified objects can't be editted, otherwise true

  KstBasicPluginPtr ptr = kst_cast<KstBasicPlugin>(_dp);
  Q_ASSERT(ptr); //should never happen...

  ptr->writeLock();
  KstObjectTag newTag = KstObjectTag::fromString(_tagName->text());
  if (newTag != ptr->tag() && KstData::self()->dataTagNameNotUnique(_tagName->text())) {
    _tagName->setFocus();
    ptr->unlock();
    return false;
  }

  ptr->setTagName(newTag);

  ptr->inputVectors().clear();
  ptr->inputScalars().clear();
  ptr->inputStrings().clear();

  ptr->unlock();

  // Save the vectors and scalars
  if (!editSingleObject(ptr) || !ptr->isValid()) {
    KMessageBox::sorry(this, i18n("There is an error in the values you entered."));
    return false;
  }

  ptr->setDirty();

  emit modified();
  return true;
}


void KstBasicDialogI::showNew(const QString &field) {
  //Call init every time on showNew, because the field might equal propertyString()...
  _pluginName = field;
  _newDialog = true;
  init();
  KstDataDialog::showNew(field);
}


void KstBasicDialogI::showEdit(const QString &field) {
  if (_pluginName != field) {
    _pluginName = field;
    _newDialog = false;
    init();
  }
  KstDataDialog::showEdit(field);
}


bool KstBasicDialogI::editSingleObject(KstBasicPluginPtr ptr) {

  KstReadLocker vl(&KST::vectorList.lock());
  KstWriteLocker scl(&KST::scalarList.lock());
  KstReadLocker stl(&KST::stringList.lock());

  { // leave this scope here to destroy the iterators

    //input vectors...
    KstVectorList::Iterator v;
    QStringList iv = ptr->inputVectorList();
    QStringList::ConstIterator ivI = iv.begin();
    for (; ivI != iv.end(); ++ivI) {
      if (VectorSelector *w = vector(*ivI)) {
        v = KST::vectorList.findTag(w->selectedVector());
        if (v != KST::vectorList.end()) {
          ptr->setInputVector(*ivI, *v);
        }
      }
    }

    //input scalars...
    KstScalarList::Iterator s;
    QStringList is = ptr->inputScalarList();
    QStringList::ConstIterator isI = is.begin();
    for (; isI != is.end(); ++isI) {
      if (ScalarSelector *w = scalar(*isI)) {
        s = KST::scalarList.findTag(w->selectedScalar());
        if (s != KST::scalarList.end()) {
          ptr->setInputScalar(*isI, *s);
        } else {
          //Deal with direct entry
          bool ok;
          double val = w->_scalar->currentText().toDouble(&ok);
          if (ok) {
            KstScalarPtr sp = new KstScalar(KstObjectTag::fromString(w->_scalar->currentText()), 0L, val, true, false, false);  // FIXME: should this be a global-context scalar?
            ptr->setInputScalar(*isI, sp);
          } else {
            return false;
          }
        }
      }
    }

    //input strings...
    KstStringList::Iterator str;
    QStringList istr = ptr->inputStringList();
    QStringList::ConstIterator istrI = istr.begin();
    for (; istrI != istr.end(); ++istrI) {
      if (StringSelector *w = string(*istrI)) {
        str = KST::stringList.findTag(w->selectedString());
        if (str != KST::stringList.end()) {
          ptr->setInputString(*istrI, *str);
        }
      }
    }

  }

  return true;
}


void KstBasicDialogI::fillFieldsForEdit() {

  KstBasicPluginPtr ptr = kst_cast<KstBasicPlugin>(_dp);
  if (!ptr)
    return; //shouldn't happen

  ptr->readLock();

  _tagName->setText(ptr->tagName());
  _legendText->setText(defaultTag); //FIXME?

  //Update the various widgets...

  //input vectors...
  QStringList iv = ptr->inputVectorList();
  QStringList::ConstIterator ivI = iv.begin();
  for (; ivI != iv.end(); ++ivI) {
    KstVectorPtr p = ptr->inputVector(*ivI);
    QString t = p ? p->tag().displayString() : QString::null;
    if (VectorSelector *w = vector(*ivI)) {
      w->setSelection(t);
    }
  }

  //input scalars...
  QStringList is = ptr->inputScalarList();
  QStringList::ConstIterator isI = is.begin();
  for (; isI != is.end(); ++isI) {
    KstScalarPtr p = ptr->inputScalar(*isI);
    QString t = p ? p->tag().displayString() : QString::null;
    if (ScalarSelector *w = scalar(*isI)) {
      w->setSelection(t);
    }
  }

  //input strings...
  QStringList istr = ptr->inputStringList();
  QStringList::ConstIterator istrI = istr.begin();
  for (; istrI != istr.end(); ++istrI) {
    KstStringPtr p = ptr->inputString(*istrI);
    QString t = p ? p->tag().displayString() : QString::null;
    if (StringSelector *w = string(*istrI)) {
      w->setSelection(t);
    }
  }

  //output vectors...
  QStringList ov = ptr->outputVectorList();
  QStringList::ConstIterator ovI = ov.begin();
  for (; ovI != ov.end(); ++ovI) {
    KstVectorPtr p = ptr->outputVector(*ovI);
    QString t = p ? p->tagName() : QString::null;
    if (QLineEdit *w = output(*ovI)) {
      w->setText(t);
      w->setEnabled(false);
    }
  }

  //output scalars...
  QStringList os = ptr->outputScalarList();
  QStringList::ConstIterator osI = os.begin();
  for (; osI != os.end(); ++osI) {
    KstScalarPtr p = ptr->outputScalar(*osI);
    QString t = p ? p->tagName() : QString::null;
    if (QLineEdit *w = output(*osI)) {
      w->setText(t);
      w->setEnabled(false);
    }
  }

  //ouput strings...
  QStringList ostr = ptr->outputStringList();
  QStringList::ConstIterator ostrI = ostr.begin();
  for (; ostrI != ostr.end(); ++ostrI) {
    KstStringPtr p = ptr->outputString(*ostrI);
    QString t = p ? p->tagName() : QString::null;
    if (QLineEdit *w = output(*ostrI)) {
      w->setText(t);
      w->setEnabled(false);
    }
  }

  ptr->unlock();

  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstBasicDialogI::fillFieldsForNew() {
  _tagName->setText(defaultTag);
  _legendText->setText(defaultTag);

  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


VectorSelector *KstBasicDialogI::vector(const QString &name) const {
  return qobject_cast<VectorSelector*>(_w->_frame->child(name.toLatin1()));
}


ScalarSelector *KstBasicDialogI::scalar(const QString &name) const {
  return qobject_cast<ScalarSelector*>(_w->_frame->child(name.toLatin1()));
}


StringSelector *KstBasicDialogI::string(const QString &name) const {
  return qobject_cast<StringSelector*>(_w->_frame->child(name.toLatin1()));
}


QLineEdit *KstBasicDialogI::output(const QString &name) const {
  return qobject_cast<QLineEdit*>(_w->_frame->child(name.toLatin1()));
}

#include "kstbasicdialog.moc"

// vim: ts=2 sw=2 et
