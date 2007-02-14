/***************************************************************************
                           bind_objectcollection.h
                             -------------------
    begin                : May 31 2005
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

#ifndef BIND_OBJECTCOLLECTION_H
#define BIND_OBJECTCOLLECTION_H

#include "bind_collection.h"

#include <kstobject.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class ObjectCollection
   @inherits Collection
   @description An array of Objects.  This could contain any objects that 
                inherit from Object, but they are only immediately accessible
                in their pure base class form.
*/
class KstBindObjectCollection : public KstBindCollection {
  public:
    KstBindObjectCollection(KJS::ExecState *exec, const KstObjectList<KstObjectPtr>& vectors);
    KstBindObjectCollection(KJS::ExecState *exec);
    ~KstBindObjectCollection();

    virtual KJS::Value length(KJS::ExecState *exec) const;

    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;

  protected:
    KstObjectList<KstObjectPtr> _objects;
};


#endif

// vim: ts=2 sw=2 et
