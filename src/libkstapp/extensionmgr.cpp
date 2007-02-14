/***************************************************************************
                   extensionmgr.cpp: Kst Extension Manager
                             -------------------
    begin                : Mar 30, 2004
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

// include files for KDE
#include <klocale.h>
#include <kparts/componentfactory.h>
#include <kservicetype.h>

// appliaction specific includes
#include "extensionmgr.h"
#include "kst.h"
#include "kstdebug.h"

static KStaticDeleter<ExtensionMgr> sdExtension;

ExtensionMgr *ExtensionMgr::self() {
  if (!_self) {
    sdExtension.setObject(_self, new ExtensionMgr);
  }

  return _self;
}


void ExtensionMgr::save() {
  KConfig cfg("kstextensionsrc", false, false);
  cfg.setGroup("Extensions");
  QStringList disabled;
  QStringList enabled;
  for (QMap<QString,bool>::ConstIterator i = _extensions.begin(); i != _extensions.end(); ++i) {
    if (i.data()) {
      enabled += i.key();
    } else {
      disabled += i.key();
    }
  }
  cfg.writeEntry("Disabled", disabled);
  cfg.writeEntry("Enabled", enabled);
}


ExtensionMgr::ExtensionMgr() : QObject(), _window(0L) {
  KConfig cfg("kstextensionsrc", true, false);
  cfg.setGroup("Extensions");
  QStringList disabled = cfg.readListEntry("Disabled");
  QStringList enabled = cfg.readListEntry("Enabled");
  for (QStringList::ConstIterator i = disabled.begin(); i != disabled.end(); ++i) {
    _extensions[*i] = false;
  }
  for (QStringList::ConstIterator i = enabled.begin(); i != enabled.end(); ++i) {
    _extensions[*i] = true;
  }
}


ExtensionMgr::~ExtensionMgr() {
  save();
  // we do not need to explicitly delete any remaining 
  // extensions as they are handled automatically...
}


KstExtension *ExtensionMgr::extension(const QString& name) const {
  QMap<QString,KstExtension*>::ConstIterator i = _registry.find(name);
  if (i != _registry.end()) {
    return *i;
  }
  return 0L;
}


void ExtensionMgr::loadExtension(const QString& name) {
  KService::List sl = KServiceType::offers("Kst Extension");
  for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
    KService::Ptr service = *it;
    if (name == service->property("Name").toString()) {
      loadExtension(service);
      return;
    }
  }
}


void ExtensionMgr::loadExtension(const KService::Ptr& service) {
  int err = 0;
  QString name = service->property("Name").toString();
  KstExtension *e = KParts::ComponentFactory::createInstanceFromService<KstExtension>(service, _window, 0, QStringList(), &err);
  if (e) {
    connect(e, SIGNAL(unregister()), this, SLOT(unregister()));
    KstDebug::self()->log(i18n("Kst Extension %1 loaded.").arg(name));
    doRegister(name,e);
  } else {
    KstDebug::self()->log(i18n("Error trying to load Kst extension %1.  Code=%2, \"%3\"").arg(name).arg(err).arg(err == KParts::ComponentFactory::ErrNoLibrary ? i18n("Library not found [%1].").arg(KLibLoader::self()->lastErrorMessage()) : KLibLoader::self()->lastErrorMessage()), KstDebug::Error);
  }
}


void ExtensionMgr::updateExtensions() {
  for (QMap<QString,bool>::ConstIterator i = _extensions.begin(); i != _extensions.end(); ++i) {
    QMap<QString,KstExtension*>::Iterator j = _registry.find(i.key());
    if (i.data()) {
      if (j == _registry.end()) {      
        loadExtension(i.key());
      }
    } else {
      if (j != _registry.end()) {      
        delete j.data();
        // Does this automatically
        //_registry.remove(j);
      }
    }
  }
}


void ExtensionMgr::doRegister(const QString& name, KstExtension *inst) {
  assert(!_registry.contains(name));
  _registry[name] = inst;
}


void ExtensionMgr::unregister(KstExtension *inst) {
  for (QMap<QString,KstExtension*>::Iterator i = _registry.begin(); i != _registry.end(); ++i) {
    if (i.data() == inst) {
      _registry.remove(i);
      break;
    }
  }
}


void ExtensionMgr::unregister() {
  unregister(const_cast<KstExtension*>(static_cast<const KstExtension*>(sender())));
}

ExtensionMgr *ExtensionMgr::_self = 0L;

#include "extensionmgr.moc"
// vim: ts=2 sw=2 et
