/***************************************************************************
                             bind_picture.h
                             --------------
    begin                : Jun 14 2005
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

#ifndef BIND_PICTURE_H
#define BIND_PICTURE_H

#include "bind_borderedviewobject.h"

#include <kstviewpicture.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Picture
   @inherits BorderedViewObject
   @collection ViewObjectCollection
   @description A customizable picture object.
*/
class KstBindPicture : public KstBindBorderedViewObject {
  public:
    /* @constructor
       @arg ViewObject parent The parent to place the new picture in.  May also
                              be a string containing the name of an existing
                              ViewObject.
       @description Creates a new picture and places it in the ViewObject <i>parent</i>.
    */
    /* @constructor
       @arg Window window The window to place the new picture in.  May also be a
                          string containing the name of an existing Window.
       @optarg string url The URL of a file to load the picture from.
       @description Creates a new picture and places it in the Window <i>window</i>.
    */
    KstBindPicture(KJS::ExecState *exec, KstViewPicturePtr d, const char *name = 0L);
    KstBindPicture(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindPicture();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @method load
       @returns boolean
       @description Load an image from a give url.  Returns true on success.
       @arg string url The URL to retrieve the image from.
    */
    KJS::Value load(KJS::ExecState *exec, const KJS::List& args);
    /* @property Image image
       @description The raw image that is being displayed.  You can take a copy
                    of this, manipulate it, then set it back to change the
                    image.
    */
    void setImage(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value image(KJS::ExecState *exec) const;
    /* @property string url
       @readonly
       @description The URL of the image if it was loaded from a file and has
                    not been modified.
    */
    KJS::Value url(KJS::ExecState *exec) const;
    /* @property number refreshTimer
       @description A timer to automatically refresh the picture.  Value of 0
                    means the timer is disabled.  Units are in seconds.
    */
    void setRefreshTimer(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value refreshTimer(KJS::ExecState *exec) const;


  protected:
    KstBindPicture(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
