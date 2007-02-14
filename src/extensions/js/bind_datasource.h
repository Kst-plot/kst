/***************************************************************************
                              bind_datasource.h
                             -------------------
    begin                : Mar 28 2005
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

#ifndef BIND_DATASOURCE_H
#define BIND_DATASOURCE_H

#include "bind_object.h"

#include <kstdatasource.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class DataSource
   @inherits Object
   @collection DataSourceCollection 
   @description An object that represents a data file that is understood by
                Kst.
*/
class KstBindDataSource : public KstBindObject {
  public:
    KstBindDataSource(KJS::ExecState *exec, KstDataSourcePtr s);
    KstBindDataSource(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindDataSource();

    /* @constructor
       @arg string url The filename or URL to load.
       @optarg string type The name of the data source plugin to use.
       @description Creates a new DataSource object.  If the url cannot be
                    opened and read, <i>isValid</i> will be false.
    */
    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions
    /* @method isValidField
       @returns boolean
       @description Returns if the field is valid or not.
    */
    KJS::Value isValidField(KJS::ExecState *exec, const KJS::List& args);
    /* @method fieldList
       @returns array[string]
       @description Generates a list of the fields available from this source.
    */
    KJS::Value fieldList(KJS::ExecState *exec, const KJS::List& args);
    /* @method samplesPerFrame
       @arg string field A field name to get the number of
                         samples per frame for.  This is the same for every
                         field in some sources, but different in others.
       @returns number
       @description The number of samples per frame for this field or source.
    */
    KJS::Value samplesPerFrame(KJS::ExecState *exec, const KJS::List& args);
    /* @method frameCount
       @optarg string field An optional field name to get the number of
                            frames for.  This is the same for every
                            field in some sources, but different in others.
       @returns number
       @description Gets the number of frames for the source, or the given
                    field if it is specified.
    */
    KJS::Value frameCount(KJS::ExecState *exec, const KJS::List& args);

    // properties
    /* @property boolean valid
       @readonly
       @description True if the data source is valid.
    */
    KJS::Value valid(KJS::ExecState *exec) const;
    /* @property boolean empty
       @readonly
       @description True if the data source is empty.
    */
    KJS::Value empty(KJS::ExecState *exec) const;
    /* @property boolean completeFieldList
       @readonly
       @description True if the field list is complete.
    */
    KJS::Value completeFieldList(KJS::ExecState *exec) const;
    /* @property string fileName
       @readonly
       @description Name of the file.
    */
    KJS::Value fileName(KJS::ExecState *exec) const;
    /* @property string fileType
       @readonly
       @description The type (format) of the file, such as ASCII.
    */
    KJS::Value fileType(KJS::ExecState *exec) const;
    /* @property string source
       @readonly
       @description The name of the data source.
    */
    KJS::Value source(KJS::ExecState *exec) const;
    /* @property array[string] metaData
       @readonly
       @description Any metadata associated with the data source.  This is an
                    associative array of form metaData["key"] = "value".
    */
    KJS::Value metaData(KJS::ExecState *exec) const;

  protected:
    KstBindDataSource(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
};


#endif

// vim: ts=2 sw=2 et
