/***************************************************************************
                                 bind_kst.h
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

#ifndef BIND_KST_H
#define BIND_KST_H

#include "kstbinding.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

class KstJS;

/* @class Kst
   @description The global Kst object.  Accessed as <i>Kst</i>, this cannot be
                reinstantiated.
*/
class KstBindKst : public KstBinding {
  public:
    KstBindKst(KJS::ExecState *exec, KJS::Object *globalObject = 0L, KstJS *ext = 0L);
    ~KstBindKst();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    /* @method loadScript
       @arg string fileName The file to load the script from.
       @description Loads a script file from disk and runs it.
       @returns boolean True if the file was successfully loaded and executed.
    */
    KJS::Value loadScript(KJS::ExecState *exec, const KJS::List& args);
    /* @method resetInterpreter
       @description Resets the KstScript interpreter.  All variables will be
                    lost.
    */
    KJS::Value resetInterpreter(KJS::ExecState *exec, const KJS::List& args);
    /* @method purge
       @description Purges unused objects in Kst.  The has the same behavior as
                    the purge button in the data manager.
    */
    KJS::Value purge(KJS::ExecState *exec, const KJS::List& args);

    /* @property DataSourceCollection dataSources
       @readonly
       @description The list of all loaded data sources.
    */
    KJS::Value dataSources(KJS::ExecState *exec) const;
    /* @property ScalarCollection scalars
       @readonly
       @description The list of all scalars in Kst.
    */
    KJS::Value scalars(KJS::ExecState *exec) const;
    /* @property StringCollection strings
       @readonly
       @description The list of all strings in Kst.
    */
    KJS::Value strings(KJS::ExecState *exec) const;
    /* @property VectorCollection vectors
       @readonly
       @description The list of all vectors in Kst.
    */
    KJS::Value vectors(KJS::ExecState *exec) const;
    /* @property WindowCollection windows
       @readonly
       @description The list of all Kst windows in this process.
    */
    KJS::Value windows(KJS::ExecState *exec) const;
    /* @property DataObjectCollection objects
       @readonly
       @description The list of all data objects in Kst.
    */
    KJS::Value objects(KJS::ExecState *exec) const;
    /* @property ColorSequence colors
       @readonly
       @description A reference to the Kst color sequence in its current state.
    */
    KJS::Value colors(KJS::ExecState *exec) const;
    /* @property ExtensionCollection extensions
       @readonly
       @description The list of all extensions Kst can find installed on the
                    system.
    */
    KJS::Value extensions(KJS::ExecState *exec) const;
    /* @property Document document
       @readonly
       @description An object that provides access to the current Kst document
                    along with various utility functions.
    */
    KJS::Value document(KJS::ExecState *exec) const;
    /* @property PluginManager pluginManager
       @readonly
       @description A reference to the plugin management subsystem of Kst.
    */
    KJS::Value pluginManager(KJS::ExecState *exec) const;
    /* @property QWidget gui
       @readonly
       @description A reference to the Kst GUI.  This is implemented using
                    KJSEmbed.  The GUI is dynamic and may change between
                    releases or even while Kst is running.
    */
    KJS::Value gui(KJS::ExecState *exec) const;

  protected:
    KstBindKst(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    KstJS *_ext;
};


#endif

// vim: ts=2 sw=2 et
