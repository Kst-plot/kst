/***************************************************************************
                             bind_debuglogentry.h
                             -------------------
    begin                : Apr 07 2005
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

#ifndef BIND_DEBUGLOGENTRY_H
#define BIND_DEBUGLOGENTRY_H

#include "kstbinding.h"

#include <kstdebug.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class DebugLogEntry
   @description An entry in the Kst debug log.
*/
class KstBindDebugLogEntry : public KstBinding {
  public:
    KstBindDebugLogEntry(KJS::ExecState *exec, KstDebug::LogMessage msg);
    ~KstBindDebugLogEntry();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // properties
    /* @property string text
       @readonly
       @description Contains the text of this entry.
    */
    KJS::Value text(KJS::ExecState *exec) const;
    /* @property date date
       @readonly
       @description Contains the timestamp of this entry.
    */
    KJS::Value date(KJS::ExecState *exec) const;
    /* @property string level
       @readonly
       @description Contains the log level for this entry.
                    <ul>
                    <li>N - Notice</li>
                    <li>D - Debug</li>
                    <li>W - Warning</li>
                    <li>E - Error</li>
                    <li>A single space ' ' indicates an unknown entry type.</li>
                    </ul>
    */
    KJS::Value level(KJS::ExecState *exec) const;

  protected:
    KstBindDebugLogEntry(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    KstDebug::LogMessage _d;
};


#endif

// vim: ts=2 sw=2 et
