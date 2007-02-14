/***************************************************************************
                              bind_dataobject.h
                             -------------------
    begin                : Apr 10 2005
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

#ifndef BIND_DATAOBJECT_H
#define BIND_DATAOBJECT_H

#include "bind_object.h"

#include <kstdataobject.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class DataObject
   @inherits Object
   @collection dataobjectcollection
   @description The abstract base class for all Kst data objects.
*/

class KstBindDataObject : public KstBindObject {
  public:
    KstBindDataObject(KJS::ExecState *exec, KstDataObjectPtr d, const char *name = 0L);
    KstBindDataObject(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindDataObject();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    int methodCount() const;
    int propertyCount() const;

    // member functions

    /* @property string type
       @readonly
       @description The type of data object this is.
    */
    KJS::Value type(KJS::ExecState *exec) const;
    /* @method convertTo
       @returns DataObject
       @arg string type The type to attempt to convert this object to.
       @description Attempts to convert this DaraObject to a different type.
                    The object must be derived from this type at some level.
                    Null is returned if it is not possible to convert to the
                    requested type.
    */
    KJS::Value convertTo(KJS::ExecState *exec, const KJS::List& args);

    static KstBindDataObject *bind(KJS::ExecState*, KstDataObjectPtr);

  protected:
    KstBindDataObject(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    static void addFactory(const QString&, KstBindDataObject*(*)(KJS::ExecState*, KstDataObjectPtr));

  private:
    static QMap<QString, KstBindDataObject*(*)(KJS::ExecState*, KstDataObjectPtr)> _factoryMap;
};


#endif

// vim: ts=2 sw=2 et
