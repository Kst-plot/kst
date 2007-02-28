/***************************************************************************
                     kstplugindialog_I.h  -  Part of KST
                             -------------------
    begin                : Mon May 12 2003
    copyright            : (C) 2003 The University of Toronto
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

#include <assert.h>

// include files for Qt
#include <qcombobox.h>
#include <q3frame.h>
#include <q3groupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qobject.h>
#include <q3textedit.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <q3vbox.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3GridLayout>

// include files for KDE
#include <kcolorbutton.h>
#include <klocale.h>
#include <kmessagebox.h>

// application specific includes
#include "kst.h"
#include "kstplugindialog_i.h"
#include "kstdataobjectcollection.h"
#include "kstdefaultnames.h"
#include "plugincollection.h"
#include "plugindialogwidget.h"
#include "pluginmanager.h"
#include "scalarselector.h"
#include "stringselector.h"
#include "vectorselector.h"

const QString& KstPluginDialogI::plugin_defaultTag = KGlobal::staticQString("<Auto Name>");

QPointer<KstPluginDialogI> KstPluginDialogI::_inst;

KstPluginDialogI *KstPluginDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstPluginDialogI(KstApp::inst());
  }
  return _inst;
}


KstPluginDialogI::KstPluginDialogI(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
: KstDataDialog(parent, name, modal, fl) {
  _w = new PluginDialogWidget(_contents);
  setMultiple(false);
  connect(_w->PluginCombo, SIGNAL(activated(int)), this, SLOT(pluginChanged(int)));
  connect(_w->_pluginManager, SIGNAL(clicked()), this, SLOT(showPluginManager()));

  _pluginInfoGrid = 0L;
  _pluginInputOutputGrid = 0L;
  _pluginName = QString::null;
}


KstPluginDialogI::~KstPluginDialogI() {
}


void KstPluginDialogI::updatePluginList() {
  PluginCollection *pc = PluginCollection::self();
  QString previous = _pluginList[_w->PluginCombo->currentItem()];
  int newFocus = -1;

  const QMap<QString,Plugin::Data>& pluginMap = pc->pluginList();

  QMap<QString,QString> oldIEntries, oldOEntries;

  _w->PluginCombo->clear();
  _pluginList.clear();
  int cnt = 0;
  Plugin::Data restoreEntry;
  for (QMap<QString,Plugin::Data>::ConstIterator it = pluginMap.begin();
                                                  it != pluginMap.end();
                                                                   ++it) {
    _pluginList += it.data()._name;
    _w->PluginCombo->insertItem(i18n("%1 (v%2)").arg(it.data()._readableName).arg(it.data()._version));
    if (it.data()._name == previous) {
      newFocus = cnt;
      oldIEntries = cacheInputs(it.data()._inputs);
      oldOEntries = cacheInputs(it.data()._outputs);
      restoreEntry = it.data();
    }
    ++cnt;
  }

  if (newFocus != -1) {
    _w->PluginCombo->setCurrentItem(newFocus);
    pluginChanged(_w->PluginCombo->currentItem());
    restoreInputs(restoreEntry._inputs, oldIEntries);
    restoreInputs(restoreEntry._outputs, oldOEntries);
  } else {
    _w->PluginCombo->setCurrentItem(0);
    pluginChanged(0);
  }
}


void KstPluginDialogI::updateForm() {
  KstSharedPtr<Plugin> plugin = PluginCollection::self()->plugin(_pluginList[_w->PluginCombo->currentItem()]);
  if (plugin) {
    const Q3ValueList<Plugin::Data::IOValue>& itable = plugin->data()._inputs;
    for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = itable.begin(); it != itable.end(); ++it) {
      if ((*it)._type == Plugin::Data::IOValue::TableType) { // vector
        QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "VectorSelector");
        assert(field);
        if (field) {
          VectorSelector *vs = static_cast<VectorSelector*>(field);
          vs->update();
        }
      } else if ((*it)._type == Plugin::Data::IOValue::StringType) { // string
        QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "StringSelector");
        assert(field);
        if (field) {
          StringSelector *ss = static_cast<StringSelector*>(field);
          ss->update();
        }
      } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
        // Nothing
      } else {
        QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "ScalarSelector");
        assert(field);
        if (field) {
          ScalarSelector *ss = static_cast<ScalarSelector*>(field);
          ss->update();
        }
      }
    }
  }
}


void KstPluginDialogI::fillFieldsForEdit() {
  KstCPluginPtr pp = kst_cast<KstCPlugin>(_dp);
  if (!pp) {
    return;
  }
  pp->readLock();
  if (!pp->plugin()) { // plugin() can be null if the kst file is invalid
    pp->unlock();
    return;
  }
  const QString pluginName(pp->tagName());
  const QString pluginObjectName(pp->plugin()->data()._name);
  const int usage = pp->getUsage();
  KstSharedPtr<Plugin> plug = pp->plugin();
  pp->unlock();

  _tagName->setText(pluginName);
  updatePluginList();

  int i = _pluginList.findIndex(pluginObjectName);
  _w->PluginCombo->setCurrentItem(i);
  pluginChanged(_w->PluginCombo->currentItem());

  fillVectorScalarCombos(plug);
  _w->PluginCombo->setEnabled(usage < 3);

  fixupLayout();
}


void KstPluginDialogI::fillFieldsForNew() {
  updatePluginList();
  int i = _pluginList.findIndex(_pluginName);
  _w->PluginCombo->setCurrentItem(i);
  pluginChanged(_w->PluginCombo->currentItem());
  _tagName->setText(plugin_defaultTag);
}


void KstPluginDialogI::fillVectorScalarCombos(KstSharedPtr<Plugin> plugin) {
  bool DPvalid = false;
  KstCPluginPtr pp = kst_cast<KstCPlugin>(_dp);

  if (pp) {
    pp->readLock();
    DPvalid = pp->isValid();
    pp->unlock();
  }

  if (plugin) {
    if (DPvalid) {
      pp->readLock();
    }
    // Update input vector and scalar combos
    const Q3ValueList<Plugin::Data::IOValue>& itable = plugin->data()._inputs;
    for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = itable.begin();
         it != itable.end(); ++it) {
      if ((*it)._type == Plugin::Data::IOValue::TableType) { // vector
        QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "VectorSelector");
        assert(field);
        if (field) {
          VectorSelector *vs = static_cast<VectorSelector*>(field);
          QString selectedVector = vs->selectedVector();
          vs->update();
          if (DPvalid) {
            vs->setSelection(pp->inputVectors()[(*it)._name]->tag().displayString());
          } else {
            vs->setSelection(selectedVector);
          }
        }
      } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
        QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "StringSelector");
        assert(field);
        if (field) {
          StringSelector *ss = static_cast<StringSelector*>(field);
          QString selectedString = ss->selectedString();
          ss->update();
          if (DPvalid) {
            ss->setSelection(pp->inputStrings()[(*it)._name]->tag().displayString());
          } else {
            ss->setSelection(selectedString);
          }
        }
      } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
        // Nothing
      } else {
        QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "ScalarSelector");
        assert(field);
        if (field) {
          ScalarSelector *ss = static_cast<ScalarSelector*>(field);
          QString selectedScalar = ss->selectedScalar();
          ss->update();
          if (DPvalid) {
            ss->setSelection(pp->inputScalars()[(*it)._name]->tag().displayString());
          } else {
            ss->setSelection(selectedScalar);
          }
        }
      }
    }

    // Update output vector and scalar lineedits
    if (DPvalid) {
      const Q3ValueList<Plugin::Data::IOValue>& otable = plugin->data()._outputs;
      for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = otable.begin(); it != otable.end(); ++it) {
        QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
        assert(field);
        if (field) {
          QLineEdit *li = static_cast<QLineEdit*>(field);
          QString ts;
          if ((*it)._type == Plugin::Data::IOValue::TableType) { // vector
            ts = pp->outputVectors()[(*it)._name]->tagName();
          } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
          } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
            ts = pp->outputStrings()[(*it)._name]->tagName();
          } else { // scalar
            ts = pp->outputScalars()[(*it)._name]->tagName();
          }
          li->setText(ts);
        }
      }
      pp->unlock();
    }
  } else { // invalid plugin
    PluginCollection *pc = PluginCollection::self();
    QString cur = _pluginList[_w->PluginCombo->currentItem()];
    Plugin::Data pdata = pc->pluginList()[pc->pluginNameList()[cur]];
    for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = pdata._outputs.begin(); it != pdata._outputs.end(); ++it) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
      if (field) {
        static_cast<QLineEdit*>(field)->setText(QString::null);
      }
    }
  }
}


void KstPluginDialogI::update() {
  updatePluginList();
}


void KstPluginDialogI::fixupLayout() {
  adjustSize();
  resize(650, minimumSizeHint().height());
  setFixedHeight(height());
}


QMap<QString,QString> KstPluginDialogI::cacheInputs(const Q3ValueList<Plugin::Data::IOValue>& table) {
  QMap<QString,QString> rc;
  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = table.begin(); it != table.end(); ++it) {
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "VectorSelector");
      if (field) {
        rc[(*it)._name] = static_cast<VectorSelector*>(field)->selectedVector();
      } else {
        field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
        if (field) {
          rc[(*it)._name] = static_cast<QLineEdit*>(field)->text();
        }
      }

    } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "StringSelector");
      if (field) {
        rc[(*it)._name] = static_cast<StringSelector*>(field)->selectedString();
      } else {
        field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
        if (field) {
          rc[(*it)._name] = static_cast<QLineEdit*>(field)->text();
        }
      }
    } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
      // Nothing
    } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "ScalarSelector");
      if (field) {
        rc[(*it)._name] = static_cast<ScalarSelector*>(field)->selectedScalar();
      } else {
        field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
        if (field) {
          rc[(*it)._name] = static_cast<QLineEdit*>(field)->text();
        }
      }
    }
  }
  return rc;
}


void KstPluginDialogI::restoreInputs(const Q3ValueList<Plugin::Data::IOValue>& table, const QMap<QString,QString>& v) {
  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = table.begin(); it != table.end(); ++it) {
    if (!v.contains((*it)._name)) {
      continue;
    }
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "VectorSelector");
      if (field) {
        static_cast<VectorSelector*>(field)->setSelection(v[(*it)._name]);
      } else {
        field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
        if (field) {
          static_cast<QLineEdit*>(field)->setText(v[(*it)._name]);
        }
      }
    } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "StringSelector");
      if (field) {
        static_cast<StringSelector*>(field)->setSelection(v[(*it)._name]);
      } else {
        field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
        if (field) {
          static_cast<QLineEdit*>(field)->setText(v[(*it)._name]);
        }
      }
    } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
      // Nothing
    } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "ScalarSelector");
      if (field) {
        static_cast<ScalarSelector*>(field)->setSelection(v[(*it)._name]);
      } else {
        field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
        if (field) {
          static_cast<QLineEdit*>(field)->setText(v[(*it)._name]);
        }
      }
    }
  }
}


bool KstPluginDialogI::saveInputs(KstCPluginPtr plugin, KstSharedPtr<Plugin> p) {
  bool rc = true;

  const Q3ValueList<Plugin::Data::IOValue>& itable = p->data()._inputs;
  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = itable.begin(); it != itable.end(); ++it) {
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "VectorSelector");
      assert(field);
      VectorSelector *vs = static_cast<VectorSelector*>(field);
      KstReadLocker vl(&KST::vectorList.lock());
      KstVectorPtr v = *KST::vectorList.findTag(vs->selectedVector());
      if (v) {
        plugin->inputVectors().insert((*it)._name, v);
      } else if (plugin->inputVectors().contains((*it)._name)) {
        plugin->inputVectors().erase((*it)._name);
        rc = false;
      }
    } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "StringSelector");
      assert(field);
      StringSelector *ss = static_cast<StringSelector*>(field);
      KstWriteLocker sl(&KST::stringList.lock());
      KstStringPtr s = *KST::stringList.findTag(ss->selectedString());
      if (s == *KST::stringList.end()) {
        QString val = ss->_string->currentText();
        KstStringPtr newString = new KstString(KstObjectTag::fromString(ss->_string->currentText()), 0L, val, true);
        if (!newString) {
          rc = false;
        }
        plugin->inputStrings().insert((*it)._name, newString);
      } else {
        plugin->inputStrings().insert((*it)._name, s);
      }
    } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
      // Nothing
    } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
      QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "ScalarSelector");
      assert(field);
      ScalarSelector *ss = static_cast<ScalarSelector*>(field);
      KstWriteLocker sl(&KST::scalarList.lock());
      KstScalarPtr s = *KST::scalarList.findTag(ss->selectedScalar());
      if (s == *KST::scalarList.end()) {
        bool ok;
        double val = ss->_scalar->currentText().toDouble(&ok);

        if (ok) {
          KstScalarPtr newScalar = new KstScalar(KstObjectTag::fromString(ss->_scalar->currentText()), 0L, val, true, false);
          plugin->inputScalars().insert((*it)._name, newScalar);
        } else {
          rc = false;
        }
      } else {
        plugin->inputScalars().insert((*it)._name, s);
      }
    } else {
    }
  }

  return rc;
}


bool KstPluginDialogI::saveOutputs(KstCPluginPtr plugin, KstSharedPtr<Plugin> p) {
  const Q3ValueList<Plugin::Data::IOValue>& otable = p->data()._outputs;

  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = otable.begin(); it != otable.end(); ++it) {
    QObject *field = _w->_pluginInputOutputFrame->child((*it)._name.latin1(), "QLineEdit");
    if (!field) {
      continue; // Some are unsupported
    }

    QLineEdit *li = static_cast<QLineEdit*>(field);

    if (li->text().isEmpty()) {
      li->setText((*it)._name);
    }

    QString nt = li->text();
    if ((*it)._type == Plugin::Data::IOValue::TableType) {
      if (!KstData::self()->vectorTagNameNotUnique(nt, false)) {
        // Implicitly creates it if it doesn't exist
        KstVectorPtr v = plugin->outputVectors()[(*it)._name];
        if (!v) {
          KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
          v = new KstVector(KstObjectTag(nt, plugin->tag()), 0, plugin.data());
          plugin->outputVectors().insert((*it)._name, v);
        }
        v->setTagName(KstObjectTag(nt, plugin->tag()));
      } else if (plugin->outputVectors()[(*it)._name]->tagName() != nt) {
        while (KstData::self()->vectorTagNameNotUnique(nt, false)) {
          nt += "'";
        }
        KstVectorPtr v;
        if (plugin->outputVectors().contains((*it)._name)) {
          v = plugin->outputVectors()[(*it)._name];
        } else {
          KstWriteLocker blockVectorUpdates(&KST::vectorList.lock());
          v = new KstVector(KstObjectTag(nt, plugin->tag()), 0, plugin.data());
          plugin->outputVectors().insert((*it)._name, v);
        }
        v->setTagName(KstObjectTag(nt, plugin->tag()));
      }
    } else if ((*it)._type == Plugin::Data::IOValue::StringType) {
      if (!KstData::self()->vectorTagNameNotUnique(nt, false)) {
        KstStringPtr s;
        if (plugin->outputStrings().contains((*it)._name)) {
          s = plugin->outputStrings()[(*it)._name];
        } else {
          KstWriteLocker blockStringUpdates(&KST::stringList.lock());
          s = new KstString(KstObjectTag(nt, plugin->tag()), plugin.data());
          plugin->outputStrings().insert((*it)._name, s);
        }
        s->setTagName(KstObjectTag(nt, plugin->tag()));

      } else if (plugin->outputStrings()[(*it)._name]->tagName() != nt) {
        while (KstData::self()->vectorTagNameNotUnique(nt, false)) {
          nt += "'";
        }
        KstStringPtr s;
        if (plugin->outputStrings().contains((*it)._name)) {
          s = plugin->outputStrings()[(*it)._name];
        } else {
          KstWriteLocker blockStringUpdates(&KST::stringList.lock());
          s = new KstString(KstObjectTag(nt, plugin->tag()), plugin.data());
          plugin->outputStrings().insert((*it)._name, s);
        }
        s->setTagName(KstObjectTag(nt, plugin->tag()));
      }
    } else if ((*it)._type == Plugin::Data::IOValue::PidType) {
      // Nothing
    } else if ((*it)._type == Plugin::Data::IOValue::FloatType) {
      if (!KstData::self()->vectorTagNameNotUnique(nt, false)) {
        KstScalarPtr s;
        if (plugin->outputScalars().contains((*it)._name)) {
          s = plugin->outputScalars()[(*it)._name];
        } else {
          KstWriteLocker blockScalarUpdates(&KST::scalarList.lock());
          s = new KstScalar(KstObjectTag(nt, plugin->tag()), plugin.data());
          plugin->outputScalars().insert((*it)._name, s);
        }
        s->setTagName(KstObjectTag(nt, plugin->tag()));
      } else if (plugin->outputScalars()[(*it)._name]->tagName() != nt) {
        while (KstData::self()->vectorTagNameNotUnique(nt, false)) {
          nt += "'";
        }
        KstScalarPtr s;
        if (plugin->outputScalars().contains((*it)._name)) {
          s = plugin->outputScalars()[(*it)._name];
        } else {
          KstWriteLocker blockScalarUpdates(&KST::scalarList.lock());
          s = new KstScalar(KstObjectTag(nt, plugin->tag()), plugin.data());
          plugin->outputScalars().insert((*it)._name, s);
        }
        s->setTagName(KstObjectTag(nt, plugin->tag()));
      }
    }
  }

  return true;
}


bool KstPluginDialogI::newObject() {
  QString tagName = _tagName->text();

  if (tagName != plugin_defaultTag && KstData::self()->dataTagNameNotUnique(tagName, true, this)) {
    _tagName->setFocus();
    return false;
  }
  KstCPluginPtr plugin;
  int pitem = _w->PluginCombo->currentItem();
  if (pitem >= 0 && _w->PluginCombo->count() > 0) {
    KstSharedPtr<Plugin> pPtr = PluginCollection::self()->plugin(_pluginList[pitem]);
    if (pPtr) {
      plugin = new KstCPlugin;
      KstWriteLocker pl(plugin);

      // set the tag name before any dependents are created
      if (tagName == plugin_defaultTag) {
        tagName = KST::suggestPluginName(_pluginList[pitem]);
      }
      plugin->setTagName(KstObjectTag(tagName, KstObjectTag::globalTagContext));

      if (!saveInputs(plugin, pPtr)) {
        KMessageBox::sorry(this, i18n("There is an error in the inputs you entered."));
        plugin = 0L;
        return false;
      }

      plugin->setPlugin(pPtr);

      if (!saveOutputs(plugin, pPtr)) {
        KMessageBox::sorry(this, i18n("There is an error in the outputs you entered."));
        plugin = 0L;
        return false;
      }
    }
  }

  if (!plugin || !plugin->isValid()) {
    KMessageBox::sorry(this, i18n("There is an error in the plugin you entered."));
    return false;
  }

  plugin->setDirty();
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(plugin.data());
  KST::dataObjectList.lock().unlock();
  plugin = 0L;
  emit modified();

  return true;
}


bool KstPluginDialogI::editObject() {
  KstCPluginPtr pp = kst_cast<KstCPlugin>(_dp);
  if (!pp) { // something is dreadfully wrong - this should never happen
    return false;
  }

  KstWriteLocker pl(pp);

  if (_tagName->text() != pp->tagName() && KstData::self()->dataTagNameNotUnique(_tagName->text())) {
    _tagName->setFocus();
    return false;
  }

  pp->setTagName(KstObjectTag(_tagName->text(), KstObjectTag::globalTagContext));  // FIXME: tag context always global?

  int pitem = _w->PluginCombo->currentItem();
  KstSharedPtr<Plugin> pPtr = PluginCollection::self()->plugin(_pluginList[pitem]);

  pp->inputVectors().clear();
  pp->inputScalars().clear();
  pp->inputStrings().clear();

  // Save the vectors and scalars
  if (!saveInputs(pp, pPtr)) {
    KMessageBox::sorry(this, i18n("There is an error in the inputs you entered."));
    return false;
  }

  if (pitem >= 0 && _w->PluginCombo->count() > 0) {
    pp->setPlugin(pPtr);
  }

  if (!saveOutputs(pp, pPtr)) {
    KMessageBox::sorry(this, i18n("There is an error in the outputs you entered."));
    return false;
  }

  if (!pp->isValid()) {
    KMessageBox::sorry(this, i18n("There is an error in the plugin you entered."));
    return false;
  }
  pp->setDirty();

  emit modified();
  return true;
}


void KstPluginDialogI::showNew(const QString &field) {
  //Call init every time on showNew, because the field might equal propertyString()...
  _pluginName = field;
  _newDialog = true;
  init();
  KstDataDialog::showNew(field);
}


void KstPluginDialogI::generateEntries(bool input, int& cnt, QWidget *parent, Q3GridLayout *grid, const Q3ValueList<Plugin::Data::IOValue>& table) {
  QString scalarLabelTemplate, vectorLabelTemplate, stringLabelTemplate;

  if (input) {
    stringLabelTemplate = i18n("Input string - %1:");
    scalarLabelTemplate = i18n("Input scalar - %1:");
    vectorLabelTemplate = i18n("Input vector - %1:");
  } else {
    stringLabelTemplate = i18n("Output string - %1:");
    scalarLabelTemplate = i18n("Output scalar - %1:");
    vectorLabelTemplate = i18n("Output vector - %1:");
  }

  for (Q3ValueList<Plugin::Data::IOValue>::ConstIterator it = table.begin(); it != table.end(); ++it) {
    QString labellabel;
    bool scalar = false;
    bool string = false;
    switch ((*it)._type) {
      case Plugin::Data::IOValue::PidType:
        continue;
      case Plugin::Data::IOValue::StringType:
        labellabel = stringLabelTemplate.arg((*it)._name);
        string = true;
        break;
      case Plugin::Data::IOValue::FloatType:
        labellabel = scalarLabelTemplate.arg((*it)._name);
        scalar = true;
        break;
      case Plugin::Data::IOValue::TableType:
        if ((*it)._subType == Plugin::Data::IOValue::FloatSubType ||
            (*it)._subType == Plugin::Data::IOValue::FloatNonVectorSubType) {
          labellabel = vectorLabelTemplate.arg((*it)._name);
        } else {
          // unsupported
          continue;
        }
        break;
      default:
        // unsupported
        continue;
    }

    QLabel *label = new QLabel(labellabel, parent, input ? "Input label" : "Output label");

    QWidget *widget = 0L;

    if (input) {
      if (scalar) {
        ScalarSelector *w = new ScalarSelector(parent, (*it)._name.latin1());
        widget = w;
        connect(w->_scalar, SIGNAL(activated(const QString&)), this, SLOT(updateScalarTooltip(const QString&)));
        connect(widget, SIGNAL(newScalarCreated()), this, SIGNAL(modified()));
        if (!(*it)._default.isEmpty()) {
          w->_scalar->insertItem((*it)._default);
          w->_scalar->setCurrentText((*it)._default);
        }
        KstScalarPtr p = *KST::scalarList.findTag(w->_scalar->currentText());
        w->allowDirectEntry(true);
        if (p) {
          p->readLock();
          QToolTip::remove(w->_scalar);
          QToolTip::add(w->_scalar, QString::number(p->value()));
          p->unlock();
        }
      } else if (string) {
        StringSelector *w = new StringSelector(parent, (*it)._name.latin1());
        widget = w;
        connect(w->_string, SIGNAL(activated(const QString&)), this, SLOT(updateStringTooltip(const QString&)));
        connect(widget, SIGNAL(newStringCreated()), this, SIGNAL(modified()));
        if (!(*it)._default.isEmpty()) {
          w->_string->insertItem((*it)._default);
          w->_string->setCurrentText((*it)._default);
        }
        KstStringPtr p = *KST::stringList.findTag(w->_string->currentText());
        w->allowDirectEntry(true);
        if (p) {
          p->readLock();
          QToolTip::remove(w->_string);
          QToolTip::add(w->_string, p->value());
          p->unlock();
        }
      } else {
        widget = new VectorSelector(parent, (*it)._name.latin1());
        connect(widget, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
      }
    } else {
      widget = new QLineEdit(parent, (*it)._name.latin1());
    }

    grid->addWidget(label, cnt, 0);
    _pluginWidgets.push_back(label);
    label->show();

    grid->addWidget(widget, cnt, 1);
    _pluginWidgets.push_back(widget);
    widget->show();

    if (!(*it)._description.isEmpty()) {
      Q3WhatsThis::remove(label);
      Q3WhatsThis::remove(widget);
      Q3WhatsThis::add(label, (*it)._description);
      Q3WhatsThis::add(widget, (*it)._description);
    }

    ++cnt;
  }
}


void KstPluginDialogI::pluginChanged(int idx) {

  // find all children and delete them and the grids
  while (!_pluginWidgets.isEmpty())
  {
    QWidget* tempWidget = _pluginWidgets.back();
    _pluginWidgets.pop_back();
    delete tempWidget;
  }
  delete _pluginInfoGrid;
  delete _pluginInputOutputGrid;

  // create new info grid
  _pluginInfoGrid = new Q3GridLayout(_w->_pluginInfoFrame, 2, 2, 0, 8);
  _pluginInfoGrid->setColStretch(1,1); // stretch the right column
  _pluginInfoGrid->setColStretch(0,0); // don't stretch the left column

  if (idx >= 0 && _w->PluginCombo->count() > 0) {

    // get the plugin data
    const QString& pluginName = _pluginList[idx];
    const Plugin::Data& pluginData = PluginCollection::self()->pluginList()[PluginCollection::self()->pluginNameList()[pluginName]];

    // Setup the grid and the "static" entries
    QLabel* infoLabel;

    infoLabel = new QLabel(i18n("Plugin name:"), _w->_pluginInfoFrame);
    _pluginInfoGrid->addWidget(infoLabel, 0, 0);
    _pluginWidgets.append(infoLabel);
    infoLabel->show();

    infoLabel = new QLabel(pluginData._readableName, _w->_pluginInfoFrame);
    _pluginInfoGrid->addWidget(infoLabel, 0, 1);
    _pluginWidgets.append(infoLabel);
    infoLabel->show();

    infoLabel = new QLabel(i18n("Description:"), _w->_pluginInfoFrame);
    infoLabel->setAlignment(Qt::AlignTop);
    _pluginInfoGrid->addWidget(infoLabel, 1, 0);
    _pluginWidgets.append(infoLabel);
    infoLabel->show();

    infoLabel = new QLabel(pluginData._description,  _w->_pluginInfoFrame);
    _pluginInfoGrid->addWidget(infoLabel, 1, 1);
    _pluginWidgets.append(infoLabel);
    infoLabel->show();

    // create a new inputoutput grid
    int cnt = 0;
    int numInputOutputs = pluginData._inputs.count() + pluginData._outputs.count();

    // generate inputs
    _pluginInputOutputGrid = new Q3GridLayout(_w->_pluginInputOutputFrame, numInputOutputs + 1, 2, 0, 8);
    _pluginInputOutputGrid->setColStretch(1,1);
    _pluginInputOutputGrid->setColStretch(0,0);
    generateEntries(true, cnt, _w->_pluginInputOutputFrame, _pluginInputOutputGrid, pluginData._inputs);

    // insert separator
    cnt++;
    Q3Frame* line = new Q3Frame(_w->_pluginInputOutputFrame);
    line->setFrameShadow(Q3Frame::Sunken);
    line->setFrameShape(Q3Frame::HLine);
    _pluginInputOutputGrid->addMultiCellWidget(line, cnt, cnt, 0, 1);
    _pluginWidgets.push_back(line);
    line->show();
    cnt++;

    // generate outputs
    _pluginInputOutputGrid->setColStretch(1,1);
    _pluginInputOutputGrid->setColStretch(0,0);
    generateEntries(false, cnt, _w->_pluginInputOutputFrame, _pluginInputOutputGrid, pluginData._outputs);
  }
  // resize everything
  fixupLayout();
}


void KstPluginDialogI::showPluginManager() {
  PluginManager *pm = new PluginManager(this, "Plugin Manager");
  pm->exec();
  delete pm;
  updatePluginList();
}


void KstPluginDialogI::updateScalarTooltip(const QString& n) {
  KstScalarPtr s = *KST::scalarList.findTag(n);
  QWidget *w = const_cast<QWidget*>(static_cast<const QWidget*>(sender()));
  if (s) {
    s->readLock();
    QToolTip::remove(w);
    QToolTip::add(w, QString::number(s->value()));
    s->unlock();
  } else {
    QToolTip::remove(w);
  }
}


void KstPluginDialogI::updateStringTooltip(const QString& n) {
  KstStringPtr s = *KST::stringList.findTag(n);
  QWidget *w = const_cast<QWidget*>(static_cast<const QWidget*>(sender()));
  if (s) {
    s->readLock();
    QToolTip::remove(w);
    QToolTip::add(w, s->value());
    s->unlock();
  } else {
    QToolTip::remove(w);
  }
}


#include "kstplugindialog_i.moc"

// vim: ts=2 sw=2 et
