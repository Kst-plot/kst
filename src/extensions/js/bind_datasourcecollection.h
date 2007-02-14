/***************************************************************************
                         bind_datasourcecollection.h
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

#ifndef BIND_DATASOURCECOLLECTION_H
#define BIND_DATASOURCECOLLECTION_H

#include "bind_collection.h"

#include <kstdatasource.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class DataSourceCollection
   @inherits Collection
   @description An array of DataSources.
*/
class KstBindDataSourceCollection : public KstBindCollection {
  public:
    KstBindDataSourceCollection(KJS::ExecState *exec, const KstDataSourceList& ds);
    KstBindDataSourceCollection(KJS::ExecState *exec);
    ~KstBindDataSourceCollection();

    virtual KJS::Value length(KJS::ExecState *exec) const;

    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;

  protected:
    QStringList _sources;
    bool _isGlobal;
};


#endif

// vim: ts=2 sw=2 et
