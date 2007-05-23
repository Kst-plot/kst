/***************************************************************************
                   extensionmgr.h: Kst Extension Manager
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

#ifndef EXTENSIONMGR_H
#define EXTENSIONMGR_H

#include <kservice.h>
#include <kstaticdeleter.h>
#include "kstextension.h"

class KstApp;

class ExtensionMgr : QObject {
  Q_OBJECT
  friend class KStaticDeleter<ExtensionMgr>;

  public:
    static ExtensionMgr *self();

    void save();

    const QMap<QString,bool>& extensions() const { return _extensions; }
    void setEnabled(const QString& extension, bool enabled) { _extensions[extension] = enabled; }
    bool enabled(const QString& extension) { return _extensions.contains(extension) && _extensions[extension]; }
    void updateExtensions();

    KstExtension *extension(const QString& name) const;

    void doRegister(const QString& name, KstExtension *inst);
    void unregister(KstExtension *inst);
    void unregister(const QString& name) { _registry.remove(name); }
    bool registered(const QString& name) const { return _registry.contains(name); }

    void loadExtension(const QString& name);
    void loadExtension(const KService::Ptr& service);

    void setWindow(KstApp *w) { _window = w; }

  protected:
    ExtensionMgr();
    ~ExtensionMgr();
    static ExtensionMgr *_self;
    QMap<QString,bool> _extensions;
    QMap<QString,KstExtension*> _registry;
    KstApp *_window;

  private slots:
    void unregister();
};

#endif
// vim: ts=2 sw=2 et
