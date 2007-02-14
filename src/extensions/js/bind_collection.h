/***************************************************************************
                              bind_collection.h
                             -------------------
    begin                : Mar 31 2005
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

#ifndef BIND_COLLECTION_H
#define BIND_COLLECTION_H

#include "kstbinding.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Collection
   @description This is the generic collection class used by Kst to store lists
                of objects.  It behaves mostly like an array in JavaScript.  At
                this time indices are read-only.  In addition, if a collection
                is read-only, mutator methods will throw exceptions.

                Iteration looks something like this:
                <code><pre>
                for (i = 0; i < collection.length; ++i) {
                  // do something with collection[i]
                }
                </pre></code>
*/
class KstBindCollection : public KstBinding {
  public:
    KstBindCollection(KJS::ExecState *exec, const QString& name, bool readOnly = true);
    ~KstBindCollection();

    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::Value getPropertyByIndex(KJS::ExecState *exec, unsigned propertyName) const;
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    // Read-only right now
    //void putPropertyByIndex(KJS::ExecState *exec, unsigned propertyName, const KJS::Value &value, int attr = KJS::None);
    //void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    /* @method append
       @arg Object newObj The new object to append to the collection.
       @description Appends a new object to the end of the collection.
    */
    // default throws an exception
    virtual KJS::Value append(KJS::ExecState *exec, const KJS::List& args);
    /* @method prepend
       @arg Object newObj The new object to prepend to the collection.
       @description Prepends a new object to the start of the collection.
    */
    // default throws an exception
    virtual KJS::Value prepend(KJS::ExecState *exec, const KJS::List& args);
    /* @method remove
       @arg number n The index of the entry to remove.
       @description Removes an entry from the collection.
    */
    /* @method remove
       @arg Object obj The object to remove.
       @description Removes an object from the collection.
    */
    // default throws an exception
    virtual KJS::Value remove(KJS::ExecState *exec, const KJS::List& args);
    /* @method clear
       @description Clears the collection, removing all entries.
    */
    // default throws an exception
    virtual KJS::Value clear(KJS::ExecState *exec, const KJS::List& args);
    /* @property number length
       @readonly
       @description The number of items in the collection.  Items are ordered
                    0..(length - 1) so it is easy to iterate.
    */
    virtual KJS::Value length(KJS::ExecState *exec) const;
    /* @property boolean readOnly
       @readonly
       @description True if this is a read-only collection.
    */
    virtual KJS::Value readOnly(KJS::ExecState *exec) const;
    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;


  protected:
    KstBindCollection(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    bool _readOnly;
};


#endif

// vim: ts=2 sw=2 et
