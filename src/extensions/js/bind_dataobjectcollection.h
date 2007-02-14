/***************************************************************************
                         bind_dataobjectcollection.h
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

#ifndef BIND_DATAOBJECTCOLLECTION_H
#define BIND_DATAOBJECTCOLLECTION_H

#include "bind_collection.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class DataObjectCollection
   @inherits Collection
   @description An array of DataObjects.
*/
class KstBindDataObjectCollection : public KstBindCollection {
  public:
    KstBindDataObjectCollection(KJS::ExecState *exec);
    ~KstBindDataObjectCollection();

    KJS::Value length(KJS::ExecState *exec) const;

    QStringList collection(KJS::ExecState *exec) const;
    KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    KJS::Value extract(KJS::ExecState *exec, unsigned item) const;
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    KJS::Value plugins(KJS::ExecState *exec) const;
    KJS::Value powerSpectrums(KJS::ExecState *exec) const;
    KJS::Value equations(KJS::ExecState *exec) const;
    KJS::Value histograms(KJS::ExecState *exec) const;
};


#endif

// vim: ts=2 sw=2 et
