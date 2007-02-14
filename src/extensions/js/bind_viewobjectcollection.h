/***************************************************************************
                         bind_viewobjectcollection.h
                             -------------------
    begin                : May 28 2005
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

#ifndef BIND_VIEWOBJECTCOLLECTION_H
#define BIND_VIEWOBJECTCOLLECTION_H

#include "bind_collection.h"

#include <kstviewobject.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class ViewObjectCollection
   @inherits Collection
   @description An array of ViewObjects.
*/
class KstBindViewObjectCollection : public KstBindCollection {
  public:
    KstBindViewObjectCollection(KJS::ExecState *exec, const KstViewObjectList& vectors);
    KstBindViewObjectCollection(KJS::ExecState *exec, KstViewObjectPtr parent);
    KstBindViewObjectCollection(KJS::ExecState *exec);
    ~KstBindViewObjectCollection();

    virtual KJS::Value length(KJS::ExecState *exec) const;

    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;
    virtual KJS::Value append(KJS::ExecState *exec, const KJS::List& args);
    virtual KJS::Value prepend(KJS::ExecState *exec, const KJS::List& args);
    virtual KJS::Value remove(KJS::ExecState *exec, const KJS::List& args);
    virtual KJS::Value clear(KJS::ExecState *exec, const KJS::List& args);

  protected:
    KstViewObjectList _objects;
    KstViewObjectPtr _parent;
};


#endif

// vim: ts=2 sw=2 et
