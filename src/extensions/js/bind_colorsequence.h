/***************************************************************************
                            bind_colorsequence.h
                             -------------------
    begin                : Apr 11 2005
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

#ifndef BIND_COLORSEQUENCE_H
#define BIND_COLORSEQUENCE_H

#include "kstbinding.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class ColorSequence
   @description Provides access to the Kst color sequence used for coloring
                curves.
*/

class KstBindColorSequence : public KstBinding {
  public:
    KstBindColorSequence(KJS::ExecState *exec);
    ~KstBindColorSequence();
 
    /* @constructor
       @description Default constructor.  Creates a new color sequence object.
    */
    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions
    /* @method next
       @optarg string previous The previous color in the color sequence you are
                               following.  Not necessary.
       @returns string
       @description Returns the next color in the color sequence.
    */
    KJS::Value next(KJS::ExecState *exec, const KJS::List& args);
   
    /* @method tooClose
       @arg string firstColor The first of two colors to compare.
       @arg string secondColor The second of two colors to compare.
       @returns boolean
       @description Returns true if colors are too close.
    */   
    KJS::Value tooClose(KJS::ExecState *exec, const KJS::List& args);

    // properties

  protected:
    KstBindColorSequence(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
};


#endif

// vim: ts=2 sw=2 et
