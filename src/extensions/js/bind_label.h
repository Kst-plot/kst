/***************************************************************************
                              bind_label.h
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

#ifndef BIND_LABEL_H
#define BIND_LABEL_H

#include "bind_borderedviewobject.h"

#include <kstviewlabel.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Label
   @inherits BorderedViewObject
   @collection ViewObjectCollection
   @description A text label.
*/
class KstBindLabel : public KstBindBorderedViewObject {
  public:
    /* @constructor
       @arg ViewObject parent The parent to place the new label in.  May also
                              be a string containing the name of an existing
                              ViewObject.
       @description Creates a new label and places it in the ViewObject <i>parent</i>.
    */
    /* @constructor
       @arg Window window The window to place the new label in.  May also be a
                          string containing the name of an existing Window.
       @description Creates a new label and places it in the Window <i>window</i>.
    */
    KstBindLabel(KJS::ExecState *exec, KstViewLabelPtr d, const char *name = 0L);
    KstBindLabel(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindLabel();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @method adjustSizeForText
       @description Adjusts the size of the label to fit the text.
    */
    KJS::Value adjustSizeForText(KJS::ExecState *exec, const KJS::List& args);
    /* @property string text
       @description Contains the text contents of the label.  This may include
                    carriage returns (\n), scalar references of the form
                    <i>[scalar_name]</i>, and some basic LaTeX.
    */
    void setText(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value text(KJS::ExecState *exec) const;
    /* @property string font
       @description Used to set or get the current font used for the label.
    */
    void setFont(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value font(KJS::ExecState *exec) const;
    /* @property number fontSize
       @description Contains the size of the font used to draw the label.
    */
    void setFontSize(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value fontSize(KJS::ExecState *exec) const;
    /* @property number justification
       @description The justification for the label.  This is a bit field with
                    two segments, one for horizontal and one for vertical
                    justification.  The values are as follows:
                    <ul>
                    <li>0 - Justify None</li>
                    <li>1 - Justify Left / Top</li>
                    <li>2 - Justify Right / Bottom</li>
                    <li>3 - Justify Center</li>
                    </ul>
                    <p>Set the low order 8 bits to one of these values to
                    control horizontal justification.  Set the next 8 bits
                    to one of these to control vertical justification.
                    Example:</p>
                    <code><pre>
                    // Set justification to Right-Center
                    label.justification = 2 | (3 << 8);
                    </pre></code>
    */
    void setJustification(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value justification(KJS::ExecState *exec) const;
    /* @property number rotation
       @description Contains the rotation angle of the label.  Rotation is
                    clockwise from <i>normal</i> and is stored in degrees.
    */
    void setRotation(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value rotation(KJS::ExecState *exec) const;
    /* @property number dataPrecision
       @description Contains the number of digits of precision to display data
                    values in the label.  Restricted to a value between 0 and
                    16 inclusive.
    */
    void setDataPrecision(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value dataPrecision(KJS::ExecState *exec) const;
    /* @property boolean interpreted
       @description Determines if the contents of the label should be
                    interpreted in order to substitute LaTeX-like
                    constructs.  Default is true.
    */
    void setInterpreted(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value interpreted(KJS::ExecState *exec) const;
    /* @property boolean scalarReplacement
       @description Determines if the contents of the label should be
                    interpreted in order to substitute variable reference
                    constructs.  Default is true.
    */
    void setScalarReplacement(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value scalarReplacement(KJS::ExecState *exec) const;
    /* @property boolean autoResize
       @description Automatically resizes the label to fit the text exactly.
                    May trigger a slight move of the label.  It will not
                    automatically resize if the user resizes it manually via
                    the size property or layout mode.
       @obsolete
    */
    void setAutoResize(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value autoResize(KJS::ExecState *exec) const;

  protected:
    KstBindLabel(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
