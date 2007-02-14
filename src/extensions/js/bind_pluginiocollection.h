/***************************************************************************
                          bind_pluginiocollection.h
                             -------------------
    begin                : Apr 18 2005
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

#ifndef BIND_PLUGINIOCOLLECTION_H
#define BIND_PLUGINIOCOLLECTION_H

#include "bind_collection.h"

#include <plugin.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class PluginIOCollection
   @inherits Collection
   @description An array of Plugin inputs or outputs.
*/
class KstBindPluginIOCollection : public KstBindCollection {
  public:
    KstBindPluginIOCollection(KJS::ExecState *exec, const QValueList<Plugin::Data::IOValue>& data, bool input);
    ~KstBindPluginIOCollection();

    virtual KJS::Value length(KJS::ExecState *exec) const;

    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;

    QValueList<Plugin::Data::IOValue> _d;
    bool _input;
};


#endif

// vim: ts=2 sw=2 et
