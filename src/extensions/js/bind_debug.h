/***************************************************************************
                                bind_debug.h
                             -------------------
    begin                : Apr 04 2005
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

#ifndef BIND_DEBUG_H
#define BIND_DEBUG_H

#include "kstbinding.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

class KstJS;

/* @class Debug
   @description A reference to the debug and logging subsystem of Kst.
*/
class KstBindDebug : public KstBinding {
  public:
    KstBindDebug(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindDebug();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    /* @method warning
       @arg string message
       @description Logs a warning message to the Kst log.
    */
    KJS::Value warning(KJS::ExecState *exec, const KJS::List& args);
    /* @method error
       @arg string message
       @description Logs an error message to the Kst log.
    */
    KJS::Value error(KJS::ExecState *exec, const KJS::List& args);
    /* @method notice
       @arg string message
       @description Logs a notice to the Kst log.
    */
    KJS::Value notice(KJS::ExecState *exec, const KJS::List& args);
    /* @method debug
       @arg string message
       @description Logs a debug message to the Kst log.
    */
    KJS::Value debug(KJS::ExecState *exec, const KJS::List& args);
    /* @method clear
       @description Clears the Kst log.
    */
    KJS::Value clear(KJS::ExecState *exec, const KJS::List& args);

    /* @property DebugLog log
       @readonly
       @description Provides access to the Kst log contents.
    */
    KJS::Value log(KJS::ExecState *exec) const;

  protected:
    KstBindDebug(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
};


#endif

// vim: ts=2 sw=2 et
